// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

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

	void StartWave();

	void EndWave();

	void PrepareNextWave();

	void CheckBotAliveThenNextWave();

	void CheckAnyPlayerAlive();

	void GameOver();
	
};
