#include "UI/Inventory/TAInventoryPanelWidget.h"
#include "UI/Inventory/TAInventorySlotWidget.h"
#include "UI/Inventory/TAClothingPanelWidget.h"
#include "Inventory/TAInventoryComponent.h"
#include "Inventory/TAClothingDefinition.h"
#include "Core/TAPlayerState.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void UTAInventoryPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureLayout();
}

void UTAInventoryPanelWidget::NativeDestruct()
{
	if (Inventory)
	{
		Inventory->OnInventoryUpdated.RemoveDynamic(this, &UTAInventoryPanelWidget::HandleInventoryUpdated);
	}
	Super::NativeDestruct();
}

void UTAInventoryPanelWidget::EnsureLayout()
{
	if (RootOverlay)
	{
		return;
	}
	BuildLayout();
}

void UTAInventoryPanelWidget::BuildLayout()
{
	if (!WidgetTree)
	{
		return;
	}

	RootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("RootOverlay"));
	WidgetTree->RootWidget = RootOverlay;

	// 全屏遮罩
	DimBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DimBorder"));
	if (UOverlaySlot* DimSlot = RootOverlay->AddChildToOverlay(DimBorder))
	{
		DimSlot->SetHorizontalAlignment(HAlign_Fill);
		DimSlot->SetVerticalAlignment(VAlign_Fill);
	}
	DimBorder->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.55f));

	// 中央面板
	PanelBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PanelBorder"));
	if (UOverlaySlot* PanelSlot = RootOverlay->AddChildToOverlay(PanelBorder))
	{
		PanelSlot->SetHorizontalAlignment(HAlign_Center);
		PanelSlot->SetVerticalAlignment(VAlign_Center);
	}
	PanelBorder->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.07f, 0.95f));
	PanelBorder->SetPadding(FMargin(16.f));

	USizeBox* PanelSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("PanelSize"));
	PanelSize->SetWidthOverride(PanelWidth);
	PanelSize->SetHeightOverride(PanelHeight);
	PanelBorder->SetContent(PanelSize);

	UVerticalBox* RootVBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootVBox"));
	PanelSize->AddChild(RootVBox);

	// 金钱
	MoneyText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("MoneyText"));
	MoneyText->SetText(FText::FromString(TEXT("细胞: 0")));
	if (UVerticalBoxSlot* MoneySlot = RootVBox->AddChildToVerticalBox(MoneyText))
	{
		MoneySlot->SetPadding(FMargin(0.f, 0.f, 0.f, 12.f));
		MoneySlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	}

	// 主区：左栏 + 右衣服
	UHorizontalBox* MainHBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("MainHBox"));
	if (UVerticalBoxSlot* MainSlot = RootVBox->AddChildToVerticalBox(MainHBox))
	{
		MainSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	// 左栏
	USizeBox* LeftSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("LeftSize"));
	LeftSize->SetWidthOverride(LeftColumnWidth);
	if (UHorizontalBoxSlot* LeftOuterSlot = MainHBox->AddChildToHorizontalBox(LeftSize))
	{
		LeftOuterSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		LeftOuterSlot->SetPadding(FMargin(0.f, 0.f, 12.f, 0.f));
		LeftOuterSlot->SetVerticalAlignment(VAlign_Fill);
	}

	LeftColumn = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("LeftColumn"));
	LeftSize->AddChild(LeftColumn);

	auto AddLeftSlot = [&](TObjectPtr<UTAInventorySlotWidget>& OutSlot, const FName& Name)
		{
			OutSlot = CreateWidget<UTAInventorySlotWidget>(this, UTAInventorySlotWidget::StaticClass());
			if (!OutSlot)
			{
				return;
			}
			if (UVerticalBoxSlot* VSlot = LeftColumn->AddChildToVerticalBox(OutSlot))
			{
				VSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
				VSlot->SetHorizontalAlignment(HAlign_Center);
				VSlot->SetVerticalAlignment(VAlign_Center);
				VSlot->SetPadding(FMargin(0.f, 4.f));
			}
		};

	AddLeftSlot(OuterEquipSlot, TEXT("OuterEquip"));
	StorySlots.SetNum(4);
	for (int32 i = 0; i < 4; ++i)
	{
		AddLeftSlot(StorySlots[i], *FString::Printf(TEXT("Story_%d"), i));
		if (StorySlots[i])
		{
			StorySlots[i]->SetEmpty();
		}
	}
	if (OuterEquipSlot)
	{
		OuterEquipSlot->SetEmpty();
	}

	// 右栏：外套 | 内衬
	UHorizontalBox* ClothingHBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ClothingHBox"));
	if (UHorizontalBoxSlot* ClothSlot = MainHBox->AddChildToHorizontalBox(ClothingHBox))
	{
		ClothSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	OuterClothingPanel = CreateWidget<UTAClothingPanelWidget>(this, UTAClothingPanelWidget::StaticClass());
	InnerClothingPanel = CreateWidget<UTAClothingPanelWidget>(this, UTAClothingPanelWidget::StaticClass());

	if (OuterClothingPanel)
	{
		OuterClothingPanel->SlotWidgetClass = UTAInventorySlotWidget::StaticClass();
		if (UHorizontalBoxSlot* S = ClothingHBox->AddChildToHorizontalBox(OuterClothingPanel))
		{
			S->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		}
		OuterClothingPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (InnerClothingPanel)
	{
		InnerClothingPanel->SlotWidgetClass = UTAInventorySlotWidget::StaticClass();
		if (UHorizontalBoxSlot* S = ClothingHBox->AddChildToHorizontalBox(InnerClothingPanel))
		{
			S->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		}
	}
}

void UTAInventoryPanelWidget::Init(UTAInventoryComponent* InInventory)
{
	EnsureLayout();

	if (Inventory)
	{
		Inventory->OnInventoryUpdated.RemoveDynamic(this, &UTAInventoryPanelWidget::HandleInventoryUpdated);
	}

	Inventory = InInventory;
	if (Inventory)
	{
		Inventory->OnInventoryUpdated.AddDynamic(this, &UTAInventoryPanelWidget::HandleInventoryUpdated);
	}

	RefreshAll();
}

void UTAInventoryPanelWidget::HandleInventoryUpdated()
{
	RefreshAll();
}

void UTAInventoryPanelWidget::RefreshAll()
{
	EnsureLayout();
	if (!Inventory)
	{
		return;
	}

	// 金钱
	if (MoneyText)
	{
		int32 Money = 0;
		if (APlayerController* PC = GetOwningPlayer())
		{
			if (ATAPlayerState* PS = PC->GetPlayerState<ATAPlayerState>())
			{
				Money = PS->GetMoney();
			}
		}
		MoneyText->SetText(FText::FromString(FString::Printf(TEXT("细胞: %d"), Money)));
	}

	// 剧情格
	const TArray<FTAInventorySlot>& Stories = Inventory->GetStorySlots();
	for (int32 i = 0; i < StorySlots.Num(); ++i)
	{
		if (!StorySlots[i])
		{
			continue;
		}
		if (Stories.IsValidIndex(i) && !Stories[i].IsEmpty())
		{
			StorySlots[i]->SetSlotData(Stories[i], -100 - i); // 负号标记非扁平格
		}
		else
		{
			StorySlots[i]->SetEmpty();
		}
	}

	// 外套装备格：只显示衣服图标占位（用空 Slot + 以后可扩展）
	if (OuterEquipSlot)
	{
		if (Inventory->HasOuterClothing() && Inventory->GetOuterClothing().Definition)
		{
			// 暂无空数据占位；可后续 SetClothingIcon 接口
			OuterEquipSlot->SetEmpty();
		}
		else
		{
			OuterEquipSlot->SetEmpty();
		}
	}

	// 衣服面板
	if (InnerClothingPanel)
	{
		InnerClothingPanel->BuildFromClothing(Inventory->GetInnerClothing(), 0);
	}

	if (OuterClothingPanel)
	{
		if (Inventory->HasOuterClothing())
		{
			OuterClothingPanel->SetVisibility(ESlateVisibility::Visible);
			const int32 InnerTotal = Inventory->GetInnerClothing().Definition
				? Inventory->GetInnerClothing().Definition->GetTotalSlotCount()
				: 0;
			// 更准确：用运行时口袋格数
			int32 InnerSlots = 0;
			for (const FTAPocketRuntime& P : Inventory->GetInnerClothing().Pockets)
			{
				InnerSlots += P.Slots.Num();
			}
			OuterClothingPanel->BuildFromClothing(Inventory->GetOuterClothing(), InnerSlots);
		}
		else
		{
			OuterClothingPanel->ClearPanel();
			OuterClothingPanel->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}