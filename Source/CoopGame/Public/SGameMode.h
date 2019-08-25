// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

enum class EWaveState : uint8;

// ���¼�����������ӵ��HealthComp��Actor����ɱʱ����
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KiilerActor, AController*, InstigatedBy);

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category ="GameMode")
	FOnActorKilled OnActorKilled;

public:

	ASGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	int32 CoefficientPerWave;

	int32 NumOfBotPerWave;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float TimeBetweenTwoWaves;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float BotSpawnRate;

	FTimerHandle TimerHandle_SpawnBot;

	FTimerHandle TimerHandle_NextWave;


protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	// �����ͼ����SpawnNewBotʹ�ã�����ʣ��Bot����
	void SpawnNewBotElapsed();

	void StartWaveProgress();

	void EndWaveProgress();

	void PrepareNextWave();

	void CheckBotAliveThenNextWave();

	void CheckAnyPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewWaveState);
	
};
