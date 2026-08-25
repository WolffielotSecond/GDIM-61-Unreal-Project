#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/TAInventoryComponent.h"
#include "TAClothingPanelWidget.generated.h"

class UCanvasPanel;
class UImage;
class UCanvasPanelSlot;
class UTAInventorySlotWidget;

UCLASS()
class THE_AWAKENING_API UTAClothingPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 根据衣服实例生成图标、格子；FlatIndexStart 对应 Inventory 扁平下标起点 */
	UFUNCTION(BlueprintCallable, Category = "ClothingUI")
	void BuildFromClothing(const FTAClothingInstance& Instance, int32 FlatIndexStart = 0);

	UFUNCTION(BlueprintCallable, Category = "ClothingUI")
	void ClearPanel();

	/** 格子 Widget 类 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ClothingUI")
	TSubclassOf<UUserWidget> SlotWidgetClass;

protected:
	virtual void NativeConstruct() override;

	void EnsureCanvas();
	void SetClothingIcon(UTAClothingDefinition* Def);
	FVector2D GetClothingImageTopLeft() const;
	FVector2D PocketAnchorToCanvas(const FVector2D& AnchorUV) const;
	FVector2D ComputePocketSlotsOrigin(const FVector2D& AnchorCanvas, ETAPocketSlotSide Side, int32 SlotCount) const;
	void CreateSlotsForPocket(
		const FTAPocketRuntime& PocketRuntime,
		const FTAPocketDef& PocketDef,
		int32& InOutFlatIndex);
	void CreateLine(const FVector2D& From, const FVector2D& To);

protected:
	/** 命名为 CanvasRoot 的 Canvas Panel */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCanvasPanel> CanvasRoot;

	/** 命名为 Image_Clothing 的图片 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_Clothing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ClothingUI")
	float ClothingImageSize = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ClothingUI")
	float SlotSize = 64.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ClothingUI")
	float SlotPadding = 8.f;

	/** 衣服图边缘到格子的间距 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ClothingUI")
	float EdgeMargin = 24.f;

	/** 每个口袋格子每行数量 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ClothingUI", meta = (ClampMin = "1"))
	int32 SlotsPerRow = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ClothingUI")
	FVector2D CanvasSize = FVector2D(800.f, 700.f);

	/** 生成的格子，Clear 时移除 */
	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> GeneratedSlots;

	UPROPERTY()
	TArray<TObjectPtr<UWidget>> GeneratedLines;

	UPROPERTY()
	TObjectPtr<UTAClothingDefinition> CurrentDef;
};