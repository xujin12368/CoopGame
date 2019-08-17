// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STracerBot.generated.h"

class USHealthComponent;

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

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "TracerBot")
	float RequiredDistanceToPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TracerBot")
	float ForceStrength;

	UPROPERTY(EditDefaultsOnly, Category = "TracerBot")
	bool bAccelChange;

	FVector NextPathPoint;

	UMaterialInstanceDynamic* MatInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetNextMovePathPoint();

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* OwningHealthComp, int32 Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
