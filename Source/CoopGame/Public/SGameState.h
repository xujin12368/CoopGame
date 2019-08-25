// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
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
	// ÿ��ʹ����ReplicatedUsingһ��Ҫ�ǵ����GetLifetimeReplicatedProps
	UPROPERTY(ReplicatedUsing = OnRep_WaveState, BlueprintReadOnly, Category = "GameState")
	EWaveState WaveState;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

protected:
	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	// Ϊ�˿��ٵ����������ô���ͼʵ�ֺ�����ReplicatedUsing���ʹ��
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void OnWaveStateChanged(EWaveState NewState, EWaveState OldState);
};
