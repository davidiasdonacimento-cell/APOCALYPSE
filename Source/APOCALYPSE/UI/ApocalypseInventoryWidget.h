#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/ApocalypseInventoryComponent.h"
#include "Gameplay/ApocalypseEquipmentComponent.h"
#include "ApocalypseInventoryWidget.generated.h"

class UButton;
class UComboBoxString;
class UTextBlock;
class UVerticalBox;

UCLASS()
class APOCALYPSE_API UApocalypseInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="APOCALYPSE|UI")
    void SetInventoryContext(UApocalypseInventoryComponent* InInventory,
                             UApocalypseEquipmentComponent* InEquipment);

protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(Transient)
    TObjectPtr<UApocalypseInventoryComponent> Inventory;

    UPROPERTY(Transient)
    TObjectPtr<UApocalypseEquipmentComponent> Equipment;

    UPROPERTY(Transient)
    TObjectPtr<UApocalypseInventoryComponent> OpenStorage;

    UPROPERTY(Transient)
    TObjectPtr<UComboBoxString> ItemSelector;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> StatusText;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> EquipmentText;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> StorageText;

    UPROPERTY(Transient)
    TObjectPtr<UButton> ConsumeButton;

    UPROPERTY(Transient)
    TObjectPtr<UButton> EquipButton;

    UPROPERTY(Transient)
    TObjectPtr<UButton> StoreButton;

    UPROPERTY(Transient)
    TObjectPtr<UButton> TakeButton;

    FName SelectedItemId = NAME_None;

    UFUNCTION()
    void HandleSelectionChanged(FString Selected, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void HandleConsume();

    UFUNCTION()
    void HandleEquip();

    UFUNCTION()
    void HandleStore();

    UFUNCTION()
    void HandleTake();

    UFUNCTION()
    void HandleInventoryChanged();

    UFUNCTION()
    void HandleEquipmentChanged();

    UFUNCTION()
    void HandleStorageOpened(UApocalypseInventoryComponent* StorageInventory);

    void Refresh();
    void SetStatus(const FText& Message);
};
