#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/TAItemTypes.h"
#include "TAInventorySlotWidget.generated.h"

class UImage;
class UTextBlock;
class UBorder;
class USizeBox;
class UOverlay;
class UTAItemDefinition;

UCLASS()
class THE_AWAKENING_API UTAInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/** 格子数据 */
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
	void EnsureWidgets();
	void RefreshVisuals();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventorySlot")
	float SlotSize = 64.f;

	UPROPERTY()
	TObjectPtr<USizeBox> RootSizeBox;

	UPROPERTY()
	TObjectPtr<UBorder> BackgroundBorder;

	UPROPERTY()
	TObjectPtr<UOverlay> ContentOverlay;

	UPROPERTY()
	TObjectPtr<UImage> IconImage;

	UPROPERTY()
	TObjectPtr<UTextBlock> CountText;

	UPROPERTY()
	FTAInventorySlot CachedSlot;

	UPROPERTY()
	int32 FlatIndex = INDEX_NONE;
};