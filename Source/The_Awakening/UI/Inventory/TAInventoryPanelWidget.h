#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TAInventoryPanelWidget.generated.h"

class UTAInventoryComponent;
class UTAInventorySlotWidget;
class UTAClothingPanelWidget;
class UVerticalBox;
class UHorizontalBox;
class UBorder;
class UTextBlock;
class UCanvasPanel;
class UOverlay;
class USizeBox;

UCLASS()
class THE_AWAKENING_API UTAInventoryPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "InventoryUI")
	void Init(UTAInventoryComponent* InInventory);

	UFUNCTION(BlueprintCallable, Category = "InventoryUI")
	void RefreshAll();

protected:
	void EnsureLayout();
	void BuildLayout();

	UFUNCTION()
	void HandleInventoryUpdated();

protected:
	UPROPERTY()
	TObjectPtr<UTAInventoryComponent> Inventory;

	// 根
	UPROPERTY()
	TObjectPtr<UOverlay> RootOverlay;

	UPROPERTY()
	TObjectPtr<UBorder> DimBorder;

	UPROPERTY()
	TObjectPtr<UBorder> PanelBorder;

	UPROPERTY()
	TObjectPtr<UTextBlock> MoneyText;

	UPROPERTY()
	TObjectPtr<UVerticalBox> LeftColumn;

	UPROPERTY()
	TObjectPtr<UTAInventorySlotWidget> OuterEquipSlot;

	UPROPERTY()
	TArray<TObjectPtr<UTAInventorySlotWidget>> StorySlots;

	UPROPERTY()
	TObjectPtr<UTAClothingPanelWidget> OuterClothingPanel;

	UPROPERTY()
	TObjectPtr<UTAClothingPanelWidget> InnerClothingPanel;

	UPROPERTY(EditAnywhere, Category = "InventoryUI")
	float PanelWidth = 1200.f;

	UPROPERTY(EditAnywhere, Category = "InventoryUI")
	float PanelHeight = 750.f;

	UPROPERTY(EditAnywhere, Category = "InventoryUI")
	float LeftColumnWidth = 100.f;

	UPROPERTY(EditAnywhere, Category = "InventoryUI")
	float SlotSize = 72.f;
};