// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SuspensionComponent.h"
#include "AsyncTickFunctions.h"

#include "InputActionValue.h"

#include "AsyncTickPawn.h"
#include "Vehicle.generated.h"

UCLASS()
class RACER_API AVehicle : public AAsyncTickPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVehicle();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NativeAsyncTick(float DeltaTime) override;

	//Create suspension components
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USuspensionComponent* SuspensionFR;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USuspensionComponent* SuspensionFL;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USuspensionComponent* SuspensionRR;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USuspensionComponent* SuspensionRL;

	//Driving input
	UPROPERTY(BlueprintReadOnly)
		float ThrottleInput;
	UPROPERTY(BlueprintReadOnly)
		float SteeringInput;

	//Input mapping context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;
	//Drive input action 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* DriveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LookAhead)
		FVector LookAheadOrigin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LookAhead)
		float LookAheadDistance = 400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LookAhead)
		float StuntTorqueApplied = 25000000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LookAhead)
		float UpForceAssist = 25000000.0f;

	UPROPERTY(BlueprintReadOnly)
		FVector LookAheadStartDebug;
	UPROPERTY(BlueprintReadOnly)
		FVector LookAheadEndDebug;

	   
	
protected:

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	virtual void BeginPlay() override;

	//Physics mesh on which all forces are added
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UStaticMeshComponent* Body;

	//Car handling properties
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Handling")
		float Acceleration = 500000.f; // to be replaced by acceleration curve
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Handling")
		float Torque = 50000000.f; // to be replaced by Torque curve
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Handling")
		float Traction = 50000.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Speed")
		float SpeedCap = 2000.f;


	UPROPERTY(BlueprintReadOnly)
		FColor ArrowColor;

	UPROPERTY(BlueprintReadOnly)
		float Speed;

	UPROPERTY(BlueprintReadOnly)
		FVector CurrentAccelerationForce;

	UPROPERTY(BlueprintReadOnly)
		FVector CurrentSteeringForce;

	//Vehicle state properties
	UFUNCTION()
	bool IsGrounded();

	UFUNCTION()
	void ApplySuspensionForces(USuspensionComponent* SuspensionComponent, FTransform TargetVehicleTransform, float DeltaTime);

	UFUNCTION()
	void ApplyThrottle();

	UFUNCTION()
	void ApplyTorque();

	UFUNCTION()
	void ApplyTraction();

	UFUNCTION()
	void SetDrivingInput(const FInputActionValue& Value);

	UFUNCTION()
	void LookAhead(FTransform VehicleTransform);

	UPROPERTY(BlueprintReadOnly)
	TArray<USuspensionComponent*> SuspensionArray;

	

};
