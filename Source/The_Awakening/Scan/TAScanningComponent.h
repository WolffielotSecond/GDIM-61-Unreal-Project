// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Materials/MaterialParameterCollection.h"
#include "CoreMinimal.h"
#include "TimerManager.h"
#include "Components/ActorComponent.h"
#include "TAScanningComponent.generated.h"

class ATAScanningActor;
class ATA_HighlightPPActor;

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
	ATAScanningActor* GetScanPPActor();
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

	// Settings | Highlight
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scan|Settings|Highlight")
	float ShowDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scan|Settings|Highlight")
	float HideDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scan|Settings|Highlight")
	float HighlightFadeTime = 0.3f;

	//之后技能树要修改这两个变量直接调用这个function即可，ShowDelay减小（按下扫描后物品reveal时间缩短），HideDelay增加（松开扫描后物品会花更长时间才会隐藏）
	UFUNCTION(BlueprintCallable, Category = "Scan|Highlight")
	void SetShowDelay(float NewShowDelay);

	UFUNCTION(BlueprintCallable, Category = "Scan|Highlight")
	void SetHideDelay(float NewHideDelay);

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
		

	ETAScanState ScanState = ETAScanState::FadedOut;

	UPROPERTY()
	TObjectPtr<ATAScanningActor> ScanActor;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> PostProcessMID;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	float ScanNormalizedTime = 0.0f;

	//Highlight

	void UpdateHighlight(float DeltaTime);

	ATA_HighlightPPActor* GetHighlightPPActor();

	void DestroyHighlightPPActor();

	void StartHighlightHideTimer();

	void CancelHighlightHideTimer();

	UPROPERTY()
	TObjectPtr<ATA_HighlightPPActor> HighlightPPActor;

	float HighlightShowProgress = 0.0f;

	FTimerHandle HighlightHideTimerHandle;

	float HighlightValue = 0.0f;

	bool bHighlightFadingIn = false;

	bool bHighlightFadingOut = false;

	void BeginHighlightFadeIn();

	void BeginHighlightFadeOut();
};
