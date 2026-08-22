// Fill out your copyright notice in the Description page of Project Settings.


#include "Scan/TAScanningActor.h"

#include "NiagaraSystem.h"
#include "UObject/ConstructorHelpers.h"
// Sets default values
ATAScanningActor::ATAScanningActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root Component
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(
		TEXT("DefaultSceneRoot")
	);

	SetRootComponent(DefaultSceneRoot);

	// Post Process Component
	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(
		TEXT("PostProcessComponent")
	);

	PostProcessComponent->SetupAttachment(DefaultSceneRoot);
	PostProcessComponent->Priority = 1000.0f;
	PostProcessComponent->bUnbound = true;

	// Niagara Component
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(
		TEXT("NiagaraComponent")
	);

	NiagaraComponent->SetupAttachment(DefaultSceneRoot);


	// ур NS_Scan
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemRef(
		TEXT("/Game/FX/NS_Scan.NS_Scan")
	);

	if (NiagaraSystemRef.Succeeded())
	{
		NiagaraComponent->SetAsset(NiagaraSystemRef.Object);
	}
}


