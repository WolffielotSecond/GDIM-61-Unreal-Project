#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TAItemTypes.h"
#include "TAInventoryComponent.generated.h"

class UTAItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THE_AWAKENING_API UTAInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTAInventoryComponent();

	virtual void BeginPlay() override;

	/** 最大格数（可配置，默认 9） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = "1"))
	int32 MaxSlots = 9;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FTAInventorySlot>& GetSlots() const { return Slots; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetMaxSlots() const { return MaxSlots; }

	/** 尝试加入物品，返回实际加入数量 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 TryAddItem(UTAItemDefinition* ItemDef, int32 Count = 1);

	/** 从指定格子移除，返回实际移除数量 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveFromSlot(int32 SlotIndex, int32 Count = 1);

	/** 查询某物品总数量 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemCount(UTAItemDefinition* ItemDef) const;

	/** 格子是否为空 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool IsSlotEmpty(int32 SlotIndex) const;

	/** 是否还有空位（不考虑堆叠） */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasEmptySlot() const;

	/** 丢弃：从格子扣数量，由外部生成世界物品 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool DropFromSlot(int32 SlotIndex, int32 Count, FTAInventorySlot& OutDropped);

protected:
	void InitSlots();
	void NotifyUpdated();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FTAInventorySlot> Slots;
};