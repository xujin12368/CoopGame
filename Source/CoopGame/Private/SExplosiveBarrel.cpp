// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "CoopGame.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	SphereComp->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetSimulatePhysics(true);

	MyHealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("MyHealthComp"));

	ImpuseStrength = 500.f;
	RadialStrength = 10000.f;

	bDestroyed = false;

}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	if (MyHealthComp)
	{
		MyHealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::ExplosiveToSelf);
	}
	
}

void ASExplosiveBarrel::ExplosiveToSelf(USHealthComponent* HealthComp, int32 Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bDestroyed)
	{
		return;
	}

	if (Health <= 0)
	{
		bDestroyed = true;

		MeshComp->AddImpulse(GetActorLocation().UpVector * ImpuseStrength, NAME_None, true);

		if (ExplosiveEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosiveEffect, GetActorLocation());
		}
	}
}

void ASExplosiveBarrel::ExplosiveToOtherActors()
{
	TArray<AActor*> OverlappingActors;
	SphereComp->GetOverlappingActors(OverlappingActors, MyClassFilter);

	for (auto OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor != this)
		{
			UStaticMeshComponent* MyRootComp = Cast<UStaticMeshComponent>(OverlappingActor->GetRootComponent());
			if (MyRootComp)
			{
				MyRootComp->AddRadialForce(
					GetActorLocation(), 
					SphereComp->GetScaledSphereRadius(), 
					RadialStrength, 
					ERadialImpulseFalloff::RIF_Linear, 
					true
				);
			}
		}
	}
}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bDestroyed)
	{
		return;
	}

	if (ChangeMaterial)
	{
		MeshComp->SetMaterial(0, ChangeMaterial);
	}

	ExplosiveToOtherActors();

}

