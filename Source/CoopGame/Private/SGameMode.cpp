// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "TimerManager.h"
#include "Components/SHealthComponent.h"

ASGameMode::ASGameMode()
{
	CoefficientPerWave = 2;

	TimeBetweenTwoWaves = 2.f;

	BotSpawnRate = 3.f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareNextWave();
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckBotAliveThenNextWave();

	CheckAnyPlayerAlive();
}

void ASGameMode::SpawnNewBotElapsed()
{
	NumOfBotPerWave--;

	SpawnNewBot();

	if (NumOfBotPerWave <= 0)
	{
		EndWave();
	}
}

void ASGameMode::StartWave()
{
	WaveCount++;

	NumOfBotPerWave = CoefficientPerWave * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBot, this, &ASGameMode::SpawnNewBotElapsed, BotSpawnRate, true, 0.f);
}

void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_SpawnBot);

	// PrepareNextWave(); // ��Bot��������������
}

void ASGameMode::PrepareNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWave, this, &ASGameMode::StartWave, TimeBetweenTwoWaves, false);
}

void ASGameMode::CheckBotAliveThenNextWave()
{
	// ��ΪTick��ÿһ���飬����Ϊ�˷�ֹPrepareNextWave����ʱִ�д˺�����
	// ��TimerHandle_NextWave����ʱ�˳���
	bool bNextWaveBeingPreparing = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWave);
	if (NumOfBotPerWave > 0 || bNextWaveBeingPreparing)
	{
		return;
	}

	bool bAnyBotAlive = false;
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp->GetHealth() >= 0)
		{
			bAnyBotAlive = true;
			break;
		}
	}

	if (!bAnyBotAlive)
	{
		PrepareNextWave();
	}

}

void ASGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn* PlayerPawn = PC->GetPawn();
			USHealthComponent* HealthComp = Cast<USHealthComponent>(PlayerPawn->GetComponentByClass(USHealthComponent::StaticClass()));
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0)
			{
				return;
			}
		}
	}

	// If Any Player Alive
	GameOver();
}

void ASGameMode::GameOver()
{
	EndWave();

	// @TODO: Handle Game Over
	UE_LOG(LogTemp, Warning, TEXT("Player Died , Game Over."));
}
