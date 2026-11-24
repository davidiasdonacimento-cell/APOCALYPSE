#include "Gameplay/ApocalypseItemCatalog.h"
const FApocalypseItemDefinition* UApocalypseItemCatalog::FindItem(FName ItemId) const
{
    if (ItemId.IsNone()) return nullptr;
    return Items.FindByPredicate([ItemId](const FApocalypseItemDefinition& Item) { return Item.ItemId == ItemId; });
}