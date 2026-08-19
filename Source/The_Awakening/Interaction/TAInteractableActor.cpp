#include "Interaction/TAInteractableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UI/TAPromptWidgetUtils.h"

ATAInteractableActor::ATAInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	InteractPromptComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractPromptComponent"));
	InteractPromptComponent->SetupAttachment(RootComponent);
	InteractPromptComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	InteractPromptComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InteractPromptComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractPromptComponent->SetDrawAtDesiredSize(true);
	InteractPromptComponent->SetVisibility(false);
}

void ATAInteractableActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATAInteractableActor::OnInteract_Implementation(AActor* Interactor)
{
	if (Interactor)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s 与 %s 发生了交互"), *Interactor->GetName(), *GetName());
	}
}

bool ATAInteractableActor::CanInteract_Implementation(AActor* Interactor) const
{
	return true;
}

FText ATAInteractableActor::GetInteractText_Implementation() const
{
	// 保留接口
	if (!InteractText.IsEmpty())
	{
		return InteractText;
	}
	return FText::FromString(InteractTextId);
}

FString ATAInteractableActor::GetPromptTextId() const
{
	if (!InteractTextId.IsEmpty())
	{
		return InteractTextId;
	}
	return InteractText.ToString();
}

void ATAInteractableActor::SetPromptVisible(bool bVisible)
{
	if (InteractPromptComponent)
	{
		InteractPromptComponent->SetVisibility(bVisible);
	}
}

void ATAInteractableActor::RefreshPrompt(UTexture2D* KeyIcon, const FText& PromptText)
{
	if (!InteractPromptComponent)
	{
		return;
	}

	UUserWidget* Widget = InteractPromptComponent->GetUserWidgetObject();
	if (!Widget)
	{
		return;
	}
	FTAPromptWidgetUtils::ApplyPrompt(Widget, KeyIcon, PromptText, 40.f);
}