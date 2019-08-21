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
	// Ϊ���ڵ�һ�α�ʰȡ��ʼ��ʱ
	void ActivatePowerUp();

	// ��PowerUp�������ʱ����ã���ʼ����
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickups")
	void OnActivated();

	// �ڼ�ʱ�����ڵ�ʱ����ã���������
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickups")
	void OnExpired();

	// ��ÿ�θ��µ�ʱ����ã����ڵ��ڸ���ϸ�ڣ����粻��������
	// ����ÿ�θ���ʱ�ᷢ�����¼�����������ػ�Ѫ
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickups")
	void OnPowerUpTicked();

	float GetTotalNumOfTick() const;

	float GetPickInterval() const;
};
