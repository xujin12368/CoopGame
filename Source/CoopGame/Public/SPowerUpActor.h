// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerUpActor.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class URotatingMovementComponent;

UCLASS()
class COOPGAME_API ASPowerUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerUpActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URotatingMovementComponent* RotatingMovementComp;

	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	float PickInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	int32 TotalNumOfTick;

	int32 NumOfTick;

	FTimerHandle TimerHandle_Pickup;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTickPowerUp();

public:
	// 为了在第一次被拾取后开始计时
	void ActivatePowerUp();

	// 在PowerUp被激活的时候调用，开始能力
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickups")
	void OnActivated();

	// 在计时器过期的时候调用，结束能力
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickups")
	void OnExpired();

	// 在每次更新的时候调用，用于调节更新细节，比如不叠加能力
	// 处理每次更新时会发生地事件，比如持续地回血
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickups")
	void OnPowerUpTicked();

	float GetTotalNumOfTick() const;

	float GetPickInterval() const;
};
