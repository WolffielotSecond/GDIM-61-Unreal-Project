// Fill out your copyright notice in the Description page of Project Settings.


#include "Scan/TAScanningComponent.h"

#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMaterialLibrary.h"

// Sets default values for this component's properties
UTAScanningComponent::UTAScanningComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	//找Material Parameter Collection
	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> MPCRef(
		TEXT("/Game/Materials/Collections/MPC_Scan.MPC_Scan")
	);

	//找到了喵
	if (MPCRef.Succeeded())
	{
		ScanParameterCollection = MPCRef.Object;
	}

}


// Called when the game starts
void UTAScanningComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(GetOwner());

	if (PlayerController)
	{
		UpdateScanState(ETAScanState::FadedOut, true);

		UKismetMaterialLibrary::SetScalarParameterValue(
			GetWorld(),
			ScanParameterCollection,
			FName("GridCellSize"),
			GridCellSize
		);

		UKismetMaterialLibrary::SetScalarParameterValue(
			GetWorld(),
			ScanParameterCollection,
			FName("Range"),
			Range
		);
	}
	else
	{
		UpdateScanState(ETAScanState::Invalid, true);
	}
	
}


// Called every frame
void UTAScanningComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateScanTime(DeltaTime);

	//debug print time
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			0.0f,
			FColor::Green,
			FString::Printf(TEXT("ScanNormalizedTime: %.3f"), ScanNormalizedTime)
		);
	}

	switch (ScanState)
	{
		case ETAScanState::FadeIn:
		case ETAScanState::FadeOut:
			UpdateScanPPBlendWeight();
			break;

		case ETAScanState::FadedIn:
		case ETAScanState::FadedOut:
			break;

		case ETAScanState::Invalid:
			return;
	}

	if (!PlayerController)
	{
		return;
	}

	APawn* ControlledPawn = PlayerController->GetPawn();

	if (!ControlledPawn)
	{
		return;
	}

	const FVector PlayerLocation = ControlledPawn->GetActorLocation();

	UKismetMaterialLibrary::SetVectorParameterValue(
		GetWorld(),
		ScanParameterCollection,
		FName("Location"),
		FLinearColor(
			PlayerLocation.X,
			PlayerLocation.Y,
			PlayerLocation.Z,
			1.0f
		)
	);
}

ETAScanState UTAScanningComponent::GetScanState() const
{
	return ScanState;
}

bool UTAScanningComponent::UpdateScanState(ETAScanState NewState, bool bForce)
{
	if (NewState == ScanState && !bForce)
	{
		return false;
	}
	//离开旧状态
	switch (ScanState)
	{
		case ETAScanState::FadedIn:
			UKismetMaterialLibrary::SetScalarParameterValue(
				GetWorld(),
				ScanParameterCollection,
				FName("bFadedIn"),
				0.0f
			);
			break;

		case ETAScanState::FadedOut:
			UKismetMaterialLibrary::SetScalarParameterValue(
				GetWorld(),
				ScanParameterCollection,
				FName("bFadedOut"),
				0.0f
			);
			SetComponentTickEnabled(true);
			break;
		
		default:
			break;
	}
	//更新状态
	ScanState = NewState;

	//进入新状态
	switch (ScanState)
	{
		case ETAScanState::FadedIn:
			UKismetMaterialLibrary::SetScalarParameterValue(
				GetWorld(),
				ScanParameterCollection,
				FName("bFadedIn"),
				1.0f
			);

			UpdateScanPPBlendWeight();
			break;

		case ETAScanState::FadedOut:
			UKismetMaterialLibrary::SetScalarParameterValue(
				GetWorld(),
				ScanParameterCollection,
				FName("bFadedOut"),
				1.0f
			);

			UpdateScanPPBlendWeight();

			SetComponentTickEnabled(false);
			DestroyScanPPActor();
			break;

		default:
			break;
	}

	return true;

}

bool UTAScanningComponent::UpdateScanTime(float DeltaTime)
{
	switch (ScanState)
	{
	case ETAScanState::FadeIn:
		ScanNormalizedTime += DeltaTime * FadeInSpeed;

		if (ScanNormalizedTime >= 1.0f)
		{
			ScanNormalizedTime = 1.0f;
			UpdateScanState(ETAScanState::FadedIn, false);
		}

		break;


	case ETAScanState::FadeOut:
		ScanNormalizedTime -= DeltaTime * FadeOutSpeed;

		if (ScanNormalizedTime <= 0.0f)
		{
			ScanNormalizedTime = 0.0f;
			UpdateScanState(ETAScanState::FadedOut, false);
		}

		break;


	case ETAScanState::FadedIn:
	case ETAScanState::FadedOut:
	case ETAScanState::Invalid:
		return true;

	default:
		break;
	}


	UKismetMaterialLibrary::SetScalarParameterValue(
		GetWorld(),
		ScanParameterCollection,
		FName("NormalizedTime"),
		ScanNormalizedTime
	);

	return true;
}

AActor* UTAScanningComponent::GetScanPPActor()
{
	return nullptr;
}

void UTAScanningComponent::DestroyScanPPActor()
{
}

void UTAScanningComponent::UpdateScanPPBlendWeight()
{
}

bool UTAScanningComponent::StartScan()
{
	switch (ScanState)
	{
		case ETAScanState::FadeOut:
		case ETAScanState::FadedOut:
			return UpdateScanState(ETAScanState::FadeIn, false);

		case ETAScanState::FadeIn:
		case ETAScanState::FadedIn:
		case ETAScanState::Invalid:
			return false;
	}

	return false;
}

bool UTAScanningComponent::EndScan()
{
	switch (ScanState)
	{
		case ETAScanState::FadeIn:
		case ETAScanState::FadedIn:
			return UpdateScanState(ETAScanState::FadeOut, false);

		case ETAScanState::FadeOut:
		case ETAScanState::FadedOut:
		case ETAScanState::Invalid:
			return false;
	}

	return false;
}
