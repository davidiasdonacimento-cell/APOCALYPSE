#include "World/WorldRegionRegistry.h"

UWorldRegionDefinition* UWorldRegionRegistry::FindRegionAtLocation(const FVector& Location) const
{
    for (const TObjectPtr<UWorldRegionDefinition>& Region : Regions)
    {
        if (Region && Region->ContainsLocation(Location))
        {
            return Region.Get();
        }
    }
    return nullptr;
}
