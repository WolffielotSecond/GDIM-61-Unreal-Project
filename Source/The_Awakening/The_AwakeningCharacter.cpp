// Copyright Epic Games, Inc. All Rights Reserved.

#include "The_AwakeningCharacter.h"
#include "Core/TAPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "The_Awakening.h"
#include "Interaction/TAInteractable.h"
#include "Interaction/TAInteractableActor.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"
#include "Movement/TAParkourComponent.h"
#include "Inventory/TAInventoryComponent.h"

AThe_AwakeningCharacter::AThe_AwakeningCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 0.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// 相机
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = CameraDistance;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 8.f;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	ParkourComponent = CreateDefaultSubobject<UTAParkourComponent>(TEXT("ParkourComponent"));
	InventoryComponent = CreateDefaultSubobject<UTAInventoryComponent>(TEXT("InventoryComponent"));
}

void AThe_AwakeningCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CameraBoom)
	{
		CameraBoom->SocketOffset = FVector(0.f, 0.f, CameraHeightOffset);
		CameraBoom->TargetArmLength = CameraDistance;
	}

	if (FollowCamera)
	{
		FollowCamera->SetRelativeRotation(FRotator(CameraPitchAngle, 0.f, 0.f));
	}
}

void AThe_AwakeningCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateInteractTarget();
	UpdateMovementInput();
}

void AThe_AwakeningCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		// 手柄摇杆移动
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThe_AwakeningCharacter::Move);
		}

		// 键盘四方向
		if (MoveForwardAction)
		{
			EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AThe_AwakeningCharacter::OnMoveForward);
			EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Completed, this, &AThe_AwakeningCharacter::OnMoveForwardReleased);
		}
		if (MoveBackwardAction)
		{
			EnhancedInputComponent->BindAction(MoveBackwardAction, ETriggerEvent::Triggered, this, &AThe_AwakeningCharacter::OnMoveBackward);
			EnhancedInputComponent->BindAction(MoveBackwardAction, ETriggerEvent::Completed, this, &AThe_AwakeningCharacter::OnMoveBackwardReleased);
		}
		if (MoveLeftAction)
		{
			EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Triggered, this, &AThe_AwakeningCharacter::OnMoveLeft);
			EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Completed, this, &AThe_AwakeningCharacter::OnMoveLeftReleased);
		}
		if (MoveRightAction)
		{
			EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AThe_AwakeningCharacter::OnMoveRight);
			EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Completed, this, &AThe_AwakeningCharacter::OnMoveRightReleased);
		}

		// Look
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AThe_AwakeningCharacter::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThe_AwakeningCharacter::Look);

		// Interact
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AThe_AwakeningCharacter::TryInteract);
		}

		if (ParkourJumpAction)
		{
			EnhancedInputComponent->BindAction(
				ParkourJumpAction, ETriggerEvent::Started, this, &AThe_AwakeningCharacter::OnParkourJump);
		}
		if (ParkourDropAction)
		{
			EnhancedInputComponent->BindAction(
				ParkourDropAction, ETriggerEvent::Started, this, &AThe_AwakeningCharacter::OnParkourDrop);
		}
	}
}

void AThe_AwakeningCharacter::Move(const FInputActionValue& Value)
{
	// 手柄摇杆直接使用
	const FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AThe_AwakeningCharacter::OnMoveForward(const FInputActionValue& Value)
{
	bMoveForward = true;
}

void AThe_AwakeningCharacter::OnMoveBackward(const FInputActionValue& Value)
{
	bMoveBackward = true;
}

void AThe_AwakeningCharacter::OnMoveLeft(const FInputActionValue& Value)
{
	bMoveLeft = true;
}

void AThe_AwakeningCharacter::OnMoveRight(const FInputActionValue& Value)
{
	bMoveRight = true;
}

void AThe_AwakeningCharacter::OnMoveForwardReleased(const FInputActionValue& Value)
{
	bMoveForward = false;
}

void AThe_AwakeningCharacter::OnMoveBackwardReleased(const FInputActionValue& Value)
{
	bMoveBackward = false;
}

void AThe_AwakeningCharacter::OnMoveLeftReleased(const FInputActionValue& Value)
{
	bMoveLeft = false;
}

void AThe_AwakeningCharacter::OnMoveRightReleased(const FInputActionValue& Value)
{
	bMoveRight = false;
}

void AThe_AwakeningCharacter::UpdateMovementInput()
{
	float FinalForward = 0.f;
	float FinalRight = 0.f;

	if (bMoveForward)  FinalForward += 1.f;
	if (bMoveBackward) FinalForward -= 1.f;
	if (bMoveRight)    FinalRight += 1.f;
	if (bMoveLeft)     FinalRight -= 1.f;

	FinalForward = FMath::Clamp(FinalForward, -1.f, 1.f);
	FinalRight = FMath::Clamp(FinalRight, -1.f, 1.f);

	if (!FMath::IsNearlyZero(FinalForward) || !FMath::IsNearlyZero(FinalRight))
	{
		DoMove(FinalRight, FinalForward);
	}
}

void AThe_AwakeningCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AThe_AwakeningCharacter::DoMove(float Right, float Forward)
{

	if (ParkourComponent && ParkourComponent->IsParkouring())
	{
		return;
	}

	if (GetController() == nullptr)
	{
		return;
	}

	const FRotator YawRotation(0.f, GetController()->GetControlRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	float SafeForward = Forward;
	float SafeRight = Right;

	if (!FMath::IsNearlyZero(Forward))
	{
		const FVector Dir = ForwardDirection * FMath::Sign(Forward);
		if (!IsSafeToMoveToward(Dir))
		{
			SafeForward = 0.f;
		}
	}

	if (!FMath::IsNearlyZero(Right))
	{
		const FVector Dir = RightDirection * FMath::Sign(Right);
		if (!IsSafeToMoveToward(Dir))
		{
			SafeRight = 0.f;
		}
	}

	if (FMath::IsNearlyZero(SafeForward) && FMath::IsNearlyZero(SafeRight))
	{
		return;
	}

	AddMovementInput(ForwardDirection, SafeForward);
	AddMovementInput(RightDirection, SafeRight);
}

void AThe_AwakeningCharacter::DoLook(float Yaw, float Pitch)
{
	float FinalYaw = bInvertCameraX ? -Yaw : Yaw;
	float FinalPitch = bInvertCameraY ? Pitch : -Pitch;

	const float SoftStartRatio = 0.2f;

	auto ApplyStrongSoftLimit = [SoftStartRatio](float Current, float Delta, float Limit) -> float
		{
			if (Limit <= KINDA_SMALL_NUMBER || FMath::IsNearlyZero(Delta))
			{
				return 0.f;
			}

			const bool bMovingOutward = (Current * Delta) > 0.f;

			if (!bMovingOutward)
			{
				return Delta;
			}

			const float AbsCurrent = FMath::Abs(Current);
			const float Ratio = AbsCurrent / Limit;

			if (Ratio < SoftStartRatio)
			{
				const float Next = Current + Delta;
				if (FMath::Abs(Next) <= Limit * SoftStartRatio)
				{
					return Delta;
				}
			}

			const float Remaining = FMath::Max(0.f, Limit - AbsCurrent);

			float T = 0.f;
			if (Ratio >= SoftStartRatio)
			{
				T = (Ratio - SoftStartRatio) / FMath::Max(1.f - SoftStartRatio, KINDA_SMALL_NUMBER);
				T = FMath::Clamp(T, 0.f, 1.f);
			}
			const float Ease = 1.f - (T * T * T);

			const float MaxStep = Remaining * FMath::Lerp(0.35f, 0.03f, T);

			float ClampedDelta = Delta * Ease;
			if (FMath::Abs(ClampedDelta) > MaxStep)
			{
				ClampedDelta = FMath::Sign(ClampedDelta) * MaxStep;
			}

			return ClampedDelta;
		};

	const float DeltaY = ApplyStrongSoftLimit(
		CurrentCameraOffset.Y,
		FinalYaw * CameraMoveSensitivity,
		CameraOffsetLimitX);

	const float DeltaZ = ApplyStrongSoftLimit(
		CurrentCameraOffset.Z,
		FinalPitch * CameraMoveSensitivity,
		CameraOffsetLimitZ);

	CurrentCameraOffset.Y = FMath::Clamp(CurrentCameraOffset.Y + DeltaY, -CameraOffsetLimitX, CameraOffsetLimitX);
	CurrentCameraOffset.Z = FMath::Clamp(CurrentCameraOffset.Z + DeltaZ, -CameraOffsetLimitZ, CameraOffsetLimitZ);

	if (CameraBoom)
	{
		CameraBoom->TargetOffset = CurrentCameraOffset;
	}
}

UAbilitySystemComponent* AThe_AwakeningCharacter::GetAbilitySystemComponent() const
{
	if (const ATAPlayerState* PS = GetPlayerState<ATAPlayerState>())
	{
		return PS->GetAbilitySystemComponent();
	}
	return nullptr;
}

void AThe_AwakeningCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
}

void AThe_AwakeningCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

void AThe_AwakeningCharacter::InitAbilityActorInfo()
{
	if (ATAPlayerState* PS = GetPlayerState<ATAPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
		{
			ASC->InitAbilityActorInfo(PS, this);
		}
	}
}

void AThe_AwakeningCharacter::TryInteract()
{
	if (CurrentInteractTarget.IsValid())
	{
		AActor* Target = CurrentInteractTarget.Get();
		if (Target && Target->Implements<UTAInteractable>())
		{
			if (ITAInteractable::Execute_CanInteract(Target, this))
			{
				ITAInteractable::Execute_OnInteract(Target, this);
			}
		}
		return;
	}

	//如果没有当前目标，再临时检测一次
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const bool bHit = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(InteractionDistance),
		Params
	);

	if (!bHit)
	{
		return;
	}

	AActor* ClosestInteractable = nullptr;
	float ClosestDistanceSq = TNumericLimits<float>::Max();

	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* Actor = Result.GetActor();
		if (!Actor || !Actor->Implements<UTAInteractable>())
		{
			continue;
		}

		if (!ITAInteractable::Execute_CanInteract(Actor, this))
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(GetActorLocation(), Actor->GetActorLocation());
		if (DistSq < ClosestDistanceSq)
		{
			ClosestDistanceSq = DistSq;
			ClosestInteractable = Actor;
		}
	}

	if (ClosestInteractable)
	{
		ITAInteractable::Execute_OnInteract(ClosestInteractable, this);
	}
}

void AThe_AwakeningCharacter::UpdateInteractTarget()
{
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const bool bHit = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(InteractionDistance),
		Params
	);

	AActor* NewTarget = nullptr;
	float ClosestDistSq = TNumericLimits<float>::Max();

	if (bHit)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* Actor = Result.GetActor();
			if (!Actor || !Actor->Implements<UTAInteractable>())
			{
				continue;
			}

			if (!ITAInteractable::Execute_CanInteract(Actor, this))
			{
				continue;
			}

			const float DistSq = FVector::DistSquared(GetActorLocation(), Actor->GetActorLocation());
			if (DistSq < ClosestDistSq)
			{
				ClosestDistSq = DistSq;
				NewTarget = Actor;
			}
		}
	}

	// 目标发生变化时更新提示
	if (CurrentInteractTarget.Get() != NewTarget)
	{
		if (ATAInteractableActor* OldTarget = Cast<ATAInteractableActor>(CurrentInteractTarget.Get()))
		{
			OldTarget->SetPromptVisible(false);
		}

		if (ATAInteractableActor* NewInteractable = Cast<ATAInteractableActor>(NewTarget))
		{
			NewInteractable->SetPromptVisible(true);
		}

		CurrentInteractTarget = NewTarget;
	}
}

bool AThe_AwakeningCharacter::IsSafeToMoveToward(const FVector& WorldDirection) const
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp || !GetWorld())
	{
		return true;
	}

	if (!MoveComp->IsMovingOnGround())
	{
		return true;
	}

	const FVector Dir = WorldDirection.GetSafeNormal2D();
	if (Dir.IsNearlyZero())
	{
		return true;
	}

	const float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector CapsuleLoc = GetActorLocation();
	const float CurrentFootZ = CapsuleLoc.Z - CapsuleHalfHeight;

	const FVector ProbeStart =
		CapsuleLoc +
		Dir * (CapsuleRadius + EdgeCheckForwardDistance) +
		FVector(0.f, 0.f, 20.f);

	const float ProbeDepth = CapsuleHalfHeight + FMath::Max(MaxSafeFallHeight, MoveComp->MaxStepHeight) + 40.f;
	const FVector ProbeEnd = ProbeStart - FVector(0.f, 0.f, ProbeDepth);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(EdgeCheck), false, this);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		ProbeStart,
		ProbeEnd,
		ECC_Visibility,
		Params);

	if (!bHit)
	{
		return false;
	}

	const float HeightDelta = Hit.ImpactPoint.Z - CurrentFootZ;

	if (HeightDelta >= -MoveComp->MaxStepHeight)
	{
		if (MoveComp->IsWalkable(Hit))
		{
			return true;
		}

		return false;
	}

	const float Drop = -HeightDelta;
	if (Drop <= MaxSafeFallHeight && MoveComp->IsWalkable(Hit))
	{
		return true;
	}

	return false;
}

void AThe_AwakeningCharacter::OnParkourJump(const FInputActionValue& Value)
{
	if (ParkourComponent)
	{
		ParkourComponent->TryParkourJump();
	}
}

void AThe_AwakeningCharacter::OnParkourDrop(const FInputActionValue& Value)
{
	if (ParkourComponent)
	{
		ParkourComponent->TryParkourDrop();
	}
}