// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SWeapon_Launcher.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASWeapon_Launcher : public ASWeapon
{
	GENERATED_BODY()

public:
	ASWeapon_Launcher();

	virtual void StartFire() override;

	virtual void StopFire() override;

	virtual void SetLastFireTime() override;

	virtual void ConsumeBullet() override;
	
protected:

	virtual void Fire() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> Projectile;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FName SocketName;
};
