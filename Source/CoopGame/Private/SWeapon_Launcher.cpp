// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon_Launcher.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

ASWeapon_Launcher::ASWeapon_Launcher()
{
	SocketName = "MuzzleSocket";
}

void ASWeapon_Launcher::StartFire()
{
	Super::StartFire();
}

void ASWeapon_Launcher::StopFire()
{
	Super::StopFire();
}

void ASWeapon_Launcher::SetLastFireTime()
{
	Super::SetLastFireTime();
}

void ASWeapon_Launcher::ConsumeBullet()
{
	Super::ConsumeBullet();
}

void ASWeapon_Launcher::Fire()
{
	ConsumeBullet();
	if (!IsCanFire())
	{
		return;
	}

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		if (Projectile)
		{
			AActor* ProjectileBP = GetWorld()->SpawnActor<AActor>(
				Projectile,
				GetMeshSocketLocationByName(SocketName),
				EyeRotation
				);
			ProjectileBP->SetOwner(this);
			ProjectileBP->InitialLifeSpan = 5.f;
		}
	}

	SetLastFireTime();

}
