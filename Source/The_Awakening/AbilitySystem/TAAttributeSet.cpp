// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/TAAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UTAAttributeSet::UTAAttributeSet()
{
	// 初始默认值（后续也可以用 GameplayEffect 来初始化）
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitAttackMultiplier(1.0f);
	InitDamageReduction(0.0f);
}

void UTAAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UTAAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTAAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTAAttributeSet, AttackMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTAAttributeSet, DamageReduction, COND_None, REPNOTIFY_Always);
}

void UTAAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTAAttributeSet, Health, OldHealth);
}

void UTAAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTAAttributeSet, MaxHealth, OldMaxHealth);
}

void UTAAttributeSet::OnRep_AttackMultiplier(const FGameplayAttributeData& OldAttackMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTAAttributeSet, AttackMultiplier, OldAttackMultiplier);
}

void UTAAttributeSet::OnRep_DamageReduction(const FGameplayAttributeData& OldDamageReduction)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTAAttributeSet, DamageReduction, OldDamageReduction);
}

void UTAAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 防止 MaxHealth 小于 1
	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
}

void UTAAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 生命值变化后进行钳制
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
}