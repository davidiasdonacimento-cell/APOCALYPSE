#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VerityNavigationSystem.generated.h"

UCLASS(ClassGroup=(VERITY), meta=(BlueprintSpawnableComponent))
class APOCALYPSE_API UVerityNavigationSystem : public UActorComponent
{
    GENERATED_BODY()
public:
    UVerityNavigationSystem();
    void SetFollowTarget(AActor* Target);
    void SetCombatTarget(AActor* Target);
    UFUNCTION(BlueprintCallable, Category="VERITY|Navigation") void SetFollowingEnabled(bool bEnabled);
    UFUNCTION(BlueprintPure, Category="VERITY|Navigation") bool IsFollowingEnabled() const { return bFollowingEnabled; }
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VERITY|Navigation", meta=(ClampMin="100.0")) float FollowDistance = 230.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VERITY|Navigation", meta=(ClampMin="50.0")) float CombatDistance = 170.0f;
protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
    TWeakObjectPtr<AActor> FollowTarget;
    TWeakObjectPtr<AActor> CombatTarget;
    bool bFollowingEnabled = true;
    float MoveRequestCooldown = 0.0f;
};
