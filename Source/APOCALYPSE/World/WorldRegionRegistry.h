#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "World/WorldRegionDefinition.h"
#include "WorldRegionRegistry.generated.h"

UCLASS(BlueprintType)
class APOCALYPSE_API UWorldRegionRegistry : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Regions")
    TArray<TObjectPtr<UWorldRegionDefinition>> Regions;

    UFUNCTION(BlueprintPure, Category="Regions")
    UWorldRegionDefinition* FindRegionAtLocation(const FVector& Location) const;
};
