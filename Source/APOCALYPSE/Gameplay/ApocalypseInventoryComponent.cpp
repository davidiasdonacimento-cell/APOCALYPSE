#include "Gameplay/ApocalypseInventoryComponent.h"
#include "Gameplay/ApocalypseSurvivalComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

UApocalypseInventoryComponent::UApocalypseInventoryComponent()
{
    SetIsReplicatedByDefault(true);
}

int32 UApocalypseInventoryComponent::GetItemQuantity(FName ItemId) const
{
    int32 Total = 0;
    for (const FApocalypseInventoryStack& Stack : Items)
        if (Stack.ItemId == ItemId) Total += Stack.Quantity;
    return Total;
}

int32 UApocalypseInventoryComponent::AddItem(FName ItemId, int32 Quantity)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || ItemId.IsNone() || Quantity <= 0) return 0;
    const UApocalypseItemCatalog* Catalog = GetCatalog();
    const FApocalypseItemDefinition* Definition = Catalog ? Catalog->FindItem(ItemId) : nullptr;
    if (!Definition || Definition->MaxStack <= 0) return 0;

    int32 Remaining = Quantity;
    for (FApocalypseInventoryStack& Stack : Items)
    {
        if (Stack.ItemId != ItemId || Stack.Quantity >= Definition->MaxStack) continue;
        const int32 Added = FMath::Min(Remaining, Definition->MaxStack - Stack.Quantity);
        Stack.Quantity += Added;
        Remaining -= Added;
        if (Remaining == 0) break;
    }
    while (Remaining > 0 && Items.Num() < MaxSlots)
    {
        FApocalypseInventoryStack& NewStack = Items.AddDefaulted_GetRef();
        NewStack.ItemId = ItemId;
        NewStack.Quantity = FMath::Min(Remaining, Definition->MaxStack);
        Remaining -= NewStack.Quantity;
    }

    const int32 Accepted = Quantity - Remaining;
    if (Accepted > 0) NotifyInventoryChanged();
    return Accepted;
}

bool UApocalypseInventoryComponent::RemoveItem(FName ItemId, int32 Quantity)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || ItemId.IsNone() ||
        Quantity <= 0 || GetItemQuantity(ItemId) < Quantity) return false;

    int32 Remaining = Quantity;
    for (int32 Index = Items.Num() - 1; Index >= 0 && Remaining > 0; --Index)
    {
        FApocalypseInventoryStack& Stack = Items[Index];
        if (Stack.ItemId != ItemId) continue;
        const int32 Removed = FMath::Min(Remaining, Stack.Quantity);
        Stack.Quantity -= Removed;
        Remaining -= Removed;
        if (Stack.Quantity <= 0) Items.RemoveAt(Index);
    }
    NotifyInventoryChanged();
    return Remaining == 0;
}

bool UApocalypseInventoryComponent::ConsumeItem(FName ItemId)
{
    if (!GetOwner() || !GetOwner()->HasAuthority() || GetItemQuantity(ItemId) < 1) return false;
    const UApocalypseItemCatalog* Catalog = GetCatalog();
    const FApocalypseItemDefinition* Definition = Catalog ? Catalog->FindItem(ItemId) : nullptr;
    if (!Definition) return false;

    UApocalypseSurvivalComponent* Survival = GetOwner()->FindComponentByClass<UApocalypseSurvivalComponent>();
    const bool bHasEffect = Definition->HungerRestoration > 0.0f ||
        Definition->ThirstRestoration > 0.0f || Definition->HealthRestoration > 0.0f;
    if (!Survival || !Survival->IsAlive() || !bHasEffect || !RemoveItem(ItemId, 1)) return false;

    if (Definition->HungerRestoration > 0.0f) Survival->ConsumeFood(Definition->HungerRestoration);
    if (Definition->ThirstRestoration > 0.0f) Survival->ConsumeWater(Definition->ThirstRestoration);
    if (Definition->HealthRestoration > 0.0f) Survival->RestoreHealth(Definition->HealthRestoration);
    return true;
}

void UApocalypseInventoryComponent::RequestTransferTo(UApocalypseInventoryComponent* Target, FName ItemId, int32 Quantity)
{
    ServerTransferTo(Target, ItemId, Quantity);
}

void UApocalypseInventoryComponent::RequestTransferFrom(UApocalypseInventoryComponent* Source, FName ItemId, int32 Quantity)
{
    ServerTransferFrom(Source, ItemId, Quantity);
}

void UApocalypseInventoryComponent::ServerTransferTo_Implementation(UApocalypseInventoryComponent* Target, FName ItemId, int32 Quantity)
{
    if (!IsValidTransferTarget(Target) || ItemId.IsNone() || Quantity <= 0) return;
    const AActor* SourceOwner = GetOwner();
    const AActor* TargetOwner = Target->GetOwner();
    if (FVector::DistSquared(SourceOwner->GetActorLocation(), TargetOwner->GetActorLocation()) > FMath::Square(500.0f)) return;

    const int32 Available = GetItemQuantity(ItemId);
    const int32 Accepted = Target->AddItem(ItemId, FMath::Min(Quantity, Available));
    if (Accepted > 0 && !RemoveItem(ItemId, Accepted))
        Target->RemoveItem(ItemId, Accepted);
}

void UApocalypseInventoryComponent::ServerTransferFrom_Implementation(UApocalypseInventoryComponent* Source, FName ItemId, int32 Quantity)
{
    if (!IsValidTransferTarget(Source) || ItemId.IsNone() || Quantity <= 0) return;
    const AActor* TargetOwner = GetOwner();
    const AActor* SourceOwner = Source->GetOwner();
    if (FVector::DistSquared(SourceOwner->GetActorLocation(), TargetOwner->GetActorLocation()) > FMath::Square(500.0f)) return;

    const int32 Available = Source->GetItemQuantity(ItemId);
    const int32 Accepted = AddItem(ItemId, FMath::Min(Quantity, Available));
    if (Accepted > 0 && !Source->RemoveItem(ItemId, Accepted))
        RemoveItem(ItemId, Accepted);
}

bool UApocalypseInventoryComponent::IsValidTransferTarget(const UApocalypseInventoryComponent* Target) const
{
    return Target && Target != this && GetOwner() && Target->GetOwner() &&
        GetOwner()->HasAuthority() && Target->GetOwner()->HasAuthority() &&
        GetOwner()->GetWorld() == Target->GetOwner()->GetWorld();
}

void UApocalypseInventoryComponent::NotifyStorageOpened(UApocalypseInventoryComponent* StorageInventory)
{
    if (GetOwner() && GetOwner()->HasAuthority() && StorageInventory)
        ClientNotifyStorageOpened(StorageInventory);
}

void UApocalypseInventoryComponent::ClientNotifyStorageOpened_Implementation(UApocalypseInventoryComponent* StorageInventory)
{
    if (StorageInventory) OnStorageOpened.Broadcast(StorageInventory);
}

void UApocalypseInventoryComponent::OnRep_Items() { NotifyInventoryChanged(); }
void UApocalypseInventoryComponent::NotifyInventoryChanged() { OnInventoryChanged.Broadcast(); }

void UApocalypseInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UApocalypseInventoryComponent, Items);
}
