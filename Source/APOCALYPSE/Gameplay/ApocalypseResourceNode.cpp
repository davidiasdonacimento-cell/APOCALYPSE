#include "Gameplay/ApocalypseResourceNode.h"
#include "Gameplay/ApocalypseInventoryComponent.h"
AApocalypseResourceNode::AApocalypseResourceNode()
{
    bReplicates = true;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
}
bool AApocalypseResourceNode::CanInteract_Implementation(AActor* InteractingActor) const
{
    return IsValid(InteractingActor) && !ResourceItemId.IsNone() && RemainingQuantity > 0 && YieldPerHarvest > 0 &&
        InteractingActor->FindComponentByClass<UApocalypseInventoryComponent>() != nullptr;
}
void AApocalypseResourceNode::Interact_Implementation(AActor* InteractingActor)
{
    if (!HasAuthority() || !CanInteract_Implementation(InteractingActor)) return;
    UApocalypseInventoryComponent* Inventory = InteractingActor->FindComponentByClass<UApocalypseInventoryComponent>();
    if (!Inventory) return;
    const int32 Requested = FMath::Min(YieldPerHarvest, RemainingQuantity);
    RemainingQuantity -= Inventory->AddItem(ResourceItemId, Requested);
    if (RemainingQuantity <= 0) Destroy();
}