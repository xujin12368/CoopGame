// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaveInProgress,

	// 不在生成Bot，等待玩家消灭所有Bot
	WaitingToComplete,

	WaveComplete,

	GameOver
};

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	// 每次使用了ReplicatedUsing一定要记得添加GetLifetimeReplicatedProps
	UPROPERTY(ReplicatedUsing = OnRep_WaveState, BlueprintReadOnly, Category = "GameState")
	EWaveState WaveState;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

protected:
	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	// 为了快速迭代，所以让此蓝图实现函数和ReplicatedUsing结合使用
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void OnWaveStateChanged(EWaveState NewState, EWaveState OldState);
};
