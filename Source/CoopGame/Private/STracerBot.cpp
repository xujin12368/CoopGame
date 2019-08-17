// Fill out your copyright notice in the Description page of Project Settings.


#include "STracerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASTracerBot::ASTracerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	RequiredDistanceToPathPoint = 100.f;
	ForceStrength = 500.f;

	bAccelChange = false;

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
	
	// ����Tick���棬ÿ�ζ����µ�ActorLocation������ÿ��Tick��������ǰ����PathPoint�н�
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath && NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
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

