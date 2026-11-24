#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Verity/VerityMemorySystem.h"
#include "Verity/VerityPersonalitySystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FVeritySessionMemoryTest,
    "APOCALYPSE.Verity.SessionMemory",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVeritySessionMemoryTest::RunTest(const FString& Parameters)
{
    UVerityMemorySystem* Memory = NewObject<UVerityMemorySystem>();
    TestNotNull(TEXT("Memory component can be created"), Memory);
    if (!Memory) return false;
    Memory->Remember(TEXT("Jogador entrou na floresta."));
    Memory->Remember(TEXT("VERITY identificou movimento próximo."));
    const FString Context = Memory->BuildContext(8);
    TestTrue(TEXT("Recent world event is retained"), Context.Contains(TEXT("floresta")));
    TestTrue(TEXT("Companion event is retained"), Context.Contains(TEXT("movimento próximo")));
    Memory->ClearSessionMemory();
    TestTrue(TEXT("Session memory can be cleared"), Memory->BuildContext().IsEmpty());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FVerityPersonalityTest,
    "APOCALYPSE.Verity.Personality",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVerityPersonalityTest::RunTest(const FString& Parameters)
{
    UVerityPersonalitySystem* Personality = NewObject<UVerityPersonalitySystem>();
    TestNotNull(TEXT("Personality component can be created"), Personality);
    if (!Personality) return false;
    const FString Prompt = Personality->BuildSystemPrompt();
    TestTrue(TEXT("Personality prompt sets Brazilian Portuguese"), Prompt.Contains(TEXT("português brasileiro")));
    TestTrue(TEXT("Personality prompt prevents invented game facts"), Prompt.Contains(TEXT("nunca invente")));
    const FString OfflineLine = Personality->GetOfflineLine(EVerityMood::Friendly);
    TestFalse(TEXT("Offline companion line is available"), OfflineLine.IsEmpty());
    return true;
}
#endif
