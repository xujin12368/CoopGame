// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshComp"));
	RootComponent = WeaponMeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerParameter = "BeamEnd";
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		FHitResult OutHitResult;
		FVector TraceStart;
		FVector TraceEnd;
		FRotator OutRotation;
		MyOwner->GetActorEyesViewPoint(TraceStart, OutRotation);

		TraceEnd = TraceStart + OutRotation.Vector() * 10000;

		FVector TraceEndPoint = TraceEnd;
		
		FCollisionQueryParams QueryParams = new FCollisionQueryParams();
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		if (GetWorld()->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			// UGameplayStatics::ApplyDamage(OutHitResult.GetActor(), BaseDamage, MyOwner->GetInstigatorController(), this, DamageType);
			UGameplayStatics::ApplyPointDamage(
				OutHitResult.GetActor(), 
				BaseDamage, 
				OutRotation.Vector(), 
				OutHitResult, 
				MyOwner->GetInstigatorController(), 
				this, 
				DamageType
			);

			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, OutHitResult.ImpactPoint, OutHitResult.ImpactNormal.Rotation());
			}
			TraceEndPoint = OutHitResult.ImpactPoint;
		}
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.f, 0, 5.f);

		if (MuzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(
				MuzzleEffect,
				WeaponMeshComp,
				MuzzleSocketName
			);
		}

		if (TracerEffect)
		{
			FVector MuzzleLocation = WeaponMeshComp->GetSocketLocation(MuzzleSocketName);

			UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
			TracerComp->SetVectorParameter(TracerParameter, TraceEndPoint);
		}

	}
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

