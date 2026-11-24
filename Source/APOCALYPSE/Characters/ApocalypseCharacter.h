#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ApocalypseCharacter.generated.h"

UCLASS()
class APOCALYPSE_API AApocalypseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AApocalypseCharacter();

protected:
    virtual void BeginPlay() override;
};
