#include "Gameplay/ApocalypseStorageContainer.h"
#include "Gameplay/ApocalypseInventoryComponent.h"
AApocalypseStorageContainer::AApocalypseStorageContainer()
{
    bReplicates = true;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
    Inventory = CreateDefaultSubobject<UApocalypseInventoryComponent>(TEXT("StorageInventory"));
}
bool AApocalypseStorageContainer::CanInteract_Implementation(AActor* InteractingActor) const
{
    return IsValid(InteractingActor) && Inventory != nullptr;
}
void AApocalypseStorageContainer::Interact_Implementation(AActor* InteractingActor)
{
    if (HasAuthority() && CanInteract_Implementation(InteractingActor)) OnStorageOpened.Broadcast(InteractingActor);
}