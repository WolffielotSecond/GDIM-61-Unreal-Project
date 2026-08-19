#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TAInteractable.h"
#include "TAInteractableActor.generated.h"

class UWidgetComponent;
class UTexture2D;

UCLASS()
class THE_AWAKENING_API ATAInteractableActor : public AActor, public ITAInteractable
{
	GENERATED_BODY()

public:
	ATAInteractableActor();

protected:
	virtual void BeginPlay() override;

public:
	// ITAInteractable
	virtual void OnInteract_Implementation(AActor* Interactor) override;
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual FText GetInteractText_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetPromptVisible(bool bVisible);

	/** 供 PromptComponent 刷新图标与文字 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void RefreshPrompt(UTexture2D* KeyIcon, const FText& PromptText);

	UWidgetComponent* GetPromptWidgetComponent() const { return InteractPromptComponent; }

	/** 本地化 ID（与物品 DisplayName 一样填 ID 字符串） */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	FString GetPromptTextId() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> InteractPromptComponent;

	/**
	 * 本地化 ID，例如 Interact_Default
	 * 若你仍用 FText 在编辑器里填 ID，可用 InteractText.ToString()；
	 * 这里单独用 FString 更清晰。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FString InteractTextId = TEXT("Interact_Default");

	/** 旧字段：可逐步废弃，避免和 ID 混用 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractText;
};