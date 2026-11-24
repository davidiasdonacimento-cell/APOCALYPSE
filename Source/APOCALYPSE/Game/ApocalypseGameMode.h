#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ApocalypseGameMode.generated.h"

class UWorldRegionRegistry;

UCLASS()
class APOCALYPSE_API AApocalypseGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AApocalypseGameMode();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="World")
    TObjectPtr<UWorldRegionRegistry> WorldRegionRegistryAsset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="World", meta=(ToolTip="Stable seed used by world systems such as regional weather."))
    int32 WorldSeed = 20261124;
};
