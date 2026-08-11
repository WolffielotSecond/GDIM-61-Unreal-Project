// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TAInteractable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UTAInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 所有可交互物体的统一接口
 */
class THE_AWAKENING_API ITAInteractable
{
	GENERATED_BODY()

public:
	/** 执行交互 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* Interactor);

	/** 是否可以交互 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(AActor* Interactor) const;

	/** 获取交互提示文本（可选） */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractText() const;
};