// Fill out your copyright notice in the Description page of Project Settings.

#include "TAPlayerState.h"
#include "AbilitySystem/TAAttributeSet.h"

ATAPlayerState::ATAPlayerState()
{
	// 创建 AbilitySystemComponent
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// 推荐使用 Mixed 复制模式
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// 创建 AttributeSet
	AttributeSet = CreateDefaultSubobject<UTAAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ATAPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}