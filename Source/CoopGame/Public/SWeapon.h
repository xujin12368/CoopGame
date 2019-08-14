// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UParticleSystem;
class UDamageType;
class UCameraShake;

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float BaseDamage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerParameter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCamShake;

	FTimerHandle TimerHandle_BetweedTowShots;

	// RPM - bullet per min.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float FireRate;

	float FireRateTime;

	float LastFireTime;

	bool bCanFire;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 BulletUpperLimit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "Weapon")
	int32 CurrentBullet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "Weapon")
	int32 TotalBullets;

protected:
	virtual void BeginPlay() override;

	virtual void Fire();

public:	

	virtual void StartFire();

	virtual void StopFire();

	virtual void SetLastFireTime();

	virtual void ConsumeBullet();

	void ProcessWeaponBullet();

	void PlayImpactEffect(EPhysicalSurface PhysicalSurfaceMat, FHitResult OutHitResult);

	void PlayFireEffect(FVector TraceEndPoint);

	FVector GetMeshSocketLocationByName(FName SocketName) const;

	FRotator GetMeshSocketRotationByName(FName SocketName) const;

	bool IsCanFire() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentBulletNum() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetTotalBulletNum() const;

};
