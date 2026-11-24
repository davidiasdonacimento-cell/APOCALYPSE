#include "Verity/VerityCombatSystem.h"
#include "Verity/VerityHostileTarget.h"

UVerityCombatSystem::UVerityCombatSystem()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.1f;
}
void UVerityCombatSystem::BeginPlay() { Super::BeginPlay(); }
void UVerityCombatSystem::SetTarget(AActor* NewTarget) { Target = NewTarget; }
void UVerityCombatSystem::SetCombatEnabled(bool bEnabled)
{
    bCombatEnabled = bEnabled;
    if (!bEnabled) Target.Reset();
}
void UVerityCombatSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!bCombatEnabled) return;
    AttackCooldown -= DeltaTime;
    AActor* Victim = Target.Get();
    AActor* Self = GetOwner();
    if (!Self || !Self->HasAuthority() || !IsValid(Victim) ||
        !Victim->GetClass()->ImplementsInterface(UVerityHostileTarget::StaticClass())) return;
    const float Distance = FVector::Dist(Self->GetActorLocation(), Victim->GetActorLocation());
    if (Distance > AttackRange || AttackCooldown > 0.0f) return;
    if (!IVerityHostileTarget::Execute_IsValidVerityTarget(Victim, Self))
    {
        Target.Reset();
        return;
    }
    IVerityHostileTarget::Execute_ReceiveVerityDamage(Victim, FictionalDamagePerHit, Self);
    AttackCooldown = FMath::Max(0.3f, AttackInterval);
}
