#include "Game/ApocalypseGameMode.h"
#include "Characters/ApocalypseCharacter.h"

AApocalypseGameMode::AApocalypseGameMode()
{
    DefaultPawnClass = AApocalypseCharacter::StaticClass();
}
