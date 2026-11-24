#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/ApocalypseInteractable.h"
#include "ApocalypseWorldPickup.generated.h"
UCLASS()
class APOCALYPSE_API AApocalypseWorldPickup : public AActor, public IApocalypseInteractable
{
    GENERATED_BODY()
public:
    AApocalypseWorldPickup();
    virtual bool CanInteract_Implementation(AActor* InteractingActor) const override;
    virtual void Interact_Implementation(AActor* InteractingActor) override;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Pickup") FName ItemId = NAME_None;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Pickup", meta=(ClampMin="1")) int32 Quantity = 1;
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup") TObjectPtr<USceneComponent> SceneRoot;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup") TObjectPtr<class UBoxComponent> InteractionCollision;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup") TObjectPtr<class UStaticMeshComponent> VisualMesh;
};