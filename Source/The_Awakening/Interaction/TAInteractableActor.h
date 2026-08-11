#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TAInteractable.h"
#include "TAInteractableActor.generated.h"

class UWidgetComponent;

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

	/** 显示 / 隐藏交互提示 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetPromptVisible(bool bVisible);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> InteractPromptComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractText;
};