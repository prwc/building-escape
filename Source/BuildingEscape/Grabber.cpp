// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
	: Reach(100.0f), HitActor(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	PhysicsHandleComponent = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandleComponent != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Found UPhysicsHandleComponent on %s"), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No UPhysicsHandleComponent found on %s"), *GetOwner()->GetName());
	}

	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Found UInputComponent on %s"), *GetOwner()->GetName());
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No UInputComponent on %s"), *GetOwner()->GetName());
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector ViewPointLocation;
	FRotator ViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(ViewPointLocation, ViewPointRotation);

	FVector LineTraceEnd = ViewPointLocation + ViewPointRotation.Vector() * Reach;

	DrawDebugLine(
		GetWorld(),
		ViewPointLocation,
		LineTraceEnd,
		FColor::Red,
		false,
		0.0f,
		0,
		5.0f);

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByObjectType(
			HitResult,
			ViewPointLocation,
			LineTraceEnd,
			FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
			FCollisionQueryParams(
				FName(TEXT("")),
				false,
				GetOwner())))
	{
		HitActor = HitResult.GetActor();
	}
	else
	{
		HitActor = nullptr;
	}
}

void UGrabber::Grab()
{
	if (HitActor != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Grab %s"), *HitActor->GetName());

		UStaticMeshComponent *StaticMeshComponent = HitActor->FindComponentByClass<UStaticMeshComponent>();
		if (StaticMeshComponent != nullptr)
		{
			UE_LOG(LogTemp, Display, TEXT("Found UStaticMeshComponent on %s"), *StaticMeshComponent->GetOwner()->GetName());
			StaticMeshComponent->AddForce(FVector::UpVector * 100000.0f * StaticMeshComponent->GetMass());
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("No target to grab."));
	}
}