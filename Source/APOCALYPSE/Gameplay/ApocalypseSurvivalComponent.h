#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ApocalypseSurvivalComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnApocalypseSurvivalChanged);

UCLASS(ClassGroup=(APOCALYPSE), meta=(BlueprintSpawnableComponent))
class APOCALYPSE_API UApocalypseSurvivalComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UApocalypseSurvivalComponent();

    UPROPERTY(BlueprintAssignable, Category="Survival")
    FOnApocalypseSurvivalChanged OnSurvivalChanged;

    UFUNCTION(BlueprintPure, Category="Survival")
    float GetHealth() const { return Health; }

    UFUNCTION(BlueprintPure, Category="Survival")
    float GetHunger() const { return Hunger; }

    UFUNCTION(BlueprintPure, Category="Survival")
    float GetThirst() const { return Thirst; }

    UFUNCTION(BlueprintPure, Category="Survival")
    float GetStamina() const { return Stamina; }

    UFUNCTION(BlueprintPure, Category="Survival")
    bool IsAlive() const { return Health > 0.0f; }

    UFUNCTION(BlueprintCallable, Category="Survival")
    bool ConsumeFood(float Amount);

    UFUNCTION(BlueprintCallable, Category="Survival")
    bool ConsumeWater(float Amount);

    UFUNCTION(BlueprintCallable, Category="Survival")
    bool SpendStamina(float Amount);

    UFUNCTION(BlueprintCallable, Category="Survival")
    void RestoreStamina(float Amount);

    UFUNCTION(BlueprintCallable, Category="Survival")
    void ApplyDamage(float Amount);

    UFUNCTION(BlueprintCallable, Category="Survival")
    void RestoreHealth(float Amount);

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Survival|Limits", meta=(ClampMin="1.0"))
    float MaxHealth = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Survival|Limits", meta=(ClampMin="1.0"))
    float MaxHunger = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Survival|Limits", meta=(ClampMin="1.0"))
    float MaxThirst = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Survival|Limits", meta=(ClampMin="1.0"))
    float MaxStamina = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Survival|Rates", meta=(ClampMin="0.0"))
    float HungerDrainPerMinute = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Survival|Rates", meta=(ClampMin="0.0"))
    float ThirstDrainPerMinute = 1.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Survival|Rates", meta=(ClampMin="0.0"))
    float StarvationDamagePerSecond = 2.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Survival|Rates", meta=(ClampMin="0.0"))
    float DehydrationDamagePerSecond = 3.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Survival|Rates", meta=(ClampMin="0.0"))
    float StaminaRecoveryPerSecond = 12.0f;

    UPROPERTY(ReplicatedUsing=OnRep_SurvivalValues, VisibleInstanceOnly, BlueprintReadOnly, Category="Survival")
    float Health = 100.0f;

    UPROPERTY(ReplicatedUsing=OnRep_SurvivalValues, VisibleInstanceOnly, BlueprintReadOnly, Category="Survival")
    float Hunger = 100.0f;

    UPROPERTY(ReplicatedUsing=OnRep_SurvivalValues, VisibleInstanceOnly, BlueprintReadOnly, Category="Survival")
    float Thirst = 100.0f;

    UPROPERTY(ReplicatedUsing=OnRep_SurvivalValues, VisibleInstanceOnly, BlueprintReadOnly, Category="Survival")
    float Stamina = 100.0f;

    UFUNCTION()
    void OnRep_SurvivalValues();

private:
    float SimulationAccumulator = 0.0f;
    void SimulateOneSecond();
    void BroadcastChanged();
};