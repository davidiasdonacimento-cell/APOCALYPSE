#include "Game/ApocalypseGameMode.h"
#include "Characters/ApocalypseCharacter.h"
#include "World/ApocalypseWorldSubsystem.h"
#include "World/WorldRegionRegistry.h"
#include "UI/MainLobbyWidget.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AApocalypseGameMode::AApocalypseGameMode()
{
    DefaultPawnClass = AApocalypseCharacter::StaticClass();
}
void AApocalypseGameMode::BeginPlay()
{
    Super::BeginPlay();
    if (UWorld* World = GetWorld())
    {
        if (UApocalypseWorldSubsystem* WorldSubsystem = World->GetSubsystem<UApocalypseWorldSubsystem>())
        {
            WorldSubsystem->SetWorldSeed(WorldSeed);
            WorldSubsystem->SetRegionRegistry(WorldRegionRegistryAsset);
        }
    }
}
void AApocalypseGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    ShowLobby(NewPlayer);
}
void AApocalypseGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    // Deliberately do not spawn a pawn on application start. The lobby owns the first screen.
    ShowLobby(NewPlayer);
}
void AApocalypseGameMode::ShowLobby(APlayerController* PlayerController)
{
    if (!IsValid(PlayerController) || LobbyWidgets.Contains(PlayerController)) return;
    const TSubclassOf<UMainLobbyWidget> ClassToCreate = LobbyWidgetClass ? LobbyWidgetClass : UMainLobbyWidget::StaticClass();
    UMainLobbyWidget* Widget = CreateWidget<UMainLobbyWidget>(PlayerController, ClassToCreate);
    if (!Widget) return;
    LobbyWidgets.Add(PlayerController, Widget);
    Widget->AddToViewport(100);
    PlayerController->SetShowMouseCursor(true);
    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(Widget->TakeWidget());
    PlayerController->SetInputMode(InputMode);
}
void AApocalypseGameMode::StartGameForPlayer(APlayerController* PlayerController, bool bContinue)
{
    if (!HasAuthority() || !IsValid(PlayerController)) return;
    if (UUserWidget** Existing = LobbyWidgets.Find(PlayerController))
    {
        if (IsValid(*Existing)) (*Existing)->RemoveFromParent();
        LobbyWidgets.Remove(PlayerController);
    }
    PlayerController->SetShowMouseCursor(false);
    FInputModeGameOnly InputMode;
    PlayerController->SetInputMode(InputMode);
    RestartPlayer(PlayerController);
}