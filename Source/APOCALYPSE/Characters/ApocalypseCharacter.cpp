#include "Characters/ApocalypseCharacter.h"
#include "Gameplay/ApocalypseInteractionComponent.h"
#include "Gameplay/ApocalypseInventoryComponent.h"
#include "Gameplay/ApocalypseSurvivalComponent.h"
AApocalypseCharacter::AApocalypseCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    SurvivalComponent = CreateDefaultSubobject<UApocalypseSurvivalComponent>(TEXT("SurvivalComponent"));
    InteractionComponent = CreateDefaultSubobject<UApocalypseInteractionComponent>(TEXT("InteractionComponent"));
    InventoryComponent = CreateDefaultSubobject<UApocalypseInventoryComponent>(TEXT("InventoryComponent"));
}
void AApocalypseCharacter::BeginPlay() { Super::BeginPlay(); }