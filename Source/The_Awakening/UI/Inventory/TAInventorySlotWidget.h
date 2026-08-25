#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/TAItemTypes.h"
#include "TAInventorySlotWidget.generated.h"

class UImage;
class UTextBlock;
class UBorder;
class USizeBox;
class UTAItemDefinition;

UCLASS()
class THE_AWAKENING_API UTAInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "InventorySlot")
	void SetSlotData(const FTAInventorySlot& SlotData, int32 FlatIndex);

	UFUNCTION(BlueprintCallable, Category = "InventorySlot")
	void SetEmpty();

	UFUNCTION(BlueprintCallable, Category = "InventorySlot")
	int32 GetFlatIndex() const { return FlatIndex; }

	UFUNCTION(BlueprintCallable, Category = "InventorySlot")
	bool IsEmpty() const;

	UFUNCTION(BlueprintCallable, Category = "InventorySlot")
	UTAItemDefinition* GetItemDef() const;

protected:
	void RefreshVisuals();

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<USizeBox> SizeBox_Root; // 若根 Size Box 没改名，可删掉此绑定

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> Border;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_item;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Count;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Name;

	UPROPERTY()
	FTAInventorySlot CachedSlot;

	UPROPERTY()
	int32 FlatIndex = INDEX_NONE;
};