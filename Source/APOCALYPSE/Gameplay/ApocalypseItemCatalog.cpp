#include "Gameplay/ApocalypseItemCatalog.h"

UApocalypseItemCatalog::UApocalypseItemCatalog()
{
    auto AddDefaultItem = [this](FName Id, const TCHAR* Name, EApocalypseItemCategory Category,
        int32 Stack, float Hunger, float Thirst, float Health)
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

    auto AddEquipment = [this](FName Id, const TCHAR* Name, EApocalypseEquipmentSlot Slot)
    {
        FApocalypseItemDefinition Item;
        Item.ItemId = Id;
        Item.DisplayName = FText::FromString(Name);
        Item.Category = EApocalypseItemCategory::Equipment;
        Item.MaxStack = 1;
        Item.EquipmentSlot = Slot;
        Items.Add(Item);
    };
    AddEquipment(TEXT("cloth_cap"), TEXT("Boné de tecido"), EApocalypseEquipmentSlot::Head);
    AddEquipment(TEXT("reinforced_vest"), TEXT("Colete reforçado"), EApocalypseEquipmentSlot::Body);
    AddEquipment(TEXT("field_pack"), TEXT("Mochila de campo"), EApocalypseEquipmentSlot::Backpack);
    AddEquipment(TEXT("utility_belt"), TEXT("Cinto utilitário"), EApocalypseEquipmentSlot::Utility);
}

const FApocalypseItemDefinition* UApocalypseItemCatalog::FindItem(FName ItemId) const
{
    if (ItemId.IsNone()) return nullptr;
    return Items.FindByPredicate([ItemId](const FApocalypseItemDefinition& Item)
    {
        return Item.ItemId == ItemId;
    });
}
