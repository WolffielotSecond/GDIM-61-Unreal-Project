#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InputCoreTypes.h"
#include "TAInputIconSubsystem.generated.h"

class UInputAction;
class UTexture2D;

UENUM(BlueprintType)
enum class EInputDeviceType : uint8
{
	KeyboardMouse,
	Xbox,
	PS5,
	Switch
};

UCLASS()
class THE_AWAKENING_API UTAInputIconSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** 获取指定按键对应的图标 */
	UFUNCTION(BlueprintCallable, Category = "InputIcon")
	UTexture2D* GetIconForKey(FKey Key) const;

	/** 获取指定 InputAction 当前绑定按键的图标 */
	UFUNCTION(BlueprintCallable, Category = "InputIcon")
	UTexture2D* GetIconForAction(UInputAction* Action) const;

	/** 当前输入设备类型 */
	UFUNCTION(BlueprintCallable, Category = "InputIcon")
	EInputDeviceType GetCurrentDeviceType() const { return CurrentDeviceType; }

	/** 手动设置当前输入设备类型 */
	UFUNCTION(BlueprintCallable, Category = "InputIcon")
	void SetCurrentDeviceType(EInputDeviceType NewType);

	/** 根据实际按下的按键自动更新设备类型 */
	UFUNCTION(BlueprintCallable, Category = "InputIcon")
	void NotifyInputKey(const FKey& Key);

protected:
	FString BuildIconPath(FKey Key) const;
	UTexture2D* LoadIconFromPath(const FString& Path) const;
	FString KeyToAssetName(FKey Key) const;

protected:
	UPROPERTY()
	EInputDeviceType CurrentDeviceType = EInputDeviceType::KeyboardMouse;

	UPROPERTY(EditDefaultsOnly, Category = "InputIcon")
	FString IconRootPath = TEXT("/Game/UI/KeyImage");
};