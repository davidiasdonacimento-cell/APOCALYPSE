#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VerityCombatSystem.generated.h"

UCLASS(ClassGroup=(VERITY), meta=(BlueprintSpawnableComponent))
class APOCALYPSE_API UVerityCombatSystem : public UActorComponent
{
    GENERATED_BODY()
public:
    UVerityCombatSystem();
    void SetTarget(AActor* NewTarget);
    void SetCombatEnabled(bool bEnabled);
    UFUNCTION(BlueprintPure, Category="VERITY|Combat") bool IsInCombat() const { return Target.IsValid(); }
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VERITY|Combat", meta=(ClampMin="1.0", ClampMax="100.0")) float FictionalDamagePerHit = 12.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VERITY|Combat", meta=(ClampMin="0.3", ClampMax="5.0")) float AttackInterval = 1.25f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VERITY|Combat", meta=(ClampMin="50.0", ClampMax="300.0")) float AttackRange = 190.0f;
protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
    TWeakObjectPtr<AActor> Target;
    bool bCombatEnabled = true;
    float AttackCooldown = 0.0f;
};
