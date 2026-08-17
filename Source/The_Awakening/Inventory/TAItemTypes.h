#pragma once

#include "CoreMinimal.h"
#include "TAItemTypes.generated.h"

class UTAItemDefinition;

UENUM(BlueprintType)
enum class ETAItemType : uint8
{
	Treasure      UMETA(DisplayName = "宝物"),
	Consumable    UMETA(DisplayName = "消耗道具"),
	QuestItem     UMETA(DisplayName = "任务道具"),
	Weapon        UMETA(DisplayName = "武器"),
	SkillBook     UMETA(DisplayName = "技能书"),
};

USTRUCT(BlueprintType)
struct FTAInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTAItemDefinition> ItemDef = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;

	/** 武器耐久 非武器为 0 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Durability = 0.f;

	bool IsEmpty() const
	{
		return ItemDef == nullptr || Count <= 0;
	}
};