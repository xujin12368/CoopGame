// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

enum class EWaveState : uint8;

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

	// 配合蓝图函数SpawnNewBot使用，计算剩余Bot数量
	void SpawnNewBotElapsed();

	void StartWaveProgress();

	void EndWaveProgress();

	void PrepareNextWave();

	void CheckBotAliveThenNextWave();

	void CheckAnyPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewWaveState);
	
};
