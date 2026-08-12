#include "Movement/TAParkourComponent.h"
#include "Movement/TAParkourMarker.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

UTAParkourComponent::UTAParkourComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTAParkourComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UTAParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsParkouring || !OwnerCharacter.IsValid())
	{
		return;
	}

	ParkourTime += DeltaTime;
	const float Alpha = FMath::Clamp(ParkourTime / ParkourDuration, 0.f, 1.f);

	const FVector NewLoc = EvalParabola(ParkourStart, ParkourEnd, ParkourArcHeight, Alpha);
	OwnerCharacter->SetActorLocation(NewLoc, false, nullptr, ETeleportType::None);

	if (HasLanded())
	{
		FinishParkour();
		return;
	}

	if (Alpha >= 1.f)
	{
		FinishParkour();
	}
}

FVector UTAParkourComponent::EvalParabola(const FVector& Start, const FVector& End, float ArcHeight, float Alpha)
{
	const FVector Linear = FMath::Lerp(Start, End, Alpha);
	const float HeightOffset = 4.f * ArcHeight * Alpha * (1.f - Alpha); // 顶点在 Alpha=0.5
	return Linear + FVector(0.f, 0.f, HeightOffset);
}

void UTAParkourComponent::TryParkourJump()
{
	if (bIsParkouring || !CurrentMarker)
	{
		return;
	}
	if (CurrentMarker->MarkerType != ETAParkourMarkerType::JumpToPoint)
	{
		return;
	}
	StartParkour(CurrentMarker);
}

void UTAParkourComponent::TryParkourDrop()
{
	if (bIsParkouring || !CurrentMarker)
	{
		return;
	}
	if (CurrentMarker->MarkerType != ETAParkourMarkerType::DropDown)
	{
		return;
	}
	StartParkour(CurrentMarker);
}

bool UTAParkourComponent::StartParkour(ATAParkourMarker* Marker)
{
	if (!Marker || !Marker->IsValidMarker() || !OwnerCharacter.IsValid())
	{
		return false;
	}

	ParkourStart = OwnerCharacter->GetActorLocation();
	ParkourEnd = Marker->GetLandingLocation();
	ParkourArcHeight = Marker->ArcHeight;
	ParkourDuration = FMath::Max(Marker->JumpDuration, 0.05f);
	ParkourTime = 0.f;
	bIsParkouring = true;

	if (UCharacterMovementComponent* Move = OwnerCharacter->GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->SetMovementMode(MOVE_Flying); // 期间不受重力干扰
	}

	return true;
}

void UTAParkourComponent::FinishParkour()
{
	bIsParkouring = false;

	if (!OwnerCharacter.IsValid())
	{
		return;
	}

	if (UCharacterMovementComponent* Move = OwnerCharacter->GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->Velocity = FVector::ZeroVector;
		Move->SetMovementMode(MOVE_Walking);
	}
}

void UTAParkourComponent::RegisterMarker(ATAParkourMarker* Marker)
{
	if (!Marker)
	{
		return;
	}
	OverlappingMarkers.AddUnique(Marker);
	CurrentMarker = Marker; // 多个重叠时取最后进入的
}

void UTAParkourComponent::UnregisterMarker(ATAParkourMarker* Marker)
{
	OverlappingMarkers.Remove(Marker);
	if (CurrentMarker == Marker)
	{
		CurrentMarker = OverlappingMarkers.Num() > 0 ? OverlappingMarkers.Last() : nullptr;
	}
}

bool UTAParkourComponent::HasLanded() const
{
	if (!OwnerCharacter.IsValid())
	{
		return false;
	}

	// 起跳后短暂忽略
	if (ParkourTime < 0.08f)
	{
		return false;
	}

	UCharacterMovementComponent* Move = OwnerCharacter->GetCharacterMovement();
	if (!Move)
	{
		return false;
	}

	FFindFloorResult FloorResult;
	Move->FindFloor(OwnerCharacter->GetActorLocation(), FloorResult, false);

	if (!FloorResult.IsWalkableFloor())
	{
		return false;
	}

	const float FloorDist = FloorResult.FloorDist;
	return FloorDist <= Move->MAX_FLOOR_DIST + 5.f;
}