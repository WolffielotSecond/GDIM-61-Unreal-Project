#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/TAPromptTypes.h"
#include "TAPromptComponent.generated.h"

class UInputAction;
class UTAParkourComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THE_AWAKENING_API UTAPromptComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTAPromptComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 主更新：收集、按类型取最近、显隐 */
	UFUNCTION(BlueprintCallable, Category = "Prompt")
	void UpdatePrompts();

	/** 语言/设备变化后刷新当前显示中的提示 */
	UFUNCTION(BlueprintCallable, Category = "Prompt")
	void RefreshVisiblePrompts();

protected:
	void CollectCandidates(TArray<FTAPromptCandidate>& OutCandidates);
	void CollectInteractCandidates(TArray<FTAPromptCandidate>& OutCandidates);
	void CollectParkourCandidates(TArray<FTAPromptCandidate>& OutCandidates);

	void ApplyWinners(const TMap<ETAPromptType, FTAPromptCandidate>& Best);
	void HideAllTracked();

	void ShowCandidate(const FTAPromptCandidate& Candidate);
	void HideCandidate(const FTAPromptCandidate& Candidate);
	void RefreshCandidate(const FTAPromptCandidate& Candidate);

	FString GetDefaultTextId(ETAPromptType Type) const;
	UInputAction* GetActionForType(ETAPromptType Type) const;

public:
	/** 与交互检测共用距离 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt")
	float PromptRadius = 250.f;

	UPROPERTY(EditAnywhere, Category = "Prompt|Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category = "Prompt|Input")
	TObjectPtr<UInputAction> ParkourJumpAction;

	UPROPERTY(EditAnywhere, Category = "Prompt|Input")
	TObjectPtr<UInputAction> ParkourDropAction;

	UPROPERTY(EditAnywhere, Category = "Prompt|Text")
	FString InteractTextId = TEXT("Interact_Default");

	UPROPERTY(EditAnywhere, Category = "Prompt|Text")
	FString ParkourJumpTextId = TEXT("Parkour_Jump");

	UPROPERTY(EditAnywhere, Category = "Prompt|Text")
	FString ParkourDropTextId = TEXT("Parkour_Drop");

	//显示UI的text的relative位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt|Layout")
	float InteractPromptZ = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt|Layout")
	float ParkourPromptZ = -50.0f;

protected:
	/** 上一帧正在显示的提示（用于 Hide） */
	UPROPERTY()
	TMap<ETAPromptType, FTAPromptCandidate> ActivePrompts;

	TWeakObjectPtr<APawn> OwnerPawn;
};