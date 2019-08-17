// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STracerBot.generated.h"

UCLASS()
class COOPGAME_API ASTracerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTracerBot();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "TracerBot")
	float RequiredDistanceToPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TracerBot")
	float ForceStrength;

	UPROPERTY(EditDefaultsOnly, Category = "TracerBot")
	bool bAccelChange;

	FVector NextPathPoint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetNextMovePathPoint();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
