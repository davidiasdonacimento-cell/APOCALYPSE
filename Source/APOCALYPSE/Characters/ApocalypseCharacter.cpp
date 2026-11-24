#include "Characters/ApocalypseCharacter.h"
#include "Gameplay/ApocalypseInteractionComponent.h"
#include "Gameplay/ApocalypseInventoryComponent.h"
#include "Gameplay/ApocalypseEquipmentComponent.h"
#include "Gameplay/ApocalypseSurvivalComponent.h"
#include "UI/ApocalypseInventoryWidget.h"
#include "UI/VeritySettingsWidget.h"
#include "Verity/VerityCompanionCharacter.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

AApocalypseCharacter::AApocalypseCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    SurvivalComponent = CreateDefaultSubobject<UApocalypseSurvivalComponent>(TEXT("SurvivalComponent"));
    InteractionComponent = CreateDefaultSubobject<UApocalypseInteractionComponent>(TEXT("InteractionComponent"));
    InventoryComponent = CreateDefaultSubobject<UApocalypseInventoryComponent>(TEXT("InventoryComponent"));
    EquipmentComponent = CreateDefaultSubobject<UApocalypseEquipmentComponent>(TEXT("EquipmentComponent"));
}

void AApocalypseCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority()) SpawnVerityCompanion();
    CreateInventoryWidgetIfNeeded();
    CreateVeritySettingsWidgetIfNeeded();
}

void AApocalypseCharacter::OnRep_Controller()
{
    Super::OnRep_Controller();
    CreateInventoryWidgetIfNeeded();
    CreateVeritySettingsWidgetIfNeeded();
}

void AApocalypseCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    CreateInventoryWidgetIfNeeded();
    CreateVeritySettingsWidgetIfNeeded();
}

void AApocalypseCharacter::SpawnVerityCompanion()
{
    if (!HasAuthority() || !bEnableVerityCompanion || VerityCompanion || !GetWorld()) return;
    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    const FVector SpawnLocation = GetActorLocation() + GetActorRightVector() * 180.0f + FVector(0, 0, 20.0f);
    VerityCompanion = GetWorld()->SpawnActor<AVerityCompanionCharacter>(
        AVerityCompanionCharacter::StaticClass(), SpawnLocation, GetActorRotation(), Params);
    if (!VerityCompanion) return;
    VerityCompanion->SetPlayerTarget(this);
    ForceNetUpdate();
    OnRep_VerityCompanion();
}

void AApocalypseCharacter::OnRep_VerityCompanion()
{
    CreateVeritySettingsWidgetIfNeeded();
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

void AApocalypseCharacter::CreateVeritySettingsWidgetIfNeeded()
{
    if (VeritySettingsWidget || !IsLocallyControlled() || !IsValid(VerityCompanion)) return;
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController) return;
    VeritySettingsWidget = CreateWidget<UVeritySettingsWidget>(PlayerController, UVeritySettingsWidget::StaticClass());
    if (!VeritySettingsWidget) return;
    VeritySettingsWidget->AddToViewport(30);
    VeritySettingsWidget->SetCompanion(VerityCompanion);
}

void AApocalypseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AApocalypseCharacter, VerityCompanion);
}
