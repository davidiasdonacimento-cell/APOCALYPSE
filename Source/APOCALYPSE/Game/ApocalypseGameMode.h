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
};
