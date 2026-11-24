#include "Gameplay/ApocalypseItemCatalog.h"
UApocalypseItemCatalog::UApocalypseItemCatalog()
{
    auto AddDefaultItem = [this](FName Id, const TCHAR* Name, EApocalypseItemCategory Category, int32 Stack,
        float Hunger, float Thirst, float Health)
    {
        FApocalypseItemDefinition Item;
        Item.ItemId = Id;
        Item.DisplayName = FText::FromString(Name);
        Item.Category = Category;
        Item.MaxStack = Stack;
        Item.HungerRestoration = Hunger;
        Item.ThirstRestoration = Thirst;
        Item.HealthRestoration = Health;
        Items.Add(Item);
    };
    AddDefaultItem(TEXT("canned_food"), TEXT("Comida enlatada"), EApocalypseItemCategory::Food, 10, 30.0f, 0.0f, 0.0f);
    AddDefaultItem(TEXT("water_bottle"), TEXT("Garrafa de água"), EApocalypseItemCategory::Water, 10, 0.0f, 35.0f, 0.0f);
    AddDefaultItem(TEXT("medical_kit"), TEXT("Kit médico"), EApocalypseItemCategory::Medical, 5, 0.0f, 0.0f, 35.0f);
    AddDefaultItem(TEXT("wood"), TEXT("Madeira"), EApocalypseItemCategory::Material, 50, 0.0f, 0.0f, 0.0f);
    AddDefaultItem(TEXT("stone"), TEXT("Pedra"), EApocalypseItemCategory::Material, 50, 0.0f, 0.0f, 0.0f);
    AddDefaultItem(TEXT("cloth"), TEXT("Tecido"), EApocalypseItemCategory::Material, 50, 0.0f, 0.0f, 0.0f);
}
const FApocalypseItemDefinition* UApocalypseItemCatalog::FindItem(FName ItemId) const
{
    if (ItemId.IsNone()) return nullptr;
    return Items.FindByPredicate([ItemId](const FApocalypseItemDefinition& Item) { return Item.ItemId == ItemId; });
}