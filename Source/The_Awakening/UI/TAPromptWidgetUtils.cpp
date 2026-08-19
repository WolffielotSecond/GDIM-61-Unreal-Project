#include "UI/TAPromptWidgetUtils.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"

void FTAPromptWidgetUtils::ApplyPrompt(
	UUserWidget* Widget,
	UTexture2D* KeyIcon,
	const FText& PromptText,
	float TargetIconHeight)
{
	if (!Widget)
	{
		return;
	}

	if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("Text_Prompt"))))
	{
		TextBlock->SetText(PromptText);
	}

	if (UImage* Image = Cast<UImage>(Widget->GetWidgetFromName(TEXT("Image_Key"))))
	{
		if (KeyIcon)
		{
			const float TexW = static_cast<float>(KeyIcon->GetSizeX());
			const float TexH = static_cast<float>(FMath::Max(KeyIcon->GetSizeY(), 1));
			const float DrawW = TexW * (TargetIconHeight / TexH);

			FSlateBrush Brush;
			Brush.SetResourceObject(KeyIcon);
			Brush.ImageSize = FVector2D(DrawW, TargetIconHeight);
			Image->SetBrush(Brush);
			Image->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}