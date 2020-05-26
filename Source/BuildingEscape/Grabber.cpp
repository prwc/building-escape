// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
	: Reach(100.0f)
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
		UE_LOG(LogTemp, Display, TEXT("Found Physics handle component on %s"), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Physics handle component found on %s"), *GetOwner()->GetName());
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
		AActor *HitActor = HitResult.GetActor();
		if (HitActor != nullptr)
		{
			UE_LOG(LogTemp, Display, TEXT("Hit Actor %s"), *HitActor->GetName());
		}
	}
}
