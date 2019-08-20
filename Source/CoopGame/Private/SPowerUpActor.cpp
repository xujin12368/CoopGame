// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUpActor.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASPowerUpActor::ASPowerUpActor()
{
	PickInterval = 0.f;

	TotalNumOfTick = 10.f;

	NumOfTick = 0.f;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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
	}
}

void ASPowerUpActor::ActivatePowerUp()
{
	if (PickInterval > 0.f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Pickup, this, &ASPowerUpActor::OnTickPowerUp, PickInterval, true, 0.f);
	}
	else
	{
		OnTickPowerUp();
	}
}

