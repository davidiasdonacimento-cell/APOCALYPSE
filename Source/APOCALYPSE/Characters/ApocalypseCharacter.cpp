#include "Characters/ApocalypseCharacter.h"
#include "Gameplay/ApocalypseInteractionComponent.h"
#include "Gameplay/ApocalypseInventoryComponent.h"
#include "Gameplay/ApocalypseEquipmentComponent.h"
#include "Gameplay/ApocalypseSurvivalComponent.h"
#include "UI/ApocalypseInventoryWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

AApocalypseCharacter::AApocalypseCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    SurvivalComponent = CreateDefaultSubobject<UApocalypseSurvivalComponent>(TEXT("SurvivalComponent"));
    InteractionComponent = CreateDefaultSubobject<UApocalypseInteractionComponent>(TEXT("InteractionComponent"));
    InventoryComponent = CreateDefaultSubobject<UApocalypseInventoryComponent>(TEXT("InventoryComponent"));
    EquipmentComponent = CreateDefaultSubobject<UApocalypseEquipmentComponent>(TEXT("EquipmentComponent"));
}

void AApocalypseCharacter::BeginPlay()
{
    Super::BeginPlay();
    CreateInventoryWidgetIfNeeded();
}

void AApocalypseCharacter::OnRep_Controller()
{
    Super::OnRep_Controller();
    CreateInventoryWidgetIfNeeded();
}

void AApocalypseCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    CreateInventoryWidgetIfNeeded();
}

void AApocalypseCharacter::CreateInventoryWidgetIfNeeded()
{
    if (InventoryWidget || !IsLocallyControlled()) return;
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController) return;

    const TSubclassOf<UApocalypseInventoryWidget> WidgetClass =
        InventoryWidgetClass ? InventoryWidgetClass : UApocalypseInventoryWidget::StaticClass();
    InventoryWidget = CreateWidget<UApocalypseInventoryWidget>(PlayerController, WidgetClass);
    if (!InventoryWidget) return;

    InventoryWidget->SetInventoryContext(InventoryComponent, EquipmentComponent);
    InventoryWidget->AddToViewport(20);
}
