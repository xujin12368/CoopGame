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
#include "Sound/SoundCue.h"

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

	bExplosionSelf = false;

	MyMaxPowerLevel = 10;
	MyPowerLevel = 0;

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

	if (Role == ROLE_Authority)
	{
		NextPathPoint = GetNextMovePathPoint();
	}

	GetWorldTimerManager().SetTimer(TimerHandle_ScanPartner, this, &ASTracerBot::ScanOtherPartners, 1.f, true, 0.f);
}

FVector ASTracerBot::GetNextMovePathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	// ����Tick���棬ÿ�ζ����µ�ActorLocation������ÿ��Tick��������ǰ����PathPoint�н�
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath && NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
}

void ASTracerBot::ScanOtherPartners()
{
	// Scan partner around and save PowerLevel.
	if (PartnerClassFilter)
	{
		TArray<AActor*> OverlappingActors;
		SphereComp->GetOverlappingActors(OverlappingActors, PartnerClassFilter);
		int32 NumOfPartners = OverlappingActors.Num();
		if (NumOfPartners > 0)
		{
			for (auto OverlappingActor : OverlappingActors)
			{
				ASTracerBot* MyPartner = Cast<ASTracerBot>(OverlappingActor);
				if (MyPartner && MyPartner!=this && MyPowerLevel < NumOfPartners)
				{
					MyPowerLevel++;
				}
				else if (MyPartner && MyPartner != this && MyPowerLevel > NumOfPartners)
				{
					MyPowerLevel -= (MyPowerLevel - NumOfPartners);
				}
			}
		}
		else
		{
			MyPowerLevel = 0;
		}

		// Set Parameter of material to bling.
		float MatAlpha = (float)MyPowerLevel / (float)MyMaxPowerLevel;

		if (MatPartnerInstance == nullptr)
		{
			MatPartnerInstance = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
		}
		else
		{
			MatPartnerInstance->SetScalarParameterValue("PowerLevelAlpha", MatAlpha);

			UE_LOG(LogTemp, Warning, TEXT("%s Alpha: %s"), *GetName(), *FString::SanitizeFloat(MatAlpha));
		}
	}
}

void ASTracerBot::SelfDestruct()
{
	if (bExplosionSelf)
	{
		return;
	}
	// �ͻ����Ͽ�������Ч
	// ԭ�򣺷����������ݻ���TracerBot������δͬ�����ͻ���
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	UGameplayStatics::SpawnSoundAtLocation(this, ExplosionSound, GetActorLocation());

	// Ϊ��ͬ��2.f������ʱ�䣬����BotΪ���ɼ��Լ�ȡ����ײ
	MeshComp->SetVisibility(false, true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bExplosionSelf = true;

	// ApplyDamage����Server������
	if (Role == ROLE_Authority)
	{
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

		// Destroy(); // ��ʹ�������ݻ٣�����ʹ��SetLifeSpan()
		// �ӳٴݻ��ÿͻ����ܹ�������Ч��

		SetLifeSpan(2.f);

		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.f, 0, 1.f);
	}
}

void ASTracerBot::DamageSelf()
{
	if (!bExplosionSelf)
	{
		UGameplayStatics::ApplyDamage(this, 20.f, GetInstigatorController(), this, nullptr);
	}
}

void ASTracerBot::HandleSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ACharacter* PlayerPawn = Cast<ACharacter>(OtherActor);
	if (!bExplosionSelf && PlayerPawn)
	{
		UGameplayStatics::SpawnSoundAttached(ReadyToExplovieSound, RootComponent);

		// ��ʱ������Server������
		if (Role == ROLE_Authority)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_DamageSelf, this, &ASTracerBot::DamageSelf, DamageSelfRate, true, 0.f);
		}
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

	// Ϊ��ͬ���ͻ��˵�2.f������Ϊ���Ա�����ִ�У�����Ѱ·
	if (Role == ROLE_Authority && !bExplosionSelf)
	{
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


}

