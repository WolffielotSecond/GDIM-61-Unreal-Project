#include "UI/Inventory/TAClothingPanelWidget.h"
#include "Inventory/TAClothingDefinition.h"
#include "Inventory/TAItemDefinition.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Texture2D.h"
#include "UI/Inventory/TAInventorySlotWidget.h"

void UTAClothingPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureCanvas();
}

void UTAClothingPanelWidget::EnsureCanvas()
{
	if (CanvasRoot)
	{
		return;
	}

	if (!WidgetTree)
	{
		return;
	}

	// 若蓝图有绑定则用绑定的
	CanvasRoot = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("CanvasRoot")));
	Image_Clothing = Cast<UImage>(GetWidgetFromName(TEXT("Image_Clothing")));

	if (!CanvasRoot)
	{
		CanvasRoot = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CanvasRoot"));
		WidgetTree->RootWidget = CanvasRoot;
	}

	if (!Image_Clothing)
	{
		Image_Clothing = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Image_Clothing"));
		CanvasRoot->AddChild(Image_Clothing);

		if (UCanvasPanelSlot* ImageSlot = Cast<UCanvasPanelSlot>(Image_Clothing->Slot))
		{
			ImageSlot->SetAnchors(FAnchors(0.5f, 0.5f));
			ImageSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			ImageSlot->SetPosition(FVector2D::ZeroVector);
			ImageSlot->SetSize(FVector2D(ClothingImageSize, ClothingImageSize));
			ImageSlot->SetZOrder(1);
		}
	}

	// 给 Canvas 一个期望尺寸，避免为 0
	if (UCanvasPanelSlot* RootAsChild = Cast<UCanvasPanelSlot>(CanvasRoot->Slot))
	{
		// 作为子控件被 Panel 加入时可能有 Slot
	}
}

void UTAClothingPanelWidget::ClearPanel()
{
	EnsureCanvas();

	for (UUserWidget* SlotWidget : GeneratedSlots)
	{
		if (SlotWidget)
		{
			SlotWidget->RemoveFromParent();
		}
	}
	GeneratedSlots.Reset();

	for (UWidget* Line : GeneratedLines)
	{
		if (Line)
		{
			Line->RemoveFromParent();
		}
	}
	GeneratedLines.Reset();

	CurrentDef = nullptr;
}

void UTAClothingPanelWidget::SetClothingIcon(UTAClothingDefinition* Def)
{
	EnsureCanvas();
	if (!Image_Clothing)
	{
		return;
	}

	UTexture2D* Icon = nullptr;
	if (Def && !Def->Icon.IsNull())
	{
		Icon = Def->Icon.LoadSynchronous();
	}

	if (Icon)
	{
		Image_Clothing->SetBrushFromTexture(Icon);
		Image_Clothing->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Image_Clothing->SetVisibility(ESlateVisibility::Hidden);
	}

	if (UCanvasPanelSlot* ImageSlot = Cast<UCanvasPanelSlot>(Image_Clothing->Slot))
	{
		ImageSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		ImageSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		ImageSlot->SetPosition(FVector2D::ZeroVector);
		ImageSlot->SetSize(FVector2D(ClothingImageSize, ClothingImageSize));
	}
}

FVector2D UTAClothingPanelWidget::GetClothingImageTopLeft() const
{
	// 图居中于 Canvas：左上角 = Canvas中心 - 半边长
	const FVector2D Center = CanvasSize * 0.5f;
	return Center - FVector2D(ClothingImageSize, ClothingImageSize) * 0.5f;
}

FVector2D UTAClothingPanelWidget::PocketAnchorToCanvas(const FVector2D& AnchorUV) const
{
	const FVector2D TopLeft = GetClothingImageTopLeft();
	return TopLeft + FVector2D(
		FMath::Clamp(AnchorUV.X, 0.f, 1.f) * ClothingImageSize,
		FMath::Clamp(AnchorUV.Y, 0.f, 1.f) * ClothingImageSize);
}

FVector2D UTAClothingPanelWidget::ComputePocketSlotsOrigin(
	const FVector2D& AnchorCanvas,
	ETAPocketSlotSide Side,
	int32 SlotCount) const
{
	const FVector2D TopLeft = GetClothingImageTopLeft();
	const FVector2D BottomRight = TopLeft + FVector2D(ClothingImageSize, ClothingImageSize);

	const int32 Rows = FMath::Max(1, FMath::CeilToInt(static_cast<float>(SlotCount) / static_cast<float>(SlotsPerRow)));
	const int32 Cols = FMath::Min(SlotCount, SlotsPerRow);
	const float BlockW = Cols * SlotSize + FMath::Max(0, Cols - 1) * SlotPadding;
	const float BlockH = Rows * SlotSize + FMath::Max(0, Rows - 1) * SlotPadding;

	FVector2D Origin = FVector2D::ZeroVector;

	switch (Side)
	{
	case ETAPocketSlotSide::Right:
		Origin.X = BottomRight.X + EdgeMargin;
		Origin.Y = AnchorCanvas.Y - BlockH * 0.5f;
		break;
	case ETAPocketSlotSide::Left:
		Origin.X = TopLeft.X - EdgeMargin - BlockW;
		Origin.Y = AnchorCanvas.Y - BlockH * 0.5f;
		break;
	case ETAPocketSlotSide::Top:
		Origin.X = AnchorCanvas.X - BlockW * 0.5f;
		Origin.Y = TopLeft.Y - EdgeMargin - BlockH;
		break;
	case ETAPocketSlotSide::Bottom:
		Origin.X = AnchorCanvas.X - BlockW * 0.5f;
		Origin.Y = BottomRight.Y + EdgeMargin;
		break;
	}

	return Origin;
}

void UTAClothingPanelWidget::CreateLine(const FVector2D& From, const FVector2D& To)
{
	EnsureCanvas();
	if (!CanvasRoot || !WidgetTree)
	{
		return;
	}

	UImage* LineImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	if (!LineImage)
	{
		return;
	}

	const FVector2D Delta = To - From;
	const float Length = Delta.Size();
	if (Length < 1.f)
	{
		return;
	}

	const float AngleDeg = FMath::RadiansToDegrees(FMath::Atan2(Delta.Y, Delta.X));
	const FVector2D Mid = (From + To) * 0.5f;
	const float Thickness = 2.f;

	CanvasRoot->AddChild(LineImage);
	if (UCanvasPanelSlot* LineSlot = Cast<UCanvasPanelSlot>(LineImage->Slot))
	{
		LineSlot->SetAnchors(FAnchors(0.f, 0.f));
		LineSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		LineSlot->SetPosition(Mid);
		LineSlot->SetSize(FVector2D(Length, Thickness));
		LineSlot->SetZOrder(0);
	}

	LineImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
	FWidgetTransform Xform;
	Xform.Angle = AngleDeg;
	LineImage->SetRenderTransform(Xform);
	LineImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.7f));
	LineImage->SetVisibility(ESlateVisibility::HitTestInvisible);

	GeneratedLines.Add(LineImage);
}

void UTAClothingPanelWidget::CreateSlotsForPocket(
	const FTAPocketRuntime& PocketRuntime,
	const FTAPocketDef& PocketDef,
	int32& InOutFlatIndex)
{
	EnsureCanvas();
	if (!CanvasRoot || !SlotWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateSlotsForPocket early out: Canvas=%d Class=%d"),
			CanvasRoot ? 1 : 0, SlotWidgetClass ? 1 : 0);
		return;
	}

	const int32 Num = PocketRuntime.Slots.Num();
	if (Num <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pocket %s has 0 slots"), *PocketRuntime.PocketId.ToString());
		return;
	}

	const FVector2D Anchor = PocketAnchorToCanvas(PocketDef.AnchorUV);
	const FVector2D Origin = ComputePocketSlotsOrigin(Anchor, PocketDef.SlotSide, Num);

	UE_LOG(LogTemp, Warning, TEXT("Pocket %s Num=%d Anchor=(%.0f,%.0f) Origin=(%.0f,%.0f)"),
		*PocketRuntime.PocketId.ToString(), Num, Anchor.X, Anchor.Y, Origin.X, Origin.Y);

	FVector2D PocketCenterSum = FVector2D::ZeroVector;
	int32 Placed = 0;

	for (int32 i = 0; i < Num; ++i)
	{
		UUserWidget* SlotWidget = CreateWidget<UUserWidget>(this, SlotWidgetClass);
		if (!SlotWidget)
		{
			++InOutFlatIndex;
			continue;
		}

		const int32 Row = i / SlotsPerRow;
		const int32 Col = i % SlotsPerRow;
		const FVector2D Pos(
			Origin.X + Col * (SlotSize + SlotPadding),
			Origin.Y + Row * (SlotSize + SlotPadding));

		CanvasRoot->AddChild(SlotWidget);
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(SlotWidget->Slot))
		{
			CanvasSlot->SetAnchors(FAnchors(0.f, 0.f));
			CanvasSlot->SetAlignment(FVector2D(0.f, 0.f));
			CanvasSlot->SetPosition(Pos);
			CanvasSlot->SetSize(FVector2D(SlotSize, SlotSize));
			CanvasSlot->SetZOrder(10);
			CanvasSlot->SetAutoSize(false);
		}

		SlotWidget->SetVisibility(ESlateVisibility::Visible);

		if (UTAInventorySlotWidget* InvSlot = Cast<UTAInventorySlotWidget>(SlotWidget))
		{
			InvSlot->SetSlotData(PocketRuntime.Slots[i], InOutFlatIndex);
		}

		GeneratedSlots.Add(SlotWidget);
		PocketCenterSum += Pos + FVector2D(SlotSize, SlotSize) * 0.5f;
		++Placed;
		++InOutFlatIndex;
	}

	UE_LOG(LogTemp, Warning, TEXT("Placed=%d GeneratedSlots=%d"), Placed, GeneratedSlots.Num());

	if (Placed > 0)
	{
		CreateLine(Anchor, PocketCenterSum / static_cast<float>(Placed));
	}
}

void UTAClothingPanelWidget::BuildFromClothing(const FTAClothingInstance& Instance, int32 FlatIndexStart)
{
	UE_LOG(LogTemp, Warning, TEXT("Clothing Build: Def=%s Pockets=%d SlotClass=%s"),
		Instance.Definition ? *Instance.Definition->GetName() : TEXT("None"),
		Instance.Pockets.Num(),
		SlotWidgetClass ? *SlotWidgetClass->GetName() : TEXT("None"));
	EnsureCanvas();
	ClearPanel();

	if (!Instance.Definition)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	SetVisibility(ESlateVisibility::Visible);
	CurrentDef = Instance.Definition;
	SetClothingIcon(CurrentDef);

	const TArray<FTAPocketDef>& PocketDefs = CurrentDef->Pockets;
	int32 FlatIndex = FlatIndexStart;

	for (int32 p = 0; p < Instance.Pockets.Num(); ++p)
	{
		FTAPocketDef PocketDef;
		if (PocketDefs.IsValidIndex(p))
		{
			PocketDef = PocketDefs[p];
		}
		else
		{
			PocketDef.PocketId = Instance.Pockets[p].PocketId;
			PocketDef.SlotCount = Instance.Pockets[p].Slots.Num();
			PocketDef.AnchorUV = FVector2D(0.5f, 0.5f);
			PocketDef.SlotSide = ETAPocketSlotSide::Right;
		}

		CreateSlotsForPocket(Instance.Pockets[p], PocketDef, FlatIndex);
	}
}