// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,

	WaveInProgress,

	// ��������Bot���ȴ������������Bot
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
	// �˺���ֻ��Server����
	void SetWaveState(EWaveState NewState);

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

protected:
	// ÿ��ʹ����ReplicatedUsingһ��Ҫ�ǵ����GetLifetimeReplicatedProps
	UPROPERTY(ReplicatedUsing = OnRep_WaveState, BlueprintReadOnly, Category = "GameState")
	EWaveState WaveState;

protected:
	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	// Ϊ�˿��ٵ����������ô���ͼʵ�ֺ�����ReplicatedUsing���ʹ��
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void OnWaveStateChanged(EWaveState NewState, EWaveState OldState);
};
