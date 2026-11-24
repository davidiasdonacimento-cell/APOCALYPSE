#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "VerityProfileSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FVeritySavedChatMessage
{
    GENERATED_BODY()
    UPROPERTY(SaveGame, BlueprintReadOnly) FString Role;
    UPROPERTY(SaveGame, BlueprintReadOnly) FString Content;
    UPROPERTY(SaveGame, BlueprintReadOnly) FDateTime Timestamp;
};

UCLASS()
class APOCALYPSE_API UVerityProfileSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(SaveGame, BlueprintReadOnly, Category="VERITY|Profile") FString PersonalContext;
    UPROPERTY(SaveGame, BlueprintReadOnly, Category="VERITY|Profile") TArray<FVeritySavedChatMessage> ChatHistory;
    UPROPERTY(SaveGame, BlueprintReadOnly, Category="VERITY|Profile") TArray<FString> PersistentMemories;
    UPROPERTY(SaveGame, BlueprintReadOnly, Category="VERITY|Profile") bool bHasCampaignProgress = false;
    UPROPERTY(SaveGame, BlueprintReadOnly, Category="VERITY|Profile") int32 SchemaVersion = 1;
};