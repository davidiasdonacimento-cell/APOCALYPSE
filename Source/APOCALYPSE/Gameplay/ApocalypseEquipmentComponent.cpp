#include "Gameplay/ApocalypseEquipmentComponent.h"
#include "Gameplay/ApocalypseInventoryComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

UApocalypseEquipmentComponent::UApocalypseEquipmentComponent()
{
    SetIsReplicatedByDefault(true);
    PrimaryComponentTick.bCanEverTick = false;
}

FName UApocalypseEquipmentComponent::GetEquippedItem(EApocalypseEquipmentSlot Slot) const
{
    for (const FApocalypseEquippedItem& Entry : EquippedItems)
        if (Entry.Slot == Slot) return Entry.ItemId;
    return NAME_None;
}

void UApocalypseEquipmentComponent::RequestEquip(FName ItemId)
{
    if (!ItemId.IsNone()) ServerEquip(ItemId);
}

void UApocalypseEquipmentComponent::RequestUnequip(EApocalypseEquipmentSlot Slot)
{
    ServerUnequip(Slot);
}

void UApocalypseEquipmentComponent::ServerEquip_Implementation(FName ItemId)
{
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->HasAuthority() || ItemId.IsNone()) return;

    UApocalypseInventoryComponent* Inventory = Owner->FindComponentByClass<UApocalypseInventoryComponent>();
    const UApocalypseItemCatalog* Catalog = Inventory ? Inventory->GetCatalog() : nullptr;
    const FApocalypseItemDefinition* Definition = Catalog ? Catalog->FindItem(ItemId) : nullptr;
    if (!Inventory || !Definition || Definition->Category != EApocalypseItemCategory::Equipment ||
        Inventory->GetItemQuantity(ItemId) < 1) return;

    const EApocalypseEquipmentSlot Slot = Definition->EquipmentSlot;
    const FName PreviousItem = GetEquippedItem(Slot);
    if (!Inventory->RemoveItem(ItemId, 1)) return;

    if (!PreviousItem.IsNone() && Inventory->AddItem(PreviousItem, 1) != 1)
    {
        Inventory->AddItem(ItemId, 1);
        return;
    }

    bool bUpdated = false;
    for (FApocalypseEquippedItem& Entry : EquippedItems)
    {
        if (Entry.Slot == Slot)
        {
            Entry.ItemId = ItemId;
            bUpdated = true;
            break;
        }
    }
    if (!bUpdated)
    {
        FApocalypseEquippedItem& NewEntry = EquippedItems.AddDefaulted_GetRef();
        NewEntry.Slot = Slot;
        NewEntry.ItemId = ItemId;
    }
    NotifyEquipmentChanged();
}

void UApocalypseEquipmentComponent::ServerUnequip_Implementation(EApocalypseEquipmentSlot Slot)
{
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->HasAuthority()) return;
    UApocalypseInventoryComponent* Inventory = Owner->FindComponentByClass<UApocalypseInventoryComponent>();
    if (!Inventory) return;

    for (int32 Index = 0; Index < EquippedItems.Num(); ++Index)
    {
        if (EquippedItems[Index].Slot != Slot) continue;
        const FName ItemId = EquippedItems[Index].ItemId;
        if (ItemId.IsNone() || Inventory->AddItem(ItemId, 1) != 1) return;
        EquippedItems.RemoveAt(Index);
        NotifyEquipmentChanged();
        return;
    }
}

void UApocalypseEquipmentComponent::OnRep_EquippedItems()
{
    NotifyEquipmentChanged();
}

void UApocalypseEquipmentComponent::NotifyEquipmentChanged()
{
    OnEquipmentChanged.Broadcast();
}

void UApocalypseEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UApocalypseEquipmentComponent, EquippedItems);
}
