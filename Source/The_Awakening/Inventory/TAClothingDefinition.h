#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Inventory/TAClothingTypes.h"
#include "TAClothingDefinition.generated.h"

class UTexture2D;

/** 衣服定义 */
UCLASS(BlueprintType)
class THE_AWAKENING_API UTAClothingDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** 内衬或外套 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clothing")
	ETAClothingLayer Layer = ETAClothingLayer::Inner;

	/** 本地化 ID */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clothing")
	FString DisplayNameId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clothing")
	FString DescriptionId;

	/** UI 上的衣服图 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clothing")
	TSoftObjectPtr<UTexture2D> Icon;

	/** 口袋列表；格子总数 = 各口袋 SlotCount 之和 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clothing")
	TArray<FTAPocketDef> Pockets;

	/** 总格子数 */
	UFUNCTION(BlueprintCallable, Category = "Clothing")
	int32 GetTotalSlotCount() const
	{
		int32 Total = 0;
		for (const FTAPocketDef& Pocket : Pockets)
		{
			Total += FMath::Max(0, Pocket.SlotCount);
		}
		return Total;
	}
};