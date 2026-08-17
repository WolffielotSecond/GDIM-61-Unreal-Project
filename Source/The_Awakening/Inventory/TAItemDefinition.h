#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TAItemTypes.h"
#include "TAItemDefinition.generated.h"

UCLASS(BlueprintType)
class THE_AWAKENING_API UTAItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	ETAItemType ItemType = ETAItemType::Treasure;

	/** 是否可堆叠 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	bool bCanStack = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (EditCondition = "bCanStack", ClampMin = "1"))
	int32 MaxStack = 1;

	/** 卖出/商店参考价 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 Value = 0;

	/** 消耗品：使用时数量-1；可在此挂恢复量等（效果后做） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable")
	float ConsumableMagnitude = 0.f;

	/** 武器 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float WeaponDamage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float MaxDurability = 100.f;

	/** 任务/门卡：标识用，开门逻辑后接 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName QuestItemId;

	/** 技能书：解锁哪棵树（原型只用 Id） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillBook")
	FName SkillTreeId;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("TAItem"), GetFName());
	}
};