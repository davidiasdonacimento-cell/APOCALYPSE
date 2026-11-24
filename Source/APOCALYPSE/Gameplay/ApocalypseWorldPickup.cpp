#include "Gameplay/ApocalypseWorldPickup.h"
#include "Gameplay/ApocalypseInventoryComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
AApocalypseWorldPickup::AApocalypseWorldPickup()
{
    bReplicates = true;
    SetReplicateMovement(true);
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
    InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
    InteractionCollision->SetupAttachment(SceneRoot);
    InteractionCollision->SetBoxExtent(FVector(45.0f));
    InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(SceneRoot);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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