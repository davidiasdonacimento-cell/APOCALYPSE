#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VerityMemorySystem.generated.h"

USTRUCT(BlueprintType)
struct FVerityMemoryEntry
{
    GENERATED_BODY()
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) FString Event;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) float WorldTimeSeconds = 0.0f;
};

UCLASS(ClassGroup=(VERITY), meta=(BlueprintSpawnableComponent))
class APOCALYPSE_API UVerityMemorySystem : public UActorComponent
{
    GENERATED_BODY()
public:
    UVerityMemorySystem();
    UFUNCTION(BlueprintCallable, Category="VERITY|Memory") void Remember(const FString& Event);
    UFUNCTION(BlueprintPure, Category="VERITY|Memory") FString BuildContext(int32 MaxEntries = 12) const;
    UFUNCTION(BlueprintCallable, Category="VERITY|Memory") void ClearSessionMemory();
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VERITY|Memory", meta=(ClampMin="8", ClampMax="64")) int32 MaxMemoryEntries = 32;
private:
    UPROPERTY(VisibleInstanceOnly, Category="VERITY|Memory") TArray<FVerityMemoryEntry> Entries;
};
