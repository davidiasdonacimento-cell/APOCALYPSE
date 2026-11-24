#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VerityHostileTarget.generated.h"

UINTERFACE(BlueprintType)
class UVerityHostileTarget : public UInterface
{
    GENERATED_BODY()
};

class APOCALYPSE_API IVerityHostileTarget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="VERITY|Target")
    bool IsValidVerityTarget(AActor* Observer) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="VERITY|Target")
    void ReceiveVerityDamage(float Amount, AActor* DamageInstigator);
};
