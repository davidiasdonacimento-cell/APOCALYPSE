#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Gameplay/ApocalypseItemCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FApocalypseDefaultItemCatalogTest,
    "APOCALYPSE.Inventory.DefaultCatalog",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FApocalypseDefaultItemCatalogTest::RunTest(const FString& Parameters)
{
    const UApocalypseItemCatalog* Catalog = GetDefault<UApocalypseItemCatalog>();
    TestNotNull(TEXT("Built-in item catalog exists"), Catalog);
    if (!Catalog) return false;

    TestTrue(TEXT("Catalog contains starter supplies and equipment"), Catalog->Items.Num() >= 10);

    const FApocalypseItemDefinition* Food = Catalog->FindItem(TEXT("canned_food"));
    const FApocalypseItemDefinition* Water = Catalog->FindItem(TEXT("water_bottle"));
    const FApocalypseItemDefinition* Medical = Catalog->FindItem(TEXT("medical_kit"));
    const FApocalypseItemDefinition* Wood = Catalog->FindItem(TEXT("wood"));
    const FApocalypseItemDefinition* HeadGear = Catalog->FindItem(TEXT("cloth_cap"));
    const FApocalypseItemDefinition* BodyGear = Catalog->FindItem(TEXT("reinforced_vest"));
    const FApocalypseItemDefinition* Backpack = Catalog->FindItem(TEXT("field_pack"));
    const FApocalypseItemDefinition* Utility = Catalog->FindItem(TEXT("utility_belt"));

    TestNotNull(TEXT("Food definition exists"), Food);
    TestNotNull(TEXT("Water definition exists"), Water);
    TestNotNull(TEXT("Medical definition exists"), Medical);
    TestNotNull(TEXT("Wood definition exists"), Wood);
    TestNotNull(TEXT("Head equipment definition exists"), HeadGear);
    TestNotNull(TEXT("Body equipment definition exists"), BodyGear);
    TestNotNull(TEXT("Backpack equipment definition exists"), Backpack);
    TestNotNull(TEXT("Utility equipment definition exists"), Utility);

    if (Food) TestTrue(TEXT("Food restores hunger"), Food->HungerRestoration > 0.0f);
    if (Water) TestTrue(TEXT("Water restores thirst"), Water->ThirstRestoration > 0.0f);
    if (Medical) TestTrue(TEXT("Medical kit restores health"), Medical->HealthRestoration > 0.0f);
    if (Wood) TestTrue(TEXT("Material has a positive stack limit"), Wood->MaxStack > 0);
    if (HeadGear) TestTrue(TEXT("Head item uses head slot"), HeadGear->Category == EApocalypseItemCategory::Equipment && HeadGear->EquipmentSlot == EApocalypseEquipmentSlot::Head);
    if (BodyGear) TestTrue(TEXT("Vest uses body slot"), BodyGear->EquipmentSlot == EApocalypseEquipmentSlot::Body);
    if (Backpack) TestTrue(TEXT("Pack uses backpack slot"), Backpack->EquipmentSlot == EApocalypseEquipmentSlot::Backpack);
    if (Utility) TestTrue(TEXT("Belt uses utility slot"), Utility->EquipmentSlot == EApocalypseEquipmentSlot::Utility);

    TSet<FName> UniqueIds;
    for (const FApocalypseItemDefinition& Item : Catalog->Items)
    {
        TestFalse(TEXT("Item ID is not empty"), Item.ItemId.IsNone());
        TestTrue(TEXT("Item stack limit is valid"), Item.MaxStack >= 1 && Item.MaxStack <= 999);
        TestFalse(TEXT("Item IDs are unique"), UniqueIds.Contains(Item.ItemId));
        UniqueIds.Add(Item.ItemId);
    }
    return true;
}
#endif
