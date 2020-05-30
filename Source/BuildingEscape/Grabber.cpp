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
	: Reach(100.0f),
	  bDrawLineTrace(false),
	  PhysicsHandleComponent(nullptr),
	  InputComponent(nullptr)
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

	FindPhysicsHandleComponent();
	SetupInputComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateDebugDrawLinTrace();
	UpdatePhysicsHandle();
}

void UGrabber::Grab()
{
	FHitResult HitResult;
	if (LineTracePhysicsBodyActor(HitResult) && HitResult.GetActor() != nullptr)
	{
		AActor *HitActor = HitResult.GetActor();
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

void UGrabber::Release()
{
	if (PhysicsHandleComponent && PhysicsHandleComponent->GrabbedComponent)
	{
		UE_LOG(LogTemp, Display, TEXT("Released %s"), *PhysicsHandleComponent->GrabbedComponent->GetOwner()->GetName());
		PhysicsHandleComponent->ReleaseComponent();
	}
}

void UGrabber::FindLineTrace(FVector &Start, FVector &End) const
{
	FRotator ViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(Start, ViewPointRotation);
	End = Start + ViewPointRotation.Vector() * Reach;
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Found UInputComponent on %s"), *GetOwner()->GetName());
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No UInputComponent on %s"), *GetOwner()->GetName());
	}
}

void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandleComponent = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandleComponent != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Found UPhysicsHandleComponent on %s"), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No UPhysicsHandleComponent found on %s"), *GetOwner()->GetName());
	}
}

void UGrabber::UpdateDebugDrawLinTrace()
{
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
}

void UGrabber::UpdatePhysicsHandle()
{
	if (PhysicsHandleComponent && PhysicsHandleComponent->GrabbedComponent)
	{
		FVector StartLineTrace;
		FVector EndLineTrace;
		FindLineTrace(StartLineTrace, EndLineTrace);

		PhysicsHandleComponent->SetTargetLocation(EndLineTrace);
	}
}

bool UGrabber::LineTracePhysicsBodyActor(FHitResult &HitResult) const
{
	FVector StartLineTrace;
	FVector EndLineTrace;
	FindLineTrace(StartLineTrace, EndLineTrace);

	return GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		StartLineTrace,
		EndLineTrace,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		FCollisionQueryParams(
			NAME_None,
			false,
			GetOwner()));
}