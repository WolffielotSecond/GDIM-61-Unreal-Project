#pragma once

#include "CoreMinimal.h"
#include "TAClothingTypes.generated.h"

UENUM(BlueprintType)
enum class ETAClothingLayer : uint8
{
	Inner UMETA(DisplayName = "内衬"),
	Outer UMETA(DisplayName = "外套")
};

/** 口袋格子相对 500x500 衣服图的摆放方向（避免挡住衣服） */
UENUM(BlueprintType)
enum class ETAPocketSlotSide : uint8
{
	Left   UMETA(DisplayName = "左侧"),
	Right  UMETA(DisplayName = "右侧"),
	Top    UMETA(DisplayName = "上方"),
	Bottom UMETA(DisplayName = "下方")
};

/**
 * 口袋定义（设计数据）
 */
USTRUCT(BlueprintType)
struct THE_AWAKENING_API FTAPocketDef
{
	GENERATED_BODY()

	/** 口袋标识，同一件衣服内唯一 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pocket")
	FName PocketId = TEXT("Main");

	/** 该口袋格子数量 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pocket", meta = (ClampMin = "1"))
	int32 SlotCount = 1;

	/**
	 * (0,0)=左上，(1,1)=右下，口袋位置
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pocket|UI",
		meta = (ClampMin = "0.0", ClampMax = "1.0"))
	FVector2D AnchorUV = FVector2D(0.5f, 0.5f);

	/** 格子生成在衣服图的哪一侧 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pocket|UI")
	ETAPocketSlotSide SlotSide = ETAPocketSlotSide::Right;
};