#include "Core/TAInputIconSubsystem.h"
#include "Engine/Texture2D.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

void UTAInputIconSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CurrentDeviceType = EInputDeviceType::KeyboardMouse;
}

void UTAInputIconSubsystem::SetCurrentDeviceType(EInputDeviceType NewType)
{
	if (CurrentDeviceType != NewType)
	{
		CurrentDeviceType = NewType;
		OnInputDeviceChanged.Broadcast();
	}
}

void UTAInputIconSubsystem::NotifyInputKey(const FKey& Key)
{
	if (!Key.IsValid())
	{
		return;
	}

	EInputDeviceType NewType = CurrentDeviceType;

	if (Key.IsGamepadKey())
	{
		NewType = EInputDeviceType::Xbox;
	}
	else
	{
		NewType = EInputDeviceType::KeyboardMouse;
	}

	if (NewType != CurrentDeviceType)
	{
		CurrentDeviceType = NewType;
		OnInputDeviceChanged.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("Input device changed"));
	}
}

FString UTAInputIconSubsystem::KeyToAssetName(FKey Key) const
{
	if (!Key.IsValid())
	{
		return FString();
	}

	// 特殊按键
	if (Key == EKeys::SpaceBar) return TEXT("space");
	if (Key == EKeys::Escape) return TEXT("esc");
	if (Key == EKeys::Enter) return TEXT("enter");
	if (Key == EKeys::BackSpace) return TEXT("backspace");
	if (Key == EKeys::Tab) return TEXT("tab");
	if (Key == EKeys::CapsLock) return TEXT("caps_lock");
	if (Key == EKeys::LeftShift || Key == EKeys::RightShift) return TEXT("shift");
	if (Key == EKeys::LeftControl || Key == EKeys::RightControl) return TEXT("ctrl");
	if (Key == EKeys::LeftAlt || Key == EKeys::RightAlt) return TEXT("alt");
	if (Key == EKeys::LeftCommand || Key == EKeys::RightCommand) return TEXT("command");

	// 鼠标
	if (Key == EKeys::LeftMouseButton) return TEXT("left");
	if (Key == EKeys::RightMouseButton) return TEXT("right");
	if (Key == EKeys::MiddleMouseButton) return TEXT("middle");
	if (Key == EKeys::ThumbMouseButton) return TEXT("side_up");
	if (Key == EKeys::ThumbMouseButton2) return TEXT("side_down");

	// Xbox / 通用手柄
	if (Key == EKeys::Gamepad_FaceButton_Bottom) return TEXT("a");
	if (Key == EKeys::Gamepad_FaceButton_Right) return TEXT("b");
	if (Key == EKeys::Gamepad_FaceButton_Left) return TEXT("x");
	if (Key == EKeys::Gamepad_FaceButton_Top) return TEXT("y");
	if (Key == EKeys::Gamepad_LeftShoulder) return TEXT("lb");
	if (Key == EKeys::Gamepad_RightShoulder) return TEXT("rb");
	if (Key == EKeys::Gamepad_LeftTrigger) return TEXT("lt");
	if (Key == EKeys::Gamepad_RightTrigger) return TEXT("rt");

	// 默认转换
	FString Name = Key.ToString().ToLower();
	Name.ReplaceInline(TEXT(" "), TEXT("_"));
	Name.ReplaceInline(TEXT("-"), TEXT("_"));
	return Name;
}

FString UTAInputIconSubsystem::BuildIconPath(FKey Key) const
{
	const FString AssetName = KeyToAssetName(Key);
	if (AssetName.IsEmpty())
	{
		return FString();
	}

	FString Folder;
	switch (CurrentDeviceType)
	{
	case EInputDeviceType::KeyboardMouse:
		Folder = Key.IsMouseButton() ? TEXT("mouse") : TEXT("key");
		break;
	case EInputDeviceType::Xbox:
		Folder = TEXT("xboxseries");
		break;
	case EInputDeviceType::PS5:
		Folder = TEXT("ps5");
		break;
	case EInputDeviceType::Switch:
		Folder = TEXT("switch");
		break;
	}

	return FString::Printf(TEXT("%s/%s/%s"), *IconRootPath, *Folder, *AssetName);
}

UTexture2D* UTAInputIconSubsystem::LoadIconFromPath(const FString& Path) const
{
	if (Path.IsEmpty())
	{
		return nullptr;
	}

	return LoadObject<UTexture2D>(nullptr, *Path);
}

UTexture2D* UTAInputIconSubsystem::GetIconForKey(FKey Key) const
{
	const FString Path = BuildIconPath(Key);
	return LoadIconFromPath(Path);
}

UTexture2D* UTAInputIconSubsystem::GetIconForAction(UInputAction* Action) const
{
	if (!Action)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return nullptr;
	}

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return nullptr;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSubsystem)
	{
		return nullptr;
	}

	const TArray<FKey> MappedKeys = InputSubsystem->QueryKeysMappedToAction(Action);
	if (MappedKeys.Num() == 0)
	{
		return nullptr;
	}

	for (const FKey& Key : MappedKeys)
	{
		if (!Key.IsValid())
		{
			continue;
		}

		if (CurrentDeviceType == EInputDeviceType::KeyboardMouse)
		{
			if (!Key.IsGamepadKey())
			{
				return GetIconForKey(Key);
			}
		}
		else
		{
			if (Key.IsGamepadKey())
			{
				return GetIconForKey(Key);
			}
		}
	}

	return GetIconForKey(MappedKeys[0]);
}