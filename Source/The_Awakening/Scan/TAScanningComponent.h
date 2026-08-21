// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Materials/MaterialParameterCollection.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TAScanningComponent.generated.h"

UENUM(BlueprintType)
enum class ETAScanState : uint8
{
	FadeIn,
	FadedIn,
	FadeOut,
	FadedOut,
	Invalid
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THE_AWAKENING_API UTAScanningComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTAScanningComponent();
	ETAScanState GetScanState() const;
	AActor* GetScanPPActor();
	bool StartScan();
	bool EndScan();

	// Settings | Speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scan|Settings|Speed")
	float FadeInSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scan|Settings|Speed")
	float FadeOutSpeed = 2.0f;

	// Settings | Actor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scan|Settings|Actor")
	bool bDestroyActor = true;

	// Settings | Material
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scan|Settings|Material")
	TObjectPtr<UCurveFloat> BlendCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scan|Settings|Material")
	TObjectPtr<UMaterialInterface> PostProcessMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scan|Settings|Material")
	float Range = 6000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scan|Settings|Material")
	float GridCellSize = 500.0f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	bool UpdateScanState(ETAScanState NewState, bool bForce);

	bool UpdateScanTime(float DeltaTime);

	void DestroyScanPPActor();

	void UpdateScanPPBlendWeight();

	UPROPERTY()
	TObjectPtr<UMaterialParameterCollection> ScanParameterCollection;
		

	ETAScanState ScanState = ETAScanState::FadedIn;

	UPROPERTY()
	TObjectPtr<AActor> ScanActor;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> PostProcessMID;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	float ScanNormalizedTime = 0.0f;
};
