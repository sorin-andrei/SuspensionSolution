// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values (Constructor)
AVehicle::AVehicle()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Add Car Body
	Body = CreateDefaultSubobject<UStaticMeshComponent>("Body");
	Body->SetSimulatePhysics(true);
	Body->SetEnableGravity(true);
	SetRootComponent(Body);

	//Setup the suspension components
	SuspensionFR = CreateDefaultSubobject<USuspensionComponent>("Suspension Front Right"); SuspensionFR->SetupAttachment(Body);
	SuspensionFL = CreateDefaultSubobject<USuspensionComponent>("Suspension Front Left"); SuspensionFL->SetupAttachment(Body);
	SuspensionRR = CreateDefaultSubobject<USuspensionComponent>("Suspension Rear Right"); SuspensionRR->SetupAttachment(Body);
	SuspensionRL = CreateDefaultSubobject<USuspensionComponent>("Suspension Rear Left"); SuspensionRL->SetupAttachment(Body);

	SuspensionArray = { SuspensionFR, SuspensionFL, SuspensionRR, SuspensionRL };
}

void AVehicle::BeginPlay()
{
	Super::BeginPlay();
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
}

void AVehicle::NativeAsyncTick(float DeltaTime)
{
	Super::NativeAsyncTick(DeltaTime);

	//Get the transform of our vehicle in an async friendly way
	FTransform BodyTransform = UAsyncTickFunctions::ATP_GetTransform(Body);

	//Get speed
	FVector Velocity = UAsyncTickFunctions::ATP_GetLinearVelocity(Body);
	Speed = FVector::DotProduct(Velocity, GetActorForwardVector());
	Speed *= 0.036; //convert cm/s (unreal unit/s) to km/h

	//Loop through each suspension component and apply the suspension logic
	for (USuspensionComponent* SuspensionComponent : SuspensionArray)
	{
		ApplySuspensionForces(SuspensionComponent, BodyTransform, DeltaTime);
	}
	//Apply vehicle forces
	ApplyTraction();
	ApplyThrottle();
	LookAhead(BodyTransform);
	ApplyTorque();
	
	
}

void AVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AVehicle::ApplySuspensionForces(USuspensionComponent* SuspensionComponent, FTransform TargetVehicleTransform, float DeltaTime)
{
/*
                                 Suspension logic, taking the following parameters
               -------------------------------------------------------------------------------------------------------------
			* SuspensionComponent >> The actual suspension object on which we want to perform the calculations
			* TargetVehicleTransform >> The world transform of our vehicle, calculated for use with async (ATP_GetTransform)
			* DeltaTime
*/

	//Get the world location of the suspension component in an Async physics friendly manner (We can't just use GetWorldLocation)
	SuspensionComponent->ComponentWorldLocation = UKismetMathLibrary::TransformLocation(TargetVehicleTransform, SuspensionComponent->GetRelativeLocation());

	//Perform suspension calculations, like compression, force
	SuspensionComponent->ComputeSuspensionData(DeltaTime, TargetVehicleTransform);

	//Apply the force to the vehicle body
	UAsyncTickFunctions::ATP_AddForceAtPosition(Body, SuspensionComponent->ComponentWorldLocation, SuspensionComponent->GetSuspensionForce() * GetActorUpVector());
}

void AVehicle::ApplyThrottle()
{
	if (ThrottleInput == 0)
		return;
	if (!IsGrounded())
		return;
	if (Speed >= SpeedCap)
		return;
	CurrentAccelerationForce = GetActorForwardVector() * Acceleration * ThrottleInput;
	UAsyncTickFunctions::ATP_AddForce(Body, CurrentAccelerationForce, false);
}

void AVehicle::ApplyTorque()
{
	if (SteeringInput == 0)
		return;
	CurrentSteeringForce = GetActorUpVector() * Torque * SteeringInput;
	UAsyncTickFunctions::ATP_AddTorque(Body, CurrentSteeringForce, false);
}

void AVehicle::ApplyTraction()
{
	FVector Velocity = UAsyncTickFunctions::ATP_GetLinearVelocity(Body);
	float CurrentDriftCoefficient = FVector::DotProduct(GetActorRightVector(), Velocity);
	FVector TractionForce = GetActorRightVector() * (-CurrentDriftCoefficient * Traction);
	UAsyncTickFunctions::ATP_AddForce(Body, TractionForce, false);
}

bool AVehicle::IsGrounded()
{
	for (USuspensionComponent* SuspensionComponent : SuspensionArray)
	{
		if (SuspensionComponent->GetHitGround())
			return true;
	}
	return false;
}

void AVehicle::LookAhead(FTransform VehicleTransform)
{
	//Convert look start origin - relative to world async
	
	FVector ATP_LookAheadOrigin = UKismetMathLibrary::TransformLocation(VehicleTransform, LookAheadOrigin);

	float NewLookAheadDistance = Speed * 2.7f;
	
	//Setup trace interval
	FVector TraceStart = ATP_LookAheadOrigin;
	FVector TraceEnd = TraceStart + GetActorForwardVector() * NewLookAheadDistance;
    
	LookAheadStartDebug = TraceStart;
	LookAheadEndDebug = TraceEnd;

	//Define linetrace collision params
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	FHitResult Hit;

	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, CollisionParams))
	{
		FVector StuntTorque = GetActorRightVector() * StuntTorqueApplied * -1.f;
		UAsyncTickFunctions::ATP_AddTorque(Body, StuntTorque, false);

		//FVector UpForce = GetActorUpVector() * UpForceAssist;
		//UAsyncTickFunctions::ATP_AddForce(Body, UpForce, false);
		
		ArrowColor = FColor(0, 255, 0);
		return;
	}
	ArrowColor = FColor(255, 0, 0);
	
}

void AVehicle::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Driving
		EnhancedInputComponent->BindAction(DriveAction, ETriggerEvent::Triggered, this, &AVehicle::SetDrivingInput);
		EnhancedInputComponent->BindAction(DriveAction, ETriggerEvent::Completed, this, &AVehicle::SetDrivingInput);
	}
	
}

void AVehicle::SetDrivingInput(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		ThrottleInput = MovementVector.Y;
		SteeringInput = MovementVector.X;
	}
}
