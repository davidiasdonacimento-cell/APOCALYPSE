#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VerityPersonalitySystem.generated.h"

UENUM(BlueprintType)
enum class EVerityMood : uint8
{
    Friendly,
    Alert,
    Focused,
    Relieved,
    Curious
};

UCLASS(ClassGroup=(VERITY), meta=(BlueprintSpawnableComponent))
class APOCALYPSE_API UVerityPersonalitySystem : public UActorComponent
{
    GENERATED_BODY()
public:
    UVerityPersonalitySystem();
    UFUNCTION(BlueprintPure, Category="VERITY|Personality") FString BuildSystemPrompt() const;
    UFUNCTION(BlueprintCallable, Category="VERITY|Personality") FString GetOfflineLine(EVerityMood Mood);
    UFUNCTION(BlueprintCallable, Category="VERITY|Personality") void SetMood(EVerityMood NewMood) { Mood = NewMood; }
    UFUNCTION(BlueprintPure, Category="VERITY|Personality") EVerityMood GetMood() const { return Mood; }
private:
    UPROPERTY(VisibleInstanceOnly, Category="VERITY|Personality") EVerityMood Mood = EVerityMood::Friendly;
    TArray<int32> RecentOfflineLineIndices;
};
