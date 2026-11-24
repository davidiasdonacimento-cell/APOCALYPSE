#include "Gameplay/ApocalypseWorldPickup.h"
#include "Gameplay/ApocalypseInventoryComponent.h"
AApocalypseWorldPickup::AApocalypseWorldPickup()
{
    bReplicates = true;
    SetReplicateMovement(true);
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
}
bool AApocalypseWorldPickup::CanInteract_Implementation(AActor* InteractingActor) const
{
    return IsValid(InteractingActor) && !ItemId.IsNone() && Quantity > 0 &&
        InteractingActor->FindComponentByClass<UApocalypseInventoryComponent>() != nullptr;
}
void AApocalypseWorldPickup::Interact_Implementation(AActor* InteractingActor)
{
    if (!HasAuthority() || !CanInteract_Implementation(InteractingActor)) return;
    UApocalypseInventoryComponent* Inventory = InteractingActor->FindComponentByClass<UApocalypseInventoryComponent>();
    if (!Inventory) return;
    Quantity -= Inventory->AddItem(ItemId, Quantity);
    if (Quantity <= 0) Destroy();
}