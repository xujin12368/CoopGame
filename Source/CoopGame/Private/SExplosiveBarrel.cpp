// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "Net/UnrealNetwork.h"
#include "CoopGame.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetSimulatePhysics(true);
	// MeshComp->SetIsReplicated(true); // 可同步物理移动，但是延迟太高了

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(RootComponent);
	SphereComp->SetSphereRadius(300.f);
	SphereComp->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	MyHealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("MyHealthComp"));

	ImpuseStrength = 500.f;
	RadialStrength = 10000.f;

	bDestroyed = false;

	bCanExplosive = false;

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		if (MyHealthComp)
		{
			MyHealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::HandleChangedHealth);
		}
	}
	
}

void ASExplosiveBarrel::HandleChangedHealth(USHealthComponent* HealthComp, int32 Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

	if (bDestroyed)
	{
		return;
	}

	if (Health <= 0)
	{
		bDestroyed = true;

		bCanExplosive = true;

		ExplosiveToSelf();
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

void ASExplosiveBarrel::ExplosiveToSelf()
{
	MeshComp->AddImpulse(GetActorLocation().UpVector * ImpuseStrength, NAME_None, true);

	if (ExplosiveEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosiveEffect, GetActorLocation());
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

	if (bCanExplosive)
	{
		if (Role < ROLE_Authority)
		{
			ExplosiveToSelf();

			bCanExplosive = false;
		}
	}

	if (ChangeMaterial)
	{
		MeshComp->SetMaterial(0, ChangeMaterial);
	}

	ExplosiveToOtherActors();

}

void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASExplosiveBarrel, bDestroyed);

	DOREPLIFETIME(ASExplosiveBarrel, bCanExplosive);

}