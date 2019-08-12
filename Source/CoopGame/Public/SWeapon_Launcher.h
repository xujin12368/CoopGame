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
	
protected:

	virtual void Fire() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> Projectile;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FName SocketName;
};
