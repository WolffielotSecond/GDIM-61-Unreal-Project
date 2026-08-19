#pragma once

#include "CoreMinimal.h"
#include "TAPromptTypes.generated.h"

class UInputAction;
class UWidgetComponent;
class AActor;

UENUM(BlueprintType)
enum class ETAPromptType : uint8
{
	Interact     UMETA(DisplayName = "交互"),
	ParkourJump  UMETA(DisplayName = "跨越"),
	ParkourDrop  UMETA(DisplayName = "跳下")
};

USTRUCT(BlueprintType)
struct FTAPromptCandidate
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ETAPromptType Type = ETAPromptType::Interact;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> Source;

	UPROPERTY(BlueprintReadOnly)
	FVector WorldLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	float DistanceSq = 0.f;

	UPROPERTY(BlueprintReadOnly)
	FString TextId;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UInputAction> Action = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UWidgetComponent> PromptWidget;
};