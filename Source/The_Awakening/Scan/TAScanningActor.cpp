// Fill out your copyright notice in the Description page of Project Settings.


#include "Scan/TAScanningActor.h"

// Sets default values
ATAScanningActor::ATAScanningActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(
		TEXT("PostProcessComponent")
	);

	SetRootComponent(PostProcessComponent);

	PostProcessComponent->Priority = 1000.0f;
	PostProcessComponent->bUnbound = true;
}


