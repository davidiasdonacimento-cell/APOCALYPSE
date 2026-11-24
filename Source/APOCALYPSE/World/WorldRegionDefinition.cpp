#include "World/WorldRegionDefinition.h"

bool UWorldRegionDefinition::ContainsLocation(const FVector& Location) const
{
    return WorldBounds.IsValid && WorldBounds.IsInsideOrOn(Location);
}
