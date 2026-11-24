#include "Gameplay/ApocalypseStorageContainer.h"
#include "Gameplay/ApocalypseInventoryComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
AApocalypseStorageContainer::AApocalypseStorageContainer()
{
    bReplicates = true;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
    InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
    InteractionCollision->SetupAttachment(SceneRoot);
    InteractionCollision->SetBoxExtent(FVector(80.0f));
    InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(SceneRoot);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Inventory = CreateDefaultSubobject<UApocalypseInventoryComponent>(TEXT("StorageInventory"));
}
bool AApocalypseStorageContainer::CanInteract_Implementation(AActor* InteractingActor) const
{
    return IsValid(InteractingActor) && Inventory != nullptr &&
        InteractingActor->FindComponentByClass<UApocalypseInventoryComponent>() != nullptr;
}
void AApocalypseStorageContainer::Interact_Implementation(AActor* InteractingActor)
{
    if (!HasAuthority() || !CanInteract_Implementation(InteractingActor)) return;
    UApocalypseInventoryComponent* PlayerInventory = InteractingActor->FindComponentByClass<UApocalypseInventoryComponent>();
    if (PlayerInventory) PlayerInventory->NotifyStorageOpened(Inventory);
}