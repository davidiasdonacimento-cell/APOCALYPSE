#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "World/WorldRegionDefinition.h"
#include "World/WorldPOIRegistry.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FApocalypseWorldRegionBoundsTest,
    "APOCALYPSE.World.RegionBounds",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FApocalypseWorldRegionBoundsTest::RunTest(const FString& Parameters)
{
    UWorldRegionDefinition* Region = NewObject<UWorldRegionDefinition>();
    Region->WorldBounds = FBox(FVector(-100.0f), FVector(100.0f));

    TestTrue(TEXT("Point inside region is accepted"), Region->ContainsLocation(FVector::ZeroVector));
    TestTrue(TEXT("Point on region boundary is accepted"), Region->ContainsLocation(FVector(100.0f, 0.0f, 0.0f)));
    TestFalse(TEXT("Point outside region is rejected"), Region->ContainsLocation(FVector(101.0f, 0.0f, 0.0f)));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FApocalypsePOILookupTest,
    "APOCALYPSE.World.POILookup",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FApocalypsePOILookupTest::RunTest(const FString& Parameters)
{
    UWorldPOIRegistry* Registry = NewObject<UWorldPOIRegistry>();
    FApocalypsePOIEntry Entry;
    Entry.Id = TEXT("hospital_north");
    Entry.DisplayName = FText::FromString(TEXT("North Hospital"));
    Registry->PointsOfInterest.Add(Entry);

    FApocalypsePOIEntry Found;
    TestTrue(TEXT("Known POI ID resolves"), Registry->FindPOIById(TEXT("hospital_north"), Found));
    TestEqual(TEXT("Resolved POI preserves its ID"), Found.Id, FName(TEXT("hospital_north")));
    TestFalse(TEXT("Unknown POI ID does not resolve"), Registry->FindPOIById(TEXT("missing"), Found));
    return true;
}

#endif
