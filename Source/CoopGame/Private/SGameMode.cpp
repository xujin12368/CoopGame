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

	// PrepareNextWave(); // 让Bot都死亡后再生成
}

void ASGameMode::PrepareNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWave, this, &ASGameMode::StartWave, TimeBetweenTwoWaves, false);
}

void ASGameMode::CheckBotAliveThenNextWave()
{
	// 因为Tick是每一秒检查，所以为了防止PrepareNextWave运行时执行此函数，
	// 在TimerHandle_NextWave激活时退出。
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
