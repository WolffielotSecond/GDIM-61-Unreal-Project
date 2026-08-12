#include "Core/TALocalizeSubsystem.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"

void UTALocalizeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 这里可以之后改成从 GameUserSettings 读取玩家保存的语言
	SetLanguage(DefaultLanguage);
}

bool UTALocalizeSubsystem::SetLanguage(const FString& LanguageCode)
{
	if (LoadLanguageFile(LanguageCode))
	{
		CurrentLanguage = LanguageCode;
		OnLanguageChanged.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("Localization language set to: %s"), *LanguageCode);
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Failed to load language: %s"), *LanguageCode);
	return false;
}

bool UTALocalizeSubsystem::LoadLanguageFile(const FString& LanguageCode)
{
	const FString FilePath = FPaths::ProjectContentDir() / LocalizationFolder / (LanguageCode + TEXT(".json"));

	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot load localization file: %s"), *FilePath);
		return false;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse localization JSON: %s"), *FilePath);
		return false;
	}

	TextMap.Empty();

	for (const auto& Pair : JsonObject->Values)
	{
		FString Value;
		if (Pair.Value->TryGetString(Value))
		{
			TextMap.Add(Pair.Key, Value);
		}
	}

	return true;
}

FText UTALocalizeSubsystem::GetText(const FString& TextId) const
{
	if (const FString* Found = TextMap.Find(TextId))
	{
		return FText::FromString(*Found);
	}

	// 找不到时返回 ID 本身
	return FText::FromString(TextId);
}

TArray<FString> UTALocalizeSubsystem::GetAvailableLanguages() const
{
	return { TEXT("zh-CN"), TEXT("zh-TW"), TEXT("en") };
}