#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ApocalypseGameMode.generated.h"
class UWorldRegionRegistry;
class UMainLobbyWidget;
class APlayerController;
UCLASS()
class APOCALYPSE_API AApocalypseGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AApocalypseGameMode();
    UFUNCTION(BlueprintCallable, Category="APOCALYPSE|Lobby") void StartGameForPlayer(APlayerController* PlayerController, bool bContinue);
protected:
    virtual void BeginPlay() override;
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="World") TObjectPtr<UWorldRegionRegistry> WorldRegionRegistryAsset;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="World", meta=(ToolTip="Stable seed used by world systems such as regional weather.")) int32 WorldSeed = 20261124;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Lobby") TSubclassOf<UMainLobbyWidget> LobbyWidgetClass;
private:
    UPROPERTY(Transient) TMap<TObjectPtr<APlayerController>, TObjectPtr<UMainLobbyWidget>> LobbyWidgets;
    void ShowLobby(APlayerController* PlayerController);
};