#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ApocalypseInteractionComponent.generated.h"

UCLASS(ClassGroup=(APOCALYPSE), meta=(BlueprintSpawnableComponent))
class APOCALYPSE_API UApocalypseInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UApocalypseInteractionComponent();

    UFUNCTION(BlueprintCallable, Category="Interaction")
    void TryInteract();

    UFUNCTION(BlueprintPure, Category="Interaction")
    AActor* GetFocusedActor() const { return FocusedActor.Get(); }

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction", meta=(ClampMin="100.0", ClampMax="1000.0"))
    float InteractionDistance = 300.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction")
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

private:
    TWeakObjectPtr<AActor> FocusedActor;

    UFUNCTION(Server, Reliable)
    void ServerTryInteract();

    bool TraceForInteractable(FHitResult& OutHit) const;
};