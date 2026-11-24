#include "Verity/VerityNavigationSystem.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UVerityNavigationSystem::UVerityNavigationSystem()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.25f;
}

void UVerityNavigationSystem::BeginPlay()
{
    Super::BeginPlay();
}

void UVerityNavigationSystem::SetFollowTarget(AActor* Target) { FollowTarget = Target; }
void UVerityNavigationSystem::SetCombatTarget(AActor* Target) { CombatTarget = Target; }
void UVerityNavigationSystem::SetFollowingEnabled(bool bEnabled)
{
    bFollowingEnabled = bEnabled;
    if (!bEnabled)
        if (AAIController* Controller = Cast<AAIController>(GetOwner() ? GetOwner()->GetInstigatorController() : nullptr))
            Controller->StopMovement();
}

void UVerityNavigationSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    MoveRequestCooldown -= DeltaTime;
    if (MoveRequestCooldown > 0.0f || !GetOwner() || !GetOwner()->HasAuthority()) return;

    AAIController* Controller = Cast<AAIController>(GetOwner()->GetInstigatorController());
    if (!Controller) Controller = Cast<AAIController>(GetOwner()->GetOwner());
    if (!Controller) return;

    AActor* Target = CombatTarget.Get();
    float Acceptance = CombatDistance;
    if (!IsValid(Target))
    {
        Target = bFollowingEnabled ? FollowTarget.Get() : nullptr;
        Acceptance = FollowDistance;
    }
    if (!IsValid(Target)) { Controller->StopMovement(); return; }

    const float Distance = FVector::Dist2D(GetOwner()->GetActorLocation(), Target->GetActorLocation());
    if (Distance <= Acceptance) { Controller->StopMovement(); return; }

    Controller->MoveToActor(Target, Acceptance, true, true, true, nullptr, true);
    MoveRequestCooldown = 0.8f;
}
