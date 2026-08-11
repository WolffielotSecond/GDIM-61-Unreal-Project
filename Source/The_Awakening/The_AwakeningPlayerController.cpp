// Copyright Epic Games, Inc. All Rights Reserved.


#include "The_AwakeningPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "The_Awakening.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "Core/TAInputIconSubsystem.h"

void AThe_AwakeningPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogThe_Awakening, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void AThe_AwakeningPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool AThe_AwakeningPlayerController::InputKey(const FInputKeyParams& Params)
{
	// 先让父类处理
	const bool bResult = Super::InputKey(Params);

	// 只在有实际按键事件时通知
	if (Params.Event == IE_Pressed || Params.Event == IE_Repeat || Params.Event == IE_Axis)
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UTAInputIconSubsystem* IconSys = GI->GetSubsystem<UTAInputIconSubsystem>())
			{
				IconSys->NotifyInputKey(Params.Key);
			}
		}
	}

	return bResult;
}