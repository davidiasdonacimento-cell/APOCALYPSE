#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/ApocalypseInteractable.h"
#include "ApocalypseResourceNode.generated.h"

UCLASS()
class APOCALYPSE_API AApocalypseResourceNode : public AActor, public IApocalypseInteractable
{
    GENERATED_BODY()
public:
    AApocalypseResourceNode();
    virtual bool CanInteract_Implementation(AActor* InteractingActor) const override;
    virtual void Interact_Implementation(AActor* InteractingActor) override;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Resource") FName ResourceItemId = NAME_None;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Resource", meta=(ClampMin="1")) int32 RemainingQuantity = 10;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Resource", meta=(ClampMin="1")) int32 YieldPerHarvest = 1;
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Resource") TObjectPtr<USceneComponent> SceneRoot;
};