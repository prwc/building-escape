// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/PrimitiveComponent.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
	: Reach(100.0f), HitActor(nullptr), bDrawLineTrace(false)
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

	if (bDrawLineTrace)
	{
		FVector StartLineTrace;
		FVector EndLineTrace;
		FindLineTrace(StartLineTrace, EndLineTrace);

		DrawDebugLine(
			GetWorld(),
			StartLineTrace,
			EndLineTrace,
			FColor::Red,
			false,
			0.0f,
			0,
			5.0f);
	}

	if (PhysicsHandleComponent && PhysicsHandleComponent->GrabbedComponent)
	{
		FVector StartLineTrace;
		FVector EndLineTrace;
		FindLineTrace(StartLineTrace, EndLineTrace);

		PhysicsHandleComponent->SetTargetLocation(EndLineTrace);
	}
}

void UGrabber::Grab()
{
	if (PhysicsHandleComponent && PhysicsHandleComponent->GrabbedComponent)
	{
		UE_LOG(LogTemp, Display, TEXT("Released %s"), *PhysicsHandleComponent->GrabbedComponent->GetOwner()->GetName());
		PhysicsHandleComponent->ReleaseComponent();
		return;
	}

	FVector StartLineTrace;
	FVector EndLineTrace;
	FindLineTrace(StartLineTrace, EndLineTrace);

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByObjectType(
			HitResult,
			StartLineTrace,
			EndLineTrace,
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

	if (HitActor != nullptr)
	{
		UPrimitiveComponent *PrimitiveComponent = HitActor->FindComponentByClass<UPrimitiveComponent>();
		if (PrimitiveComponent != nullptr)
		{
			UE_LOG(LogTemp, Display, TEXT("Grab %s"), *PrimitiveComponent->GetOwner()->GetName());
			PhysicsHandleComponent->GrabComponentAtLocation(PrimitiveComponent, NAME_None, HitActor->GetActorLocation());
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("No target to grab."));
	}
}

void UGrabber::FindLineTrace(FVector &Start, FVector &End)
{
	FRotator ViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(Start, ViewPointRotation);
	End = Start + ViewPointRotation.Vector() * Reach;
}