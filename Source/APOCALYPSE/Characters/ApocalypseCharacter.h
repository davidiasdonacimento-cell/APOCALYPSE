#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ApocalypseCharacter.generated.h"

class UApocalypseSurvivalComponent;
class UApocalypseInteractionComponent;

UCLASS()
class APOCALYPSE_API AApocalypseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AApocalypseCharacter();

    UFUNCTION(BlueprintPure, Category="APOCALYPSE|Gameplay")
    UApocalypseSurvivalComponent* GetSurvivalComponent() const { return SurvivalComponent; }

    UFUNCTION(BlueprintPure, Category="APOCALYPSE|Gameplay")
    UApocalypseInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="APOCALYPSE|Gameplay", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UApocalypseSurvivalComponent> SurvivalComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="APOCALYPSE|Gameplay", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UApocalypseInteractionComponent> InteractionComponent;
};