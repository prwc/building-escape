// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OpenDoor.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BUILDINGESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	bool MoveDoor(float TargetYaw, float Speed, float DeltaTime);
	float GetTotalOverlappingMass() const;
	void FindDoorSoundComponent();
	void OpenDoor(float DeltaTime);
	void CloseDoor(float DeltaTime);

	UPROPERTY(EditAnywhere)
	class ATriggerVolume *PressureVolume;

	UPROPERTY(EditAnywhere)
	float OpenSpeed;

	UPROPERTY(EditAnywhere)
	float CloseSpeed;

	UPROPERTY(EditAnywhere)
	float RequiredMass;

	UPROPERTY()
	class UAudioComponent *DoorSoundComponent;

	UPROPERTY(VisibleAnywhere)
	bool bDoorOpening;

	UPROPERTY(VisibleAnywhere)
	bool bDoorClosed;

	float ClosedYaw;
	float CurrentYaw;
	float OpenedYaw;
};
