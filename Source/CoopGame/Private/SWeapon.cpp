// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame.h"
#include "TimerManager.h"

static int32 DrawDebugWeapon = 0;
FAutoConsoleVariableRef CVARDrawDebugWeapon(
	TEXT("COOP.DrawDebugWeapon"), 
	DrawDebugWeapon, 
	TEXT("Draw Debug Line For Weapons."), 
	EConsoleVariableFlags::ECVF_Cheat
);

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshComp"));
	RootComponent = WeaponMeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerParameter = "BeamEnd";
	BaseDamage = 20.f;

	FireRate = 600.f;

	BulletUpperLimit = 30;
	CurrentBullet = 30;
	TotalBullets = 90;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	FireRateTime = 60.f / FireRate;
}

void ASWeapon::Fire()
{
	ConsumeBullet();
	if (!IsCanFire())
	{
		return;
	}

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
		QueryParams.bReturnPhysicalMaterial = true;
		if (GetWorld()->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			// Get Physical Surface Material
			EPhysicalSurface PhysicalSurfaceMat = UPhysicalMaterial::DetermineSurfaceType(OutHitResult.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if (PhysicalSurfaceMat == SURFACETYPE_FLESHHEADSHOT)
			{
				ActualDamage *= 4.f;
			}

			// UGameplayStatics::ApplyDamage(OutHitResult.GetActor(), BaseDamage, MyOwner->GetInstigatorController(), this, DamageType);
			UGameplayStatics::ApplyPointDamage(
				OutHitResult.GetActor(), 
				ActualDamage,
				OutRotation.Vector(), 
				OutHitResult, 
				MyOwner->GetInstigatorController(), 
				this, 
				DamageType
			);

			PlayImpactEffect(PhysicalSurfaceMat, OutHitResult);

			TraceEndPoint = OutHitResult.ImpactPoint;
		}

		if (DrawDebugWeapon > 0)
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.f, 0, 1.f);
		}

		PlayFireEffect(TraceEndPoint);

		SetLastFireTime();
	}
}

void ASWeapon::StartFire()
{
	// 此处使用FireRateTIme来约束FireDalay
	// 解决一直点击造成比自动射击还快的问题
	// 原理：一直点击的话，LastFireTime - GetWorld()->TimeSeconds == 0
	// 因此，就会由FireRateTime来控制Delay，所以无论怎么点击间隔都不会超过FireRateTime
	float FireDelay = FMath::Max<float>(0.f, LastFireTime + FireRateTime - GetWorld()->TimeSeconds);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_BetweedTowShots, this, &ASWeapon::Fire, FireRateTime, true, FireDelay);
}

void ASWeapon::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_BetweedTowShots);
}

void ASWeapon::SetLastFireTime()
{
	LastFireTime = GetWorld()->TimeSeconds;
}

void ASWeapon::ConsumeBullet()
{
	CurrentBullet = FMath::Clamp<int32>(--CurrentBullet, 0, BulletUpperLimit);
	if (CurrentBullet == 0)
	{
		bCanFire = false;
	}
	else
	{
		bCanFire = true;
	}
}

void ASWeapon::ProcessWeaponBullet()
{
	int32 DifferBetweenLimitAndCurrent = BulletUpperLimit - CurrentBullet;
	if (DifferBetweenLimitAndCurrent > 0 && TotalBullets > 0)
	{
		if (TotalBullets <= DifferBetweenLimitAndCurrent)
		{
			CurrentBullet += TotalBullets;
			TotalBullets = 0;
		}
		else
		{
			CurrentBullet += DifferBetweenLimitAndCurrent;
			TotalBullets -= DifferBetweenLimitAndCurrent;
		}
	}
}

void ASWeapon::PlayImpactEffect(EPhysicalSurface PhysicalSurfaceMat, FHitResult OutHitResult)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (PhysicalSurfaceMat)
	{
	case SURFACETYPE_FLESHDEFAULT:
	case SURFACETYPE_FLESHHEADSHOT:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, OutHitResult.ImpactPoint, OutHitResult.ImpactNormal.Rotation());
	}
}

void ASWeapon::PlayFireEffect(FVector TraceEndPoint)
{
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

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}
}

FVector ASWeapon::GetMeshSocketLocationByName(FName SocketName) const
{
	return WeaponMeshComp->GetSocketLocation(SocketName);
}

FRotator ASWeapon::GetMeshSocketRotationByName(FName SocketName) const
{
	return WeaponMeshComp->GetSocketRotation(SocketName);
}

bool ASWeapon::IsCanFire() const
{
	return bCanFire;
}

int32 ASWeapon::GetCurrentBulletNum() const
{
	return CurrentBullet;
}

int32 ASWeapon::GetTotalBulletNum() const
{
	return TotalBullets;
}

