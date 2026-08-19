// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "TAPlayerState.generated.h"

class UTAAttributeSet;

/**
 *
 */
UCLASS()
class THE_AWAKENING_API ATAPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ATAPlayerState();

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UTAAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UFUNCTION(BlueprintCallable, Category = "Money")
	int32 GetMoney() const { return Money; }

	UFUNCTION(BlueprintCallable, Category = "Money")
	void AddMoney(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Money")
	bool SpendMoney(int32 Amount);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UTAAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Money")
	int32 Money = 0;
};