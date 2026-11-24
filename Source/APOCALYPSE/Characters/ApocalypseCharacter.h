#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ApocalypseCharacter.generated.h"

class UApocalypseSurvivalComponent;
class UApocalypseInteractionComponent;
class UApocalypseInventoryComponent;
class UApocalypseEquipmentComponent;
class UApocalypseInventoryWidget;

UCLASS()
class APOCALYPSE_API AApocalypseCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AApocalypseCharacter();
    UFUNCTION(BlueprintPure, Category="APOCALYPSE|Gameplay") UApocalypseSurvivalComponent* GetSurvivalComponent() const { return SurvivalComponent; }
    UFUNCTION(BlueprintPure, Category="APOCALYPSE|Gameplay") UApocalypseInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }
    UFUNCTION(BlueprintPure, Category="APOCALYPSE|Gameplay") UApocalypseInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
    UFUNCTION(BlueprintPure, Category="APOCALYPSE|Gameplay") UApocalypseEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent; }
protected:
    virtual void BeginPlay() override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="APOCALYPSE|Gameplay", meta=(AllowPrivateAccess="true")) TObjectPtr<UApocalypseSurvivalComponent> SurvivalComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="APOCALYPSE|Gameplay", meta=(AllowPrivateAccess="true")) TObjectPtr<UApocalypseInteractionComponent> InteractionComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="APOCALYPSE|Gameplay", meta=(AllowPrivateAccess="true")) TObjectPtr<UApocalypseInventoryComponent> InventoryComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="APOCALYPSE|Gameplay", meta=(AllowPrivateAccess="true")) TObjectPtr<UApocalypseEquipmentComponent> EquipmentComponent;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="APOCALYPSE|UI") TSubclassOf<UApocalypseInventoryWidget> InventoryWidgetClass;
private:
    UPROPERTY(Transient) TObjectPtr<UApocalypseInventoryWidget> InventoryWidget;
};
