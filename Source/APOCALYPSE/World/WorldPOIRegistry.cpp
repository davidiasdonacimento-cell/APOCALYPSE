#include "World/WorldPOIRegistry.h"

bool UWorldPOIRegistry::FindPOIById(FName Id, FApocalypsePOIEntry& OutEntry) const
{
    if (Id.IsNone()) return false;
    const FApocalypsePOIEntry* Match = PointsOfInterest.FindByPredicate(
        [Id](const FApocalypsePOIEntry& Entry) { return Entry.Id == Id; });
    if (!Match) return false;
    OutEntry = *Match;
    return true;
}
