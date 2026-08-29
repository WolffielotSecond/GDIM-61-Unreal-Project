// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PostProcessComponent.h"
#include "TA_HighlightPPActor.generated.h"

UCLASS()
class THE_AWAKENING_API ATA_HighlightPPActor : public AActor
{
	GENERATED_BODY()

public:
	ATA_HighlightPPActor();

	UPostProcessComponent* GetPostProcessComponent() const
	{
		return PostProcessComponent;
	}

private:
	UPROPERTY(VisibleAnywhere, Category = "Highlight")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Highlight")
	TObjectPtr<UPostProcessComponent> PostProcessComponent;
};