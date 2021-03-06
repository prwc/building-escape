// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Engine/TriggerVolume.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"
#include "Components/AudioComponent.h"

const float OpenedDoorYaw = 90.0f;
const float DefaultOpenSpeed = 90.0f;
const float DefaultCloseSpeed = DefaultOpenSpeed;
const float DefaultRequiredMass = 50.0f;

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
	: PressureVolume(nullptr),
	  OpenSpeed(DefaultOpenSpeed),
	  CloseSpeed(DefaultCloseSpeed),
	  RequiredMass(DefaultRequiredMass),
	  DoorSoundComponent(nullptr),
	  bDoorOpening(false),
	  bDoorClosed(true)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	FindDoorSoundComponent();

	if (PressureVolume == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Pressure Volume shouldn't be null. Please assign!"));
	}

	ClosedYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = ClosedYaw;
	OpenedYaw = ClosedYaw + OpenedDoorYaw;
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressureVolume != nullptr)
	{
		if (GetTotalOverlappingMass() >= RequiredMass)
		{
			OpenDoor(DeltaTime);
			return;
		}
	}

	CloseDoor(DeltaTime);
}

bool UOpenDoor::MoveDoor(float TargetYaw, float Speed, float DeltaTime)
{
	FRotator NewRotator = GetOwner()->GetActorRotation();
	CurrentYaw = NewRotator.Yaw;
	NewRotator.Yaw = FMath::FInterpConstantTo(NewRotator.Yaw, TargetYaw, DeltaTime, Speed);
	GetOwner()->SetActorRotation(NewRotator);
	return (NewRotator.Yaw == TargetYaw);
}

float UOpenDoor::GetTotalOverlappingMass() const
{
	TArray<UPrimitiveComponent *> OverlappingPrimitiveComponents;
	PressureVolume->GetOverlappingComponents(OverlappingPrimitiveComponents);
	float TotalMass = 0.0f;
	for (auto PrimitiveComponent : OverlappingPrimitiveComponents)
	{
		if (PrimitiveComponent->IsSimulatingPhysics(NAME_None))
		{
			TotalMass += PrimitiveComponent->GetMass();
		}
	}
	return TotalMass;
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	if (!bDoorOpening && DoorSoundComponent != nullptr)
	{
		DoorSoundComponent->Play();
	}
	bDoorClosed = false;
	bDoorOpening = true;
	MoveDoor(OpenedYaw, OpenSpeed, DeltaTime);
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	bool bMoveDoorClosed = MoveDoor(ClosedYaw, CloseSpeed, DeltaTime);
	if (!bDoorClosed && bMoveDoorClosed)
	{
		bDoorOpening = false;
		bDoorClosed = true;
		if (DoorSoundComponent != nullptr)
		{
			DoorSoundComponent->Play();
		}
	}
}

void UOpenDoor::FindDoorSoundComponent()
{
	DoorSoundComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if (DoorSoundComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Audio component not found in %s"), *GetOwner()->GetName());
	}
}
