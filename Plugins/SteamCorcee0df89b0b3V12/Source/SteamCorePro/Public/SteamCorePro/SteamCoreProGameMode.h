/**
* Copyright (C) 2017-2025 eelDev AB
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SteamCoreProGameMode.generated.h"

UCLASS()
class STEAMCOREPRO_API ASteamCoreProGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SteamCore")
	bool bDisableSeamlessTravelPersistentActors;
	
public:
	virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
};
