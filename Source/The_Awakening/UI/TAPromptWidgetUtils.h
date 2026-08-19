#pragma once

#include "CoreMinimal.h"

class UUserWidget;
class UTexture2D;

struct THE_AWAKENING_API FTAPromptWidgetUtils
{
	/** 控件名为 Image_Key 和 Text_Prompt */
	static void ApplyPrompt(
		UUserWidget* Widget,
		UTexture2D* KeyIcon,
		const FText& PromptText,
		float TargetIconHeight = 40.f);
};