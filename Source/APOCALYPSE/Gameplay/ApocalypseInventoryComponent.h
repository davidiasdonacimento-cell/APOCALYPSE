#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gameplay/ApocalypseItemCatalog.h"
#include "ApocalypseInventoryComponent.generated.h"
class UApocalypseInventoryComponent;
USTRUCT(BlueprintType)
struct FApocalypseInventoryStack
{
    GENERATED_BODY()
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) FName ItemId = NAME_None;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int32 Quantity = 0;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnApocalypseInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnApocalypseStorageOpened, UApocalypseInventoryComponent*, StorageInventory);
UCLASS(ClassGroup=(APOCALYPSE), meta=(BlueprintSpawnableComponent))
class APOCALYPSE_API UApocalypseInventoryComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UApocalypseInventoryComponent();
    UPROPERTY(BlueprintAssignable, Category="Inventory") FOnApocalypseInventoryChanged OnInventoryChanged;
    UPROPERTY(BlueprintAssignable, Category="Inventory|Storage") FOnApocalypseStorageOpened OnStorageOpened;
    UFUNCTION(BlueprintPure, Category="Inventory") int32 GetItemQuantity(FName ItemId) const;
    UFUNCTION(BlueprintPure, Category="Inventory") const TArray<FApocalypseInventoryStack>& GetItems() const { return Items; }
    UFUNCTION(BlueprintCallable, Category="Inventory") int32 AddItem(FName ItemId, int32 Quantity);
    UFUNCTION(BlueprintCallable, Category="Inventory") bool RemoveItem(FName ItemId, int32 Quantity);
    UFUNCTION(BlueprintCallable, Category="Inventory") bool ConsumeItem(FName ItemId);
    UFUNCTION(BlueprintCallable, Category="Inventory") void RequestTransferTo(UApocalypseInventoryComponent* Target, FName ItemId, int32 Quantity);
    UFUNCTION(BlueprintPure, Category="Inventory") UApocalypseItemCatalog* GetCatalog() const { return ItemCatalog; }
    UFUNCTION(BlueprintCallable, Category="Inventory|Storage") void NotifyStorageOpened(UApocalypseInventoryComponent* StorageInventory);
protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory") TObjectPtr<UApocalypseItemCatalog> ItemCatalog;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Capacity", meta=(ClampMin="1", ClampMax="200")) int32 MaxSlots = 24;
    UPROPERTY(ReplicatedUsing=OnRep_Items, VisibleInstanceOnly, BlueprintReadOnly, Category="Inventory") TArray<FApocalypseInventoryStack> Items;
    UFUNCTION() void OnRep_Items();
    UFUNCTION(Server, Reliable) void ServerTransferTo(UApocalypseInventoryComponent* Target, FName ItemId, int32 Quantity);
    UFUNCTION(Client, Reliable) void ClientNotifyStorageOpened(UApocalypseInventoryComponent* StorageInventory);
private:
    void NotifyInventoryChanged();
    bool IsValidTransferTarget(const UApocalypseInventoryComponent* Target) const;
};