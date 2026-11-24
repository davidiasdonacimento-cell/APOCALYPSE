#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ApocalypseInteractable.generated.h"

UINTERFACE(BlueprintType)
class APOCALYPSE_API UApocalypseInteractable : public UInterface
{
    GENERATED_BODY()
};

class APOCALYPSE_API IApocalypseInteractable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    bool CanInteract(AActor* InteractingActor) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    void Interact(AActor* InteractingActor);
};