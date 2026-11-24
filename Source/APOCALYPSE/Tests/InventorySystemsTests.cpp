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

    TestTrue(TEXT("Catalog contains starter items"), Catalog->Items.Num() >= 6);

    const FApocalypseItemDefinition* Food = Catalog->FindItem(TEXT("canned_food"));
    const FApocalypseItemDefinition* Water = Catalog->FindItem(TEXT("water_bottle"));
    const FApocalypseItemDefinition* Medical = Catalog->FindItem(TEXT("medical_kit"));
    const FApocalypseItemDefinition* Wood = Catalog->FindItem(TEXT("wood"));

    TestNotNull(TEXT("Food definition exists"), Food);
    TestNotNull(TEXT("Water definition exists"), Water);
    TestNotNull(TEXT("Medical definition exists"), Medical);
    TestNotNull(TEXT("Wood definition exists"), Wood);

    if (Food) TestTrue(TEXT("Food restores hunger"), Food->HungerRestoration > 0.0f);
    if (Water) TestTrue(TEXT("Water restores thirst"), Water->ThirstRestoration > 0.0f);
    if (Medical) TestTrue(TEXT("Medical kit restores health"), Medical->HealthRestoration > 0.0f);
    if (Wood) TestTrue(TEXT("Material has a positive stack limit"), Wood->MaxStack > 0);

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
