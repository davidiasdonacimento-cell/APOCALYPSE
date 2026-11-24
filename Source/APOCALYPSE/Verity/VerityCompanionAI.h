#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VerityPersonalitySystem.h"
#include "VerityCompanionAI.generated.h"

UCLASS(ClassGroup=(VERITY), meta=(BlueprintSpawnableComponent))
class APOCALYPSE_API UVerityCompanionAI : public UActorComponent
{
    GENERATED_BODY()
public:
    UVerityCompanionAI();
    void SetPlayerTarget(AActor* Player);
    void SetAIEnabled(bool bEnabled);
    UFUNCTION(BlueprintPure, Category="VERITY|AI") bool IsThreatNearby() const { return CurrentThreat.IsValid(); }
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VERITY|AI", meta=(ClampMin="300.0", ClampMax="2500.0")) float AwarenessRadius = 1100.0f;
protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
    TWeakObjectPtr<AActor> PlayerTarget;
    TWeakObjectPtr<AActor> CurrentThreat;
    bool bAIEnabled = true;
    float ScanCooldown = 0.0f;
};
