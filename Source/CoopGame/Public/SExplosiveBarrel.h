// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USHealthComponent;
class UParticleSystem;

UCLASS()
class COOPGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* MyHealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float ImpuseStrength;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float RadialStrength;

	UPROPERTY(EditDefaultsOnly, Category ="Setup")
	UParticleSystem* ExplosiveEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UMaterialInterface* ChangeMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<AActor> MyClassFilter;

	UPROPERTY(ReplicatedUsing = OnRep_Explosive)
	bool bDestroyed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ExplosiveToOtherActors();

	UFUNCTION()
	void OnRep_Explosive();

	UFUNCTION()
	void HandleChangedHealth(USHealthComponent* HealthComp, int32 Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
