#include "Gameplay/ApocalypseResourceNode.h"
#include "Gameplay/ApocalypseInventoryComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
AApocalypseResourceNode::AApocalypseResourceNode()
{
    bReplicates = true;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
    InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
    InteractionCollision->SetupAttachment(SceneRoot);
    InteractionCollision->SetBoxExtent(FVector(90.0f));
    InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(SceneRoot);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
void AApocalypseResourceNode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AApocalypseResourceNode, ResourceItemId);
    DOREPLIFETIME(AApocalypseResourceNode, RemainingQuantity);
}