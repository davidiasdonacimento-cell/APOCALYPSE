#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gameplay/ApocalypseItemCatalog.h"
#include "ApocalypseEquipmentComponent.generated.h"

class UApocalypseInventoryComponent;

UENUM(BlueprintType)
enum class EApocalypseEquipmentSlot : uint8
{
    Head,
    Body,
    Backpack,
    Utility
};

USTRUCT(BlueprintType)
struct FApocalypseEquippedItem
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EApocalypseEquipmentSlot Slot = EApocalypseEquipmentSlot::Utility;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName ItemId = NAME_None;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnApocalypseEquipmentChanged);

UCLASS(ClassGroup=(APOCALYPSE), meta=(BlueprintSpawnableComponent))
class APOCALYPSE_API UApocalypseEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UApocalypseEquipmentComponent();

    UPROPERTY(BlueprintAssignable, Category="Equipment")
    FOnApocalypseEquipmentChanged OnEquipmentChanged;

    UFUNCTION(BlueprintPure, Category="Equipment")
    const TArray<FApocalypseEquippedItem>& GetEquippedItems() const { return EquippedItems; }

    UFUNCTION(BlueprintPure, Category="Equipment")
    FName GetEquippedItem(EApocalypseEquipmentSlot Slot) const;

    UFUNCTION(BlueprintCallable, Category="Equipment")
    void RequestEquip(FName ItemId);

    UFUNCTION(BlueprintCallable, Category="Equipment")
    void RequestUnequip(EApocalypseEquipmentSlot Slot);

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(ReplicatedUsing=OnRep_EquippedItems, VisibleInstanceOnly, BlueprintReadOnly, Category="Equipment")
    TArray<FApocalypseEquippedItem> EquippedItems;

    UFUNCTION()
    void OnRep_EquippedItems();

    UFUNCTION(Server, Reliable)
    void ServerEquip(FName ItemId);

    UFUNCTION(Server, Reliable)
    void ServerUnequip(EApocalypseEquipmentSlot Slot);

private:
    void NotifyEquipmentChanged();
};
