#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ApocalypseCharacter.generated.h"

class UApocalypseSurvivalComponent;
class UApocalypseInteractionComponent;
class UApocalypseInventoryComponent;
class UApocalypseEquipmentComponent;
class UApocalypseInventoryWidget;
class UVerityCompanionCharacter;
class UVeritySettingsWidget;
class AController;

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
    UFUNCTION(BlueprintPure, Category="APOCALYPSE|VERITY") UVerityCompanionCharacter* GetVerityCompanion() const { return VerityCompanion; }
protected:
    virtual void BeginPlay() override;
    virtual void OnRep_Controller() override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="APOCALYPSE|Gameplay", meta=(AllowPrivateAccess="true")) TObjectPtr<UApocalypseSurvivalComponent> SurvivalComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="APOCALYPSE|Gameplay", meta=(AllowPrivateAccess="true")) TObjectPtr<UApocalypseInteractionComponent> InteractionComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="APOCALYPSE|Gameplay", meta=(AllowPrivateAccess="true")) TObjectPtr<UApocalypseInventoryComponent> InventoryComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="APOCALYPSE|Gameplay", meta=(AllowPrivateAccess="true")) TObjectPtr<UApocalypseEquipmentComponent> EquipmentComponent;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="APOCALYPSE|UI", meta=(AllowPrivateAccess="true")) TSubclassOf<UApocalypseInventoryWidget> InventoryWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="APOCALYPSE|VERITY") bool bEnableVerityCompanion = true;
    UPROPERTY(ReplicatedUsing=OnRep_VerityCompanion, VisibleInstanceOnly, BlueprintReadOnly, Category="APOCALYPSE|VERITY") TObjectPtr<UVerityCompanionCharacter> VerityCompanion;
private:
    UPROPERTY(Transient) TObjectPtr<UApocalypseInventoryWidget> InventoryWidget;
    UPROPERTY(Transient) TObjectPtr<UVeritySettingsWidget> VeritySettingsWidget;
    UFUNCTION() void OnRep_VerityCompanion();
    void SpawnVerityCompanion();
    void CreateInventoryWidgetIfNeeded();
    void CreateVeritySettingsWidgetIfNeeded();
};
