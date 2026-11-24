#include "Gameplay/ApocalypseSurvivalComponent.h"
#include "Net/UnrealNetwork.h"

UApocalypseSurvivalComponent::UApocalypseSurvivalComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.25f;
    SetIsReplicatedByDefault(true);
}

void UApocalypseSurvivalComponent::BeginPlay()
{
    Super::BeginPlay();
    if (GetOwner() && GetOwner()->HasAuthority())
    {
        Health = FMath::Clamp(Health, 0.0f, MaxHealth);
        Hunger = FMath::Clamp(Hunger, 0.0f, MaxHunger);
        Thirst = FMath::Clamp(Thirst, 0.0f, MaxThirst);
        Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
    }
}

void UApocalypseSurvivalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!GetOwner() || !GetOwner()->HasAuthority() || !IsAlive())
    {
        return;
    }

    SimulationAccumulator += DeltaTime;
    while (SimulationAccumulator >= 1.0f)
    {
        SimulationAccumulator -= 1.0f;
        SimulateOneSecond();
    }
}

void UApocalypseSurvivalComponent::SimulateOneSecond()
{
    const float PreviousHunger = Hunger;
    const float PreviousThirst = Thirst;
    const float PreviousStamina = Stamina;
    const float PreviousHealth = Health;

    Hunger = FMath::Max(0.0f, Hunger - HungerDrainPerMinute / 60.0f);
    Thirst = FMath::Max(0.0f, Thirst - ThirstDrainPerMinute / 60.0f);
    Stamina = FMath::Min(MaxStamina, Stamina + StaminaRecoveryPerSecond);

    if (Hunger <= 0.0f)
    {
        Health = FMath::Max(0.0f, Health - StarvationDamagePerSecond);
    }
    if (Thirst <= 0.0f)
    {
        Health = FMath::Max(0.0f, Health - DehydrationDamagePerSecond);
    }

    if (!FMath::IsNearlyEqual(PreviousHunger, Hunger) ||
        !FMath::IsNearlyEqual(PreviousThirst, Thirst) ||
        !FMath::IsNearlyEqual(PreviousStamina, Stamina) ||
        !FMath::IsNearlyEqual(PreviousHealth, Health))
    {
        BroadcastChanged();
    }
}

bool UApocalypseSurvivalComponent::ConsumeFood(float Amount)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || Amount <= 0.0f || !IsAlive())
    {
        return false;
    }
    const float Previous = Hunger;
    Hunger = FMath::Min(MaxHunger, Hunger + Amount);
    if (FMath::IsNearlyEqual(Previous, Hunger)) return false;
    BroadcastChanged();
    return true;
}

bool UApocalypseSurvivalComponent::ConsumeWater(float Amount)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || Amount <= 0.0f || !IsAlive())
    {
        return false;
    }
    const float Previous = Thirst;
    Thirst = FMath::Min(MaxThirst, Thirst + Amount);
    if (FMath::IsNearlyEqual(Previous, Thirst)) return false;
    BroadcastChanged();
    return true;
}

bool UApocalypseSurvivalComponent::SpendStamina(float Amount)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || Amount <= 0.0f || !IsAlive() || Stamina < Amount)
    {
        return false;
    }
    Stamina = FMath::Max(0.0f, Stamina - Amount);
    BroadcastChanged();
    return true;
}

void UApocalypseSurvivalComponent::RestoreStamina(float Amount)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || Amount <= 0.0f || !IsAlive()) return;
    const float Previous = Stamina;
    Stamina = FMath::Min(MaxStamina, Stamina + Amount);
    if (!FMath::IsNearlyEqual(Previous, Stamina)) BroadcastChanged();
}

void UApocalypseSurvivalComponent::ApplyDamage(float Amount)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || Amount <= 0.0f || !IsAlive()) return;
    Health = FMath::Max(0.0f, Health - Amount);
    BroadcastChanged();
}

void UApocalypseSurvivalComponent::RestoreHealth(float Amount)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || Amount <= 0.0f || !IsAlive()) return;
    const float Previous = Health;
    Health = FMath::Min(MaxHealth, Health + Amount);
    if (!FMath::IsNearlyEqual(Previous, Health)) BroadcastChanged();
}

void UApocalypseSurvivalComponent::OnRep_SurvivalValues()
{
    BroadcastChanged();
}

void UApocalypseSurvivalComponent::BroadcastChanged()
{
    OnSurvivalChanged.Broadcast();
}

void UApocalypseSurvivalComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UApocalypseSurvivalComponent, Health);
    DOREPLIFETIME(UApocalypseSurvivalComponent, Hunger);
    DOREPLIFETIME(UApocalypseSurvivalComponent, Thirst);
    DOREPLIFETIME(UApocalypseSurvivalComponent, Stamina);
}