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

	/** 获取指定 InputAction 当前绑定按键的图标（取第一个有效绑定） */
	UFUNCTION(BlueprintCallable, Category = "InputIcon")
	UTexture2D* GetIconForAction(UInputAction* Action) const;

	/** 当前输入设备类型 */
	UFUNCTION(BlueprintCallable, Category = "InputIcon")
	EInputDeviceType GetCurrentDeviceType() const { return CurrentDeviceType; }

	/** 设置当前输入设备类型（后续可由输入检测自动更新） */
	UFUNCTION(BlueprintCallable, Category = "InputIcon")
	void SetCurrentDeviceType(EInputDeviceType NewType);

	/** 由外部（PlayerController / Character）在有输入时调用 */
	UFUNCTION(BlueprintCallable, Category = "InputIcon")
	void NotifyInputKey(const FKey& Key);

protected:
	/** 根据设备和按键生成资源路径 */
	FString BuildIconPath(FKey Key) const;

	/** 从路径加载 Texture */
	UTexture2D* LoadIconFromPath(const FString& Path) const;

	/** 把 FKey 转成资源文件名 */
	FString KeyToAssetName(FKey Key) const;

	/** 根据最近输入更新设备类型 */
	void UpdateDeviceTypeFromKey(const FKey& Key);

protected:
	UPROPERTY()
	EInputDeviceType CurrentDeviceType = EInputDeviceType::KeyboardMouse;

	/** 图标根目录 */
	UPROPERTY(EditDefaultsOnly, Category = "InputIcon")
	FString IconRootPath = TEXT("/Game/UI/KeyImage");
};