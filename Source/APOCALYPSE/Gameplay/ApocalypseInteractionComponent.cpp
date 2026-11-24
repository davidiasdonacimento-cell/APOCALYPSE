#include "Gameplay/ApocalypseInteractionComponent.h"
#include "Gameplay/ApocalypseInteractable.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"

UApocalypseInteractionComponent::UApocalypseInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.1f;
    SetIsReplicatedByDefault(true);
}

void UApocalypseInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UApocalypseInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (GetOwner() && GetOwner()->IsLocallyControlled())
    {
        FHitResult Hit;
        AActor* NewFocus = TraceForInteractable(Hit) && Hit.GetActor() &&
            Hit.GetActor()->GetClass()->ImplementsInterface(UApocalypseInteractable::StaticClass())
            ? Hit.GetActor() : nullptr;

        if (FocusedActor.Get() != NewFocus)
        {
            FocusedActor = NewFocus;
        }
    }
}

void UApocalypseInteractionComponent::TryInteract()
{
    if (GetOwner() && GetOwner()->IsLocallyControlled())
    {
        ServerTryInteract();
    }
}

void UApocalypseInteractionComponent::ServerTryInteract_Implementation()
{
    FHitResult Hit;
    if (!TraceForInteractable(Hit)) return;

    AActor* Target = Hit.GetActor();
    AActor* InteractingActor = GetOwner();
    if (!IsValid(Target) || !IsValid(InteractingActor) ||
        !Target->GetClass()->ImplementsInterface(UApocalypseInteractable::StaticClass()))
    {
        return;
    }

    if (IApocalypseInteractable::Execute_CanInteract(Target, InteractingActor))
    {
        IApocalypseInteractable::Execute_Interact(Target, InteractingActor);
    }
}

bool UApocalypseInteractionComponent::TraceForInteractable(FHitResult& OutHit) const
{
    const AActor* OwnerActor = GetOwner();
    const APawn* OwnerPawn = Cast<APawn>(OwnerActor);
    const AController* Controller = OwnerPawn ? OwnerPawn->GetController() : nullptr;
    if (!OwnerActor || !Controller) return false;

    FVector ViewLocation;
    FRotator ViewRotation;
    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

    FCollisionQueryParams Params(SCENE_QUERY_STAT(ApocalypseInteraction), false, OwnerActor);
    const FVector End = ViewLocation + ViewRotation.Vector() * InteractionDistance;
    return GetWorld() && GetWorld()->LineTraceSingleByChannel(OutHit, ViewLocation, End, TraceChannel, Params);
}