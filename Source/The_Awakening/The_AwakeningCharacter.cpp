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

AThe_AwakeningCharacter::AThe_AwakeningCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 500.f;
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
}

void AThe_AwakeningCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThe_AwakeningCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AThe_AwakeningCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThe_AwakeningCharacter::Look);

		// Interact
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AThe_AwakeningCharacter::TryInteract);
		}
	}
	else
	{
		UE_LOG(LogThe_Awakening, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void AThe_AwakeningCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AThe_AwakeningCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AThe_AwakeningCharacter::DoMove(float Right, float Forward)
{
	if (GetController() == nullptr)
	{
		return;
	}

	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, Forward);
	AddMovementInput(RightDirection, Right);
}

void AThe_AwakeningCharacter::DoLook(float Yaw, float Pitch)
{
	float FinalYaw = bInvertCameraX ? -Yaw : Yaw;
	float FinalPitch = bInvertCameraY ? Pitch : -Pitch;

	CurrentCameraOffset.Y += FinalYaw * CameraMoveSensitivity;
	CurrentCameraOffset.Z += FinalPitch * CameraMoveSensitivity;

	CurrentCameraOffset.Y = FMath::Clamp(CurrentCameraOffset.Y, -CameraOffsetLimitX, CameraOffsetLimitX);
	CurrentCameraOffset.Z = FMath::Clamp(CurrentCameraOffset.Z, -CameraOffsetLimitZ, CameraOffsetLimitZ);

	if (CameraBoom)
	{
		CameraBoom->TargetOffset = CurrentCameraOffset;
	}
}

void AThe_AwakeningCharacter::DoJumpStart()
{
	Jump();
}

void AThe_AwakeningCharacter::DoJumpEnd()
{
	StopJumping();
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