// Fill out your copyright notice in the Description page of Project Settings.

#include "Scan/TA_HighlightPPActor.h"

#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ATA_HighlightPPActor::ATA_HighlightPPActor()
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

	// 比 Scan Actor 高，保证 Highlight PP 在后面处理
	PostProcessComponent->Priority = 1100.0f;

	// 全局生效，不依赖摄像机是否进入 Volume
	PostProcessComponent->bUnbound = true;

	// 默认完全生效
	PostProcessComponent->BlendWeight = 1.0f;

	// 找 MI_Highlight
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HighlightMaterialRef(
		TEXT("/Game/Materials/Scan/MI_Highlight.MI_Highlight")
	);

	if (HighlightMaterialRef.Succeeded())
	{
		PostProcessComponent->Settings.AddBlendable(
			HighlightMaterialRef.Object,
			1.0f
		);
	}
}