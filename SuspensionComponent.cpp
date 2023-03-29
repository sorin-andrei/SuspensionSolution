// Fill out your copyright notice in the Description page of Project Settings.


#include "SuspensionComponent.h"

// Sets default values for this component's properties
USuspensionComponent::USuspensionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	
}


// Called when the game starts
void USuspensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void USuspensionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DrawDebugVisuals();
	
}

void USuspensionComponent::ComputeSuspensionData(float DeltaTime, FTransform BodyTransform)
{
	//Define linetrace collision params
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	FHitResult Hit;

	FVector Up = BodyTransform.GetUnitAxis(EAxis::Z);

	//Calculate the end point of our line trace
	FVector TraceStart = ComponentWorldLocation;
	FVector TraceEnd = ComponentWorldLocation - (Up * Length);
	//FVector TraceEnd = ComponentWorldLocation - (GetOwner()->GetActorUpVector() * Length);


	//Check if suspension hit ground
	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, CollisionParams))
	{
		HitGround = true;

		//Perform suspension calculations
		CurrentCompression = (Length - Hit.Distance) / Length;
		SuspensionForce = ( ( (CurrentCompression - PreviousCompression) / DeltaTime) * Damping) + (CurrentCompression * Stiffness);
		PreviousCompression = CurrentCompression;

		//Debug visualisation variables
		TraceColor = HitTraceColor;
		DebugTraceStart = TraceStart;
		DebugTraceEnd = Hit.Location;
	}
	else
	{
		HitGround = false;
		//If the vehicle is in the air, add no suspension force
		CurrentCompression = 0;
		SuspensionForce = 0;
		PreviousCompression = 0;
		
		//Debug visualisation variables
		TraceColor = MissTraceColor;
		DebugTraceStart = TraceStart;
		DebugTraceEnd = TraceEnd;
	}
}

void USuspensionComponent::DrawDebugVisuals()
{
	if (!DrawDebug)
		return;
	DrawDebugLine(GetWorld(), DebugTraceStart, DebugTraceEnd, TraceColor, false, 0.f, 1, DebugLineThickness);
	DrawDebugString(GetWorld(), DebugTraceStart, FString::SanitizeFloat(CurrentCompression), nullptr, CompressionTextColor, 0.f);
}