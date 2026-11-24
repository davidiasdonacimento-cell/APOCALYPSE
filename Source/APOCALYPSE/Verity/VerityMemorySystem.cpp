#include "Verity/VerityMemorySystem.h"

UVerityMemorySystem::UVerityMemorySystem()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UVerityMemorySystem::Remember(const FString& Event)
{
    const FString Clean = Event.TrimStartAndEnd();
    if (Clean.IsEmpty()) return;
    FVerityMemoryEntry Entry;
    Entry.Event = Clean.Left(240);
    Entry.WorldTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    Entries.Add(MoveTemp(Entry));
    const int32 Limit = FMath::Clamp(MaxMemoryEntries, 8, 64);
    while (Entries.Num() > Limit) Entries.RemoveAt(0, 1, EAllowShrinking::No);
}

FString UVerityMemorySystem::BuildContext(int32 MaxEntries) const
{
    const int32 Start = FMath::Max(0, Entries.Num() - FMath::Clamp(MaxEntries, 1, 32));
    FString Result;
    for (int32 Index = Start; Index < Entries.Num(); ++Index)
    {
        Result += FString::Printf(TEXT("- [%.0fs] %s\n"), Entries[Index].WorldTimeSeconds, *Entries[Index].Event);
    }
    return Result;
}

void UVerityMemorySystem::ClearSessionMemory()
{
    Entries.Reset();
}
