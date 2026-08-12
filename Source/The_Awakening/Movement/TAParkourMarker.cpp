#include "Movement/TAParkourMarker.h"
#include "Movement/TAParkourComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ChildActorComponent.h"
#include "Engine/TargetPoint.h"

ATAParkourMarker::ATAParkourMarker()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetBoxExtent(FVector(80.f, 80.f, 60.f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);

	LandingTargetComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("LandingTargetComponent"));
	LandingTargetComponent->SetupAttachment(RootComponent);
	LandingTargetComponent->SetRelativeLocation(FVector(300.f, 0.f, 0.f));
	LandingTargetComponent->SetChildActorClass(ATargetPoint::StaticClass());
}

void ATAParkourMarker::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	BindLandingTargetFromChild();
}

void ATAParkourMarker::BeginPlay()
{
	Super::BeginPlay();

	BindLandingTargetFromChild();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATAParkourMarker::OnBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ATAParkourMarker::OnEndOverlap);
}

void ATAParkourMarker::BindLandingTargetFromChild()
{
	if (LandingTargetComponent)
	{
		if (AActor* Child = LandingTargetComponent->GetChildActor())
		{
			LandingTarget = Child;
		}
	}
}

bool ATAParkourMarker::IsValidMarker() const
{
	return LandingTarget != nullptr;
}

FVector ATAParkourMarker::GetLandingLocation() const
{
	return LandingTarget ? LandingTarget->GetActorLocation() : GetActorLocation();
}

void ATAParkourMarker::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	if (UTAParkourComponent* ParkourComp = OtherActor->FindComponentByClass<UTAParkourComponent>())
	{
		ParkourComp->RegisterMarker(this);
	}
}

void ATAParkourMarker::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	if (UTAParkourComponent* ParkourComp = OtherActor->FindComponentByClass<UTAParkourComponent>())
	{
		ParkourComp->UnregisterMarker(this);
	}
}