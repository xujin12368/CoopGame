// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUpActor.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerUpActor::ASPowerUpActor()
{
	PickInterval = 0.f;

	TotalNumOfTick = 10.f;

	NumOfTick = 0.f;

	bPowerActivated = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RotatingMovementComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComp"));
	RotatingMovementComp->RotationRate = FRotator(0.f, 180.f, 0.f);

	SetReplicates(true);

}

// Called when the game starts or when spawned
void ASPowerUpActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASPowerUpActor::OnTickPowerUp()
{
	NumOfTick++;

	OnPowerUpTicked();

	if (NumOfTick >= TotalNumOfTick)
	{
		OnExpired();

		GetWorldTimerManager().ClearTimer(TimerHandle_Pickup);

		bPowerActivated = false;
		OnRep_PowerActivate();
		
		Destroy();
	}
}

void ASPowerUpActor::OnRep_PowerActivate()
{
	OnPowerupActivateChanged(bPowerActivated);
}

void ASPowerUpActor::ActivatePowerUp(AActor* ActivateFor)
{
	OnActivated(ActivateFor);

	bPowerActivated = true;
	OnRep_PowerActivate();

	if (PickInterval > 0.f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Pickup, this, &ASPowerUpActor::OnTickPowerUp, PickInterval, true, 0.f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Your PickInterval is less than 0.f, So It only can exist once."));
		TotalNumOfTick = 1;
		OnTickPowerUp();
	}
}

float ASPowerUpActor::GetTotalNumOfTick() const
{
	return TotalNumOfTick;
}

float ASPowerUpActor::GetPickInterval() const
{
	return PickInterval;
}

void ASPowerUpActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerUpActor, bPowerActivated);
}

