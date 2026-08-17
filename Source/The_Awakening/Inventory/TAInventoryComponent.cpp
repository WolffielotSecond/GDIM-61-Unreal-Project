#include "Inventory/TAInventoryComponent.h"
#include "Inventory/TAItemDefinition.h"

UTAInventoryComponent::UTAInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTAInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	InitSlots();
}

void UTAInventoryComponent::InitSlots()
{
	Slots.SetNum(MaxSlots);
}

void UTAInventoryComponent::NotifyUpdated()
{
	OnInventoryUpdated.Broadcast();
}

int32 UTAInventoryComponent::TryAddItem(UTAItemDefinition* ItemDef, int32 Count)
{
	if (!ItemDef || Count <= 0)
	{
		return 0;
	}

	int32 Remaining = Count;

	// 1) 可堆叠：先填已有堆
	if (ItemDef->bCanStack)
	{
		for (FTAInventorySlot& Slot : Slots)
		{
			if (Slot.ItemDef == ItemDef && Slot.Count < ItemDef->MaxStack)
			{
				const int32 CanAdd = FMath::Min(Remaining, ItemDef->MaxStack - Slot.Count);
				Slot.Count += CanAdd;
				Remaining -= CanAdd;
				if (Remaining <= 0)
				{
					NotifyUpdated();
					return Count;
				}
			}
		}
	}

	// 2) 空格子
	while (Remaining > 0)
	{
		int32 EmptyIndex = INDEX_NONE;
		for (int32 i = 0; i < Slots.Num(); ++i)
		{
			if (Slots[i].IsEmpty())
			{
				EmptyIndex = i;
				break;
			}
		}

		if (EmptyIndex == INDEX_NONE)
		{
			break; // 满了
		}

		FTAInventorySlot& Slot = Slots[EmptyIndex];
		Slot.ItemDef = ItemDef;

		if (ItemDef->bCanStack)
		{
			const int32 CanAdd = FMath::Min(Remaining, ItemDef->MaxStack);
			Slot.Count = CanAdd;
			Slot.Durability = 0.f;
			Remaining -= CanAdd;
		}
		else
		{
			Slot.Count = 1;
			Slot.Durability = (ItemDef->ItemType == ETAItemType::Weapon) ? ItemDef->MaxDurability : 0.f;
			Remaining -= 1;
		}
	}

	const int32 Added = Count - Remaining;
	if (Added > 0)
	{
		NotifyUpdated();
	}
	return Added;
}

int32 UTAInventoryComponent::RemoveFromSlot(int32 SlotIndex, int32 Count)
{
	if (!Slots.IsValidIndex(SlotIndex) || Count <= 0 || Slots[SlotIndex].IsEmpty())
	{
		return 0;
	}

	FTAInventorySlot& Slot = Slots[SlotIndex];
	const int32 Removed = FMath::Min(Count, Slot.Count);
	Slot.Count -= Removed;

	if (Slot.Count <= 0)
	{
		Slot.ItemDef = nullptr;
		Slot.Count = 0;
		Slot.Durability = 0.f;
	}

	NotifyUpdated();
	return Removed;
}

int32 UTAInventoryComponent::GetItemCount(UTAItemDefinition* ItemDef) const
{
	if (!ItemDef)
	{
		return 0;
	}

	int32 Total = 0;
	for (const FTAInventorySlot& Slot : Slots)
	{
		if (Slot.ItemDef == ItemDef)
		{
			Total += Slot.Count;
		}
	}
	return Total;
}

bool UTAInventoryComponent::IsSlotEmpty(int32 SlotIndex) const
{
	return !Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty();
}

bool UTAInventoryComponent::HasEmptySlot() const
{
	for (const FTAInventorySlot& Slot : Slots)
	{
		if (Slot.IsEmpty())
		{
			return true;
		}
	}
	return false;
}

bool UTAInventoryComponent::DropFromSlot(int32 SlotIndex, int32 Count, FTAInventorySlot& OutDropped)
{
	OutDropped = FTAInventorySlot();

	if (!Slots.IsValidIndex(SlotIndex) || Count <= 0 || Slots[SlotIndex].IsEmpty())
	{
		return false;
	}

	FTAInventorySlot& Slot = Slots[SlotIndex];
	const int32 DropCount = FMath::Min(Count, Slot.Count);

	OutDropped.ItemDef = Slot.ItemDef;
	OutDropped.Count = DropCount;
	OutDropped.Durability = Slot.Durability;

	Slot.Count -= DropCount;
	if (Slot.Count <= 0)
	{
		Slot.ItemDef = nullptr;
		Slot.Count = 0;
		Slot.Durability = 0.f;
	}

	NotifyUpdated();
	return true;
}