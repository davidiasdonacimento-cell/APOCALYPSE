#include "Gameplay/ApocalypseItemCatalog.h"
bool UApocalypseItemCatalog::FindItem(FName ItemId, FApocalypseItemDefinition& OutDefinition) const
{
    if (ItemId.IsNone()) return false;
    const FApocalypseItemDefinition* Found = Items.FindByPredicate(
        [ItemId](const FApocalypseItemDefinition& Item) { return Item.ItemId == ItemId; });
    if (!Found) return false;
    OutDefinition = *Found;
    return true;
}