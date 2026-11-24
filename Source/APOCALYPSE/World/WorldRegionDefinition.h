#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "World/ApocalypseWorldTypes.h"
#include "WorldRegionDefinition.generated.h"

UCLASS(BlueprintType)
class APOCALYPSE_API UWorldRegionDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Region") FName RegionId = NAME_None;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Region") FText DisplayName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Region") EApocalypseBiome Biome = EApocalypseBiome::TemperateForest;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Region") FBox WorldBounds = FBox(ForceInit);
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Environment", meta=(ClampMin="-30.0", ClampMax="55.0")) float BaseTemperatureCelsius = 18.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Environment", meta=(ClampMin="0.0", ClampMax="1.0")) float BaseWindStrength = 0.15f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Environment", meta=(ClampMin="0.0", ClampMax="0.2")) float BaseFogDensity = 0.015f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weather") FApocalypseWeatherWeights WeatherWeights;

    UFUNCTION(BlueprintPure, Category="Region")
    bool ContainsLocation(const FVector& Location) const;
};
