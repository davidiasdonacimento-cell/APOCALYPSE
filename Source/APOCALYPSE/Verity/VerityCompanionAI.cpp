#include "Verity/VerityCompanionAI.h"
#include "Verity/VerityHostileTarget.h"
#include "Verity/VerityCombatSystem.h"
#include "Verity/VerityNavigationSystem.h"
#include "Verity/VerityMemorySystem.h"
#include "Verity/VerityDialogueSystem.h"
#include "Verity/VerityPersonalitySystem.h"
#include "Engine/World.h"

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
    AActor* Self = GetOwner();
    if (!Self || !Self->HasAuthority()) return;
    ScanCooldown -= DeltaTime;
    if (ScanCooldown > 0.0f) return;
    ScanCooldown = 0.7f;

    TArray<FOverlapResult> Overlaps;
    FCollisionObjectQueryParams ObjectTypes;
    ObjectTypes.AddObjectTypesToQuery(ECC_Pawn);
    ObjectTypes.AddObjectTypesToQuery(ECC_WorldDynamic);
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(VerityThreatScan), false, Self);
    const FCollisionShape Sphere = FCollisionShape::MakeSphere(FMath::Clamp(AwarenessRadius, 300.0f, 2500.0f));
    GetWorld()->OverlapMultiByObjectType(Overlaps, Self->GetActorLocation(), FQuat::Identity, ObjectTypes, Sphere, QueryParams);

    AActor* BestTarget = nullptr;
    float BestDistance = AwarenessRadius;
    for (const FOverlapResult& Overlap : Overlaps)
    {
        AActor* Candidate = Overlap.GetActor();
        if (!IsValid(Candidate) || Candidate == Self || Candidate == PlayerTarget.Get() ||
            !Candidate->GetClass()->ImplementsInterface(UVerityHostileTarget::StaticClass())) continue;
        if (!IVerityHostileTarget::Execute_IsValidVerityTarget(Candidate, Self)) continue;
        const float Distance = FVector::DistSquared(Self->GetActorLocation(), Candidate->GetActorLocation());
        if (Distance < FMath::Square(BestDistance)) { BestDistance = FMath::Sqrt(Distance); BestTarget = Candidate; }
    }

    if (CurrentThreat.Get() != BestTarget)
    {
        CurrentThreat = BestTarget;
        if (UVerityCombatSystem* Combat = Self->FindComponentByClass<UVerityCombatSystem>()) Combat->SetTarget(BestTarget);
        if (UVerityNavigationSystem* Navigation = Self->FindComponentByClass<UVerityNavigationSystem>()) Navigation->SetCombatTarget(BestTarget);
        if (UVerityMemorySystem* Memory = Self->FindComponentByClass<UVerityMemorySystem>())
            Memory->Remember(BestTarget ? TEXT("Uma ameaça reconhecida nas proximidades.") : TEXT("A área próxima voltou a ficar tranquila."));
        if (UVerityPersonalitySystem* Personality = Self->FindComponentByClass<UVerityPersonalitySystem>())
            Personality->SetMood(BestTarget ? EVerityMood::Alert : EVerityMood::Relieved);
        if (UVerityDialogueSystem* Dialogue = Self->FindComponentByClass<UVerityDialogueSystem>())
            Dialogue->SetCombatActive(BestTarget != nullptr);
    }
}
