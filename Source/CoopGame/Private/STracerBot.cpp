// Fill out your copyright notice in the Description page of Project Settings.


#include "STracerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "Components/SHealthComponent.h"
#include "Components/SphereComponent.h"
#include "CoopGame.h"
#include "TimerManager.h"

// Sets default values
ASTracerBot::ASTracerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RequiredDistanceToPathPoint = 100.f;
	ForceStrength = 500.f;

	bAccelChange = false;

	ExplosionDamage = 50.f;
	ExplosionRadius = 100.f;
	DamageSelfRate = 1.f;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(RootComponent);
	SphereComp->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	SphereComp->SetSphereRadius(ExplosionRadius);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASTracerBot::HandleSphereOverlap);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTracerBot::HandleTakeDamage);
}

// Called when the game starts or when spawned
void ASTracerBot::BeginPlay()
{
	Super::BeginPlay();
	
	NextPathPoint = GetNextMovePathPoint();
}

FVector ASTracerBot::GetNextMovePathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	
	// 放在Tick里面，每次都是新的ActorLocation，所以每次Tick都会向着前方的PathPoint行进
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath && NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
}

void ASTracerBot::SelfDestruct()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(
		this, 
		ExplosionDamage, 
		GetActorLocation(), 
		ExplosionRadius, 
		nullptr, 
		IgnoreActors, 
		this, 
		GetInstigatorController(), 
		true, 
		ECC_Visibility
	);

	Destroy();

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.f, 0, 1.f);
}

void ASTracerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20.f, GetInstigatorController(), this, nullptr);
}

void ASTracerBot::HandleSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ACharacter* PlayerPawn = Cast<ACharacter>(OtherActor);
	if (PlayerPawn)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_DamageSelf, this, &ASTracerBot::DamageSelf, DamageSelfRate, true, 0.f);
	}
}

void ASTracerBot::HandleTakeDamage(USHealthComponent* OwningHealthComp, int32 Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (MatInstance == nullptr)
	{
		MatInstance = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	else
	{
		MatInstance->SetScalarParameterValue("LastTimeShot", GetWorld()->TimeSeconds);
	}

	UE_LOG(LogTemp, Warning, TEXT("%s's Health is : %s"), *GetName(), *FString::FromInt(Health));

	// Explosive when died.
	if (Health <= 0)
	{
		SelfDestruct();
	}
}

// Called every frame
void ASTracerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToPathPoint = (NextPathPoint - GetActorLocation()).Size();
	if (DistanceToPathPoint <= RequiredDistanceToPathPoint)
	{
		NextPathPoint = GetNextMovePathPoint();
	}
	else
	{
		FVector MoveDirection = NextPathPoint - GetActorLocation();
		MoveDirection.Normalize();

		MeshComp->AddForce(MoveDirection * ForceStrength, NAME_None, bAccelChange);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), NextPathPoint, 10.f, FColor::Yellow, false, 0.f, 0, 1.f);
	}
}

