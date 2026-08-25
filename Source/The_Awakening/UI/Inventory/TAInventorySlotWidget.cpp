#include "UI/Inventory/TAInventorySlotWidget.h"
#include "Inventory/TAItemDefinition.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UTAInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureWidgets();
	RefreshVisuals();
}

void UTAInventorySlotWidget::EnsureWidgets()
{
	if (RootSizeBox)
	{
		return;
	}

	if (!WidgetTree)
	{
		return;
	}

	RootSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("RootSizeBox"));
	WidgetTree->RootWidget = RootSizeBox;
	RootSizeBox->SetWidthOverride(SlotSize);
	RootSizeBox->SetHeightOverride(SlotSize);

	BackgroundBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BackgroundBorder"));
	RootSizeBox->AddChild(BackgroundBorder);
	BackgroundBorder->SetBrushColor(FLinearColor(0.08f, 0.08f, 0.1f, 0.9f));
	BackgroundBorder->SetPadding(FMargin(4.f));

	ContentOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("ContentOverlay"));
	BackgroundBorder->SetContent(ContentOverlay);

	IconImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("IconImage"));
	if (UOverlaySlot* IconSlot = ContentOverlay->AddChildToOverlay(IconImage))
	{
		IconSlot->SetHorizontalAlignment(HAlign_Fill);
		IconSlot->SetVerticalAlignment(VAlign_Fill);
	}
	IconImage->SetVisibility(ESlateVisibility::Hidden);

	CountText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CountText"));
	if (UOverlaySlot* TextSlot = ContentOverlay->AddChildToOverlay(CountText))
	{
		TextSlot->SetHorizontalAlignment(HAlign_Right);
		TextSlot->SetVerticalAlignment(VAlign_Bottom);
		TextSlot->SetPadding(FMargin(0.f, 0.f, 2.f, 0.f));
	}
	CountText->SetVisibility(ESlateVisibility::Hidden);
}

void UTAInventorySlotWidget::SetSlotData(const FTAInventorySlot& SlotData, int32 InFlatIndex)
{
	EnsureWidgets();
	CachedSlot = SlotData;
	FlatIndex = InFlatIndex;
	RefreshVisuals();
}

void UTAInventorySlotWidget::SetEmpty()
{
	EnsureWidgets();
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
	EnsureWidgets();
	if (!IconImage || !CountText)
	{
		return;
	}

	if (CachedSlot.IsEmpty() || !CachedSlot.ItemDef)
	{
		IconImage->SetVisibility(ESlateVisibility::Hidden);
		CountText->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	UTexture2D* Icon = CachedSlot.ItemDef->Icon;

	if (Icon)
	{
		IconImage->SetBrushFromTexture(Icon);
		IconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		IconImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if (CachedSlot.Count > 1)
	{
		CountText->SetText(FText::AsNumber(CachedSlot.Count));
		CountText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		CountText->SetVisibility(ESlateVisibility::Hidden);
	}
}