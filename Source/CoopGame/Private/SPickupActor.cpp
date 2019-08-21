// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/Character.h"
#include "SPowerUpActor.h"
#include "TimerManager.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	SphereComp->SetSphereRadius(75.f);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASPickupActor::HandleSphereOverlap);

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetupAttachment(RootComponent);
	DecalComp->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	DecalComp->DecalSize = FVector(75.f, 75.f, 75.f);

	PickUpInterval = 3.f;
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();

	Respawn();
	
}

void ASPickupActor::Respawn()
{
	if (!PowerUpInstance)
	{
		FActorSpawnParameters ActorSpawnParas;
		ActorSpawnParas.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		PowerUpInstance = GetWorld()->SpawnActor<ASPowerUpActor>(PowerUpActorClass, GetActorTransform(), ActorSpawnParas);
	}
}

void ASPickupActor::HandleSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// @TODO: ������ǿЧ��
	ACharacter* PlayerPawn = Cast<ACharacter>(OtherActor);
	if (PlayerPawn)
	{
		if (PowerUpInstance)
		{
			// ��ʱDestroy PowerUpInstance��
			// Destroy�Ļ�����û�к����ĺ��������ˡ�
			// ��ô�Ȳ��ÿ��������԰�
			PowerUpInstance->ActivatePowerUp();

			PowerUpInstance->SetActorLocation(FVector(0.f, 0.f, -10000.f));

			// ���õ���ˢ��ʱ��
			float PickUpDelayBeyond = PickUpDelay + PowerUpInstance->GetTotalNumOfTick() / PowerUpInstance->GetPickInterval();

			PowerUpInstance = nullptr;
			
			GetWorldTimerManager().SetTimer(TimerHandle_OnRespawn, this, &ASPickupActor::Respawn, PickUpInterval, true, PickUpDelayBeyond);
		}
	}
}

