#include "Verity/VerityCompanionAI.h"
#include "Verity/VerityHostileTarget.h"
#include "Verity/VerityCombatSystem.h"
#include "Verity/VerityNavigationSystem.h"
#include "Verity/VerityMemorySystem.h"
#include "Verity/VerityDialogueSystem.h"
#include "Verity/VerityPersonalitySystem.h"
#include "EngineUtils.h"

UVerityCompanionAI::UVerityCompanionAI()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.5f;
}

void UVerityCompanionAI::BeginPlay() { Super::BeginPlay(); }
void UVerityCompanionAI::SetPlayerTarget(AActor* Player) { PlayerTarget = Player; }

void UVerityCompanionAI::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!GetOwner() || !GetOwner()->HasAuthority()) return;
    ScanCooldown -= DeltaTime;
    if (ScanCooldown > 0.0f) return;
    ScanCooldown = 0.65f;

    AActor* BestTarget = nullptr;
    float BestDistance = AwarenessRadius;
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        AActor* Candidate = *It;
        if (!IsValid(Candidate) || Candidate == GetOwner() || Candidate == PlayerTarget.Get() ||
            !Candidate->GetClass()->ImplementsInterface(UVerityHostileTarget::StaticClass())) continue;
        if (!IVerityHostileTarget::Execute_IsValidVerityTarget(Candidate, GetOwner())) continue;
        const float Distance = FVector::Dist(GetOwner()->GetActorLocation(), Candidate->GetActorLocation());
        if (Distance < BestDistance) { BestDistance = Distance; BestTarget = Candidate; }
    }

    if (CurrentThreat.Get() != BestTarget)
    {
        CurrentThreat = BestTarget;
        if (UVerityCombatSystem* Combat = GetOwner()->FindComponentByClass<UVerityCombatSystem>())
            Combat->SetTarget(BestTarget);
        if (UVerityNavigationSystem* Navigation = GetOwner()->FindComponentByClass<UVerityNavigationSystem>())
            Navigation->SetCombatTarget(BestTarget);
        if (UVerityMemorySystem* Memory = GetOwner()->FindComponentByClass<UVerityMemorySystem>())
            Memory->Remember(BestTarget ? TEXT("Uma ameaça reconhecida nas proximidades.") : TEXT("A área próxima voltou a ficar tranquila."));
        if (UVerityPersonalitySystem* Personality = GetOwner()->FindComponentByClass<UVerityPersonalitySystem>())
            Personality->SetMood(BestTarget ? EVerityMood::Alert : EVerityMood::Relieved);
        if (UVerityDialogueSystem* Dialogue = GetOwner()->FindComponentByClass<UVerityDialogueSystem>())
            Dialogue->SetCombatActive(BestTarget != nullptr);
    }
}
