#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TAParkourMarker.generated.h"

class UBoxComponent;
class UChildActorComponent;

UENUM(BlueprintType)
enum class ETAParkourMarkerType : uint8
{
	JumpToPoint UMETA(DisplayName = "跨台跳(空格)"),
	DropDown    UMETA(DisplayName = "高台跳下(Ctrl)")
};

UCLASS()
class THE_AWAKENING_API ATAParkourMarker : public AActor
{
	GENERATED_BODY()

public:
	ATAParkourMarker();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerBox;

	/** 默认落点子 Actor，地编在关卡里拖动它即可 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UChildActorComponent> LandingTargetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parkour")
	ETAParkourMarkerType MarkerType = ETAParkourMarkerType::JumpToPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parkour")
	TObjectPtr<AActor> LandingTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parkour", meta = (ClampMin = "0"))
	float ArcHeight = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parkour", meta = (ClampMin = "0.05"))
	float JumpDuration = 0.5f;

	UFUNCTION(BlueprintCallable, Category = "Parkour")
	bool IsValidMarker() const;

	UFUNCTION(BlueprintCallable, Category = "Parkour")
	FVector GetLandingLocation() const;

protected:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void BindLandingTargetFromChild();
};