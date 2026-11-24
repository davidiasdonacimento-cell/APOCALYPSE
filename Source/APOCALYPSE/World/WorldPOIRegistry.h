#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "World/ApocalypseWorldTypes.h"
#include "WorldPOIRegistry.generated.h"

USTRUCT(BlueprintType)
struct APOCALYPSE_API FApocalypsePOIEntry
{
    GENERATED_BODY()
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="POI") FName Id = NAME_None;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="POI") FText DisplayName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="POI") EApocalypsePOIType Type = EApocalypsePOIType::Landmark;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="POI") FVector Location = FVector::ZeroVector;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="POI") FRotator Rotation = FRotator::ZeroRotator;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="POI") FName RegionId = NAME_None;
};

UCLASS(BlueprintType)
class APOCALYPSE_API UWorldPOIRegistry : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Points of Interest")
    TArray<FApocalypsePOIEntry> PointsOfInterest;

    UFUNCTION(BlueprintPure, Category="Points of Interest")
    bool FindPOIById(FName Id, FApocalypsePOIEntry& OutEntry) const;
};
