// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/KismetMathLibrary.h"

#include "Components/SceneComponent.h"
#include "SuspensionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RACER_API USuspensionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USuspensionComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
		float Length = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
    	float Damping = 98000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	    float Stiffness = 980000.f;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	    bool DrawDebug;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		float DebugLineThickness = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	    FColor HitTraceColor = FColor(0,255,0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	    FColor MissTraceColor = FColor(255,0,0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	    FColor CompressionTextColor = FColor::White;

	//We can't just use GetWorldLocation(), need to calculate Async friendly world location in the vehicle class
	FVector ComponentWorldLocation;

	float GetSuspensionForce() { return SuspensionForce; };
	bool GetHitGround() { return HitGround; };

	//Where all the physics calculations take place
	void ComputeSuspensionData(float DeltaTime, FTransform BodyTransform);

	//Draw debug info
	void DrawDebugVisuals();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//Protected values
	UPROPERTY(BlueprintReadOnly)
	float CurrentCompression = 0.f;
	float PreviousCompression = 0.f;
	UPROPERTY(BlueprintReadOnly)
	float SuspensionForce;
	bool HitGround;

	//Debug properties
	UPROPERTY(BlueprintReadOnly)
	FVector DebugTraceStart;
	UPROPERTY(BlueprintReadOnly)
	FVector DebugTraceEnd;
	UPROPERTY(BlueprintReadOnly)
	FColor TraceColor;

	
		
};
