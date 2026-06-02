/**
* Copyright (C) 2017-2025 eelDev AB
*/

#include "SteamCoreProGameMode.h"
#include <GameFramework/GameSession.h>

void ASteamCoreProGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	if (bDisableSeamlessTravelPersistentActors)
	{
		// do nothing
	}
	else
	{
		Super::GetSeamlessTravelActorList(bToTransition, ActorList);
	}
}

void ASteamCoreProGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	ErrorMessage = GameSession->ApproveLogin(Options);
	FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage);
}
