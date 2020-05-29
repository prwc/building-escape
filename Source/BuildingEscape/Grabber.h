// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Grabber.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGrabber();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	void Grab();
	void Release();
	void FindLineTrace(FVector &Start, FVector &End);
	void SetupInputComponent();
	void FindPhysicsHandleComponent();
	void UpdateDebugDrawLinTrace();
	void UpdatePhysicsHandle();
	bool LineTracePhysicsBodyActor(FHitResult &HitResult);

	UPROPERTY(EditAnywhere)
	float Reach;

	UPROPERTY(EditAnywhere)
	bool bDrawLineTrace;

	class UPhysicsHandleComponent *PhysicsHandleComponent;
	class UInputComponent *InputComponent;
};
