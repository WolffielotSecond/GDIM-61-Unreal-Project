#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Movement/TAParkourMarker.h"
#include "TAParkourComponent.generated.h"
class UInputAction;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THE_AWAKENING_API UTAParkourComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTAParkourComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	const TArray<TObjectPtr<ATAParkourMarker>>& GetOverlappingMarkers() const { return OverlappingMarkers; }

	/** 尝试跨台跳（空格） */
	UFUNCTION(BlueprintCallable, Category = "Parkour")
	void TryParkourJump();

	/** 尝试高台跳下（Ctrl） */
	UFUNCTION(BlueprintCallable, Category = "Parkour")
	void TryParkourDrop();

	UFUNCTION(BlueprintCallable, Category = "Parkour")
	bool IsParkouring() const { return bIsParkouring; }

	//用于在动画蓝图中（什么几把中文语法

	UFUNCTION(BlueprintCallable, Category = "Parkour")
	bool GetParkourFacing() const;

	UFUNCTION(BlueprintCallable, Category = "Parkour")
	float GetParkourDuration() const { return ParkourDuration; }

	void RegisterMarker(ATAParkourMarker* Marker);
	void UnregisterMarker(ATAParkourMarker* Marker);

protected:
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	ATAParkourMarker* FindCurrentMarkerOfType(ETAParkourMarkerType Type) const;

	bool StartParkour(ATAParkourMarker* Marker);
	void FinishParkour();
	bool HasLanded() const;

	/** 抛物线：t∈[0,1] */
	static FVector EvalParabola(const FVector& Start, const FVector& End, float ArcHeight, float Alpha);

protected:

	UPROPERTY()
	TArray<TObjectPtr<ATAParkourMarker>> OverlappingMarkers;

	bool bIsParkouring = false;
	float ParkourTime = 0.f;
	float ParkourDuration = 0.5f;
	float ParkourArcHeight = 120.f;
	FVector ParkourStart = FVector::ZeroVector;
	FVector ParkourEnd = FVector::ZeroVector;

	TWeakObjectPtr<ACharacter> OwnerCharacter;
};