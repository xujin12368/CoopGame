// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STracerBot.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class USHealthComponent;
class USoundCue;

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
	USphereComponent* SphereComp;

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

	UMaterialInstanceDynamic* MatPartnerInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float DamageSelfRate;

	FTimerHandle TimerHandle_DamageSelf;

	FTimerHandle TimerHandle_ScanPartner;

	FTimerHandle TimerHandle_Repath;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	USoundCue* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	USoundCue* ReadyToExplovieSound;

	bool bExplosionSelf;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<AActor> PartnerClassFilter;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	int32 MyMaxPowerLevel;

	int32 MyPowerLevel;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float RateOfRepath;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetNextMovePathPoint();

	void ScanOtherPartners();

	void SelfDestruct();

	void DamageSelf();

	void Repath();

	UFUNCTION()
	void HandleSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
