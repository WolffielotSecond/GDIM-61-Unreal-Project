// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PostProcessComponent.h"
#include "TAScanningActor.generated.h"

UCLASS()
class THE_AWAKENING_API ATAScanningActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATAScanningActor();

	UPostProcessComponent* GetPostProcessComponent() const
	{
		return PostProcessComponent;
	}

private:
	UPROPERTY(VisibleAnywhere, Category = "Scan")
	TObjectPtr<UPostProcessComponent> PostProcessComponent;
};