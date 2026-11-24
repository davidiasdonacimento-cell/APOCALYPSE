#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/ApocalypseInteractable.h"
#include "ApocalypseStorageContainer.generated.h"
UCLASS()
class APOCALYPSE_API AApocalypseStorageContainer : public AActor, public IApocalypseInteractable
{
    GENERATED_BODY()
public:
    AApocalypseStorageContainer();
    virtual bool CanInteract_Implementation(AActor* InteractingActor) const override;
    virtual void Interact_Implementation(AActor* InteractingActor) override;
    UFUNCTION(BlueprintPure, Category="Storage") class UApocalypseInventoryComponent* GetStorageInventory() const { return Inventory; }
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Storage") TObjectPtr<USceneComponent> SceneRoot;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Storage") TObjectPtr<class UBoxComponent> InteractionCollision;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Storage") TObjectPtr<class UStaticMeshComponent> VisualMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Storage") TObjectPtr<class UApocalypseInventoryComponent> Inventory;
};