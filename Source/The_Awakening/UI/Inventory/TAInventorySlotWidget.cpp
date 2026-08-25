#include "UI/Inventory/TAInventorySlotWidget.h"
#include "Inventory/TAItemDefinition.h"
#include "Core/TALocalizeSubsystem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Engine/Texture2D.h"

void UTAInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshVisuals();
}

void UTAInventorySlotWidget::SetSlotData(const FTAInventorySlot& SlotData, int32 InFlatIndex)
{
	CachedSlot = SlotData;
	FlatIndex = InFlatIndex;
	RefreshVisuals();
}

void UTAInventorySlotWidget::SetEmpty()
{
	CachedSlot = FTAInventorySlot();
	FlatIndex = INDEX_NONE;
	RefreshVisuals();
}

bool UTAInventorySlotWidget::IsEmpty() const
{
	return CachedSlot.IsEmpty();
}

UTAItemDefinition* UTAInventorySlotWidget::GetItemDef() const
{
	return CachedSlot.ItemDef;
}

void UTAInventorySlotWidget::RefreshVisuals()
{
	const bool bHasItem = !CachedSlot.IsEmpty() && CachedSlot.ItemDef;

	if (Image_item)
	{
		if (bHasItem)
		{
			UTexture2D* Icon = CachedSlot.ItemDef->Icon.Get(); // 若是 Soft 则 LoadSynchronous
			if (Icon)
			{
				Image_item->SetBrushFromTexture(Icon);
				Image_item->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
			else
			{
				Image_item->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
		{
			Image_item->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (Text_Count)
	{
		if (bHasItem && CachedSlot.Count > 1)
		{
			Text_Count->SetText(FText::AsNumber(CachedSlot.Count));
			Text_Count->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			Text_Count->SetText(FText::GetEmpty());
			Text_Count->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (Text_Name)
	{
		if (bHasItem && CachedSlot.ItemDef)
		{
			const FString Id = CachedSlot.ItemDef->DisplayName.ToString();
			FText NameText = FText::FromString(Id);

			if (UWorld* World = GetWorld())
			{
				if (UGameInstance* GI = World->GetGameInstance())
				{
					if (const UTALocalizeSubsystem* Loc = GI->GetSubsystem<UTALocalizeSubsystem>())
					{
						NameText = Loc->GetText(Id);
					}
				}
			}

			Text_Name->SetText(NameText);
			Text_Name->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			Text_Name->SetText(FText::GetEmpty());
			Text_Name->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}