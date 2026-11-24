#include "Game/ApocalypseGameMode.h"
#include "Characters/ApocalypseCharacter.h"
#include "World/ApocalypseWorldSubsystem.h"
#include "World/WorldRegionRegistry.h"

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
