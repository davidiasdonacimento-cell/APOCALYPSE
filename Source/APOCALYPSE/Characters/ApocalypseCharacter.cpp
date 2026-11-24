#include "Characters/ApocalypseCharacter.h"
#include "Gameplay/ApocalypseInteractionComponent.h"
#include "Gameplay/ApocalypseInventoryComponent.h"
#include "Gameplay/ApocalypseEquipmentComponent.h"
#include "Gameplay/ApocalypseSurvivalComponent.h"
#include "UI/ApocalypseInventoryWidget.h"
#include "UI/VeritySettingsWidget.h"
#include "Verity/VerityCompanionCharacter.h"
#include "Verity/VerityDialogueSystem.h"
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
    BindVerityGameEvents();
    CreateInventoryWidgetIfNeeded();
    CreateVeritySettingsWidgetIfNeeded();
}
void AApocalypseCharacter::OnRep_Controller()
{
    Super::OnRep_Controller();
    BindVerityGameEvents();
    CreateInventoryWidgetIfNeeded();
    CreateVeritySettingsWidgetIfNeeded();
}
void AApocalypseCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    BindVerityGameEvents();
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
    VerityCompanion = GetWorld()->SpawnActor<AVerityCompanionCharacter>(AVerityCompanionCharacter::StaticClass(), SpawnLocation, GetActorRotation(), Params);
    if (!VerityCompanion) return;
    VerityCompanion->SetPlayerTarget(this);
    ForceNetUpdate();
    OnRep_VerityCompanion();
}
void AApocalypseCharacter::OnRep_VerityCompanion()
{
    BindVerityGameEvents();
    CreateVeritySettingsWidgetIfNeeded();
}
void AApocalypseCharacter::BindVerityGameEvents()
{
    if (!IsLocallyControlled() || !IsValid(VerityCompanion)) return;
    if (!bVerityEventDelegatesBound)
    {
        if (SurvivalComponent) SurvivalComponent->OnSurvivalChanged.AddUniqueDynamic(this, &AApocalypseCharacter::HandleSurvivalChanged);
        if (InventoryComponent) InventoryComponent->OnInventoryChanged.AddUniqueDynamic(this, &AApocalypseCharacter::HandleInventoryChanged);
        if (EquipmentComponent) EquipmentComponent->OnEquipmentChanged.AddUniqueDynamic(this, &AApocalypseCharacter::HandleEquipmentChanged);
        bVerityEventDelegatesBound = true;
    }
    if (SurvivalComponent)
    {
        const float Health = SurvivalComponent->GetHealth();
        const float Hunger = SurvivalComponent->GetHunger();
        const float Thirst = SurvivalComponent->GetThirst();
        LastHealthBand = Health <= 0.0f ? 0 : (Health <= 25.0f ? 1 : (Health <= 50.0f ? 2 : 3));
        LastHungerBand = Hunger <= 15.0f ? 1 : (Hunger <= 35.0f ? 2 : 3);
        LastThirstBand = Thirst <= 15.0f ? 1 : (Thirst <= 35.0f ? 2 : 3);
        bVeritySurvivalBaselineInitialized = true;
    }
}
void AApocalypseCharacter::HandleSurvivalChanged()
{
    if (!IsLocallyControlled() || !IsValid(VerityCompanion) || !SurvivalComponent) return;
    const float Health = SurvivalComponent->GetHealth();
    const float Hunger = SurvivalComponent->GetHunger();
    const float Thirst = SurvivalComponent->GetThirst();
    const int32 HealthBand = Health <= 0.0f ? 0 : (Health <= 25.0f ? 1 : (Health <= 50.0f ? 2 : 3));
    const int32 HungerBand = Hunger <= 15.0f ? 1 : (Hunger <= 35.0f ? 2 : 3);
    const int32 ThirstBand = Thirst <= 15.0f ? 1 : (Thirst <= 35.0f ? 2 : 3);
    if (!bVeritySurvivalBaselineInitialized)
    {
        LastHealthBand = HealthBand; LastHungerBand = HungerBand; LastThirstBand = ThirstBand;
        bVeritySurvivalBaselineInitialized = true; return;
    }
    FString EventName;
    if (HealthBand < LastHealthBand) EventName = HealthBand <= 1 ? TEXT("health_critical") : TEXT("health_low");
    else if (HungerBand < LastHungerBand) EventName = TEXT("hunger_low");
    else if (ThirstBand < LastThirstBand) EventName = TEXT("thirst_low");
    LastHealthBand = HealthBand; LastHungerBand = HungerBand; LastThirstBand = ThirstBand;
    if (!EventName.IsEmpty() && VerityCompanion->GetDialogueSystem())
        VerityCompanion->GetDialogueSystem()->NotifyGameEvent(EventName,
            FString::Printf(TEXT("Sobrevivência confirmada: saúde %.0f%%, fome %.0f%% e sede %.0f%%."), Health, Hunger, Thirst));
}
void AApocalypseCharacter::HandleInventoryChanged()
{
    if (!IsLocallyControlled() || !IsValid(VerityCompanion) || !VerityCompanion->GetDialogueSystem()) return;
    const int32 Count = InventoryComponent ? InventoryComponent->GetItems().Num() : 0;
    VerityCompanion->GetDialogueSystem()->NotifyGameEvent(TEXT("inventory_changed"),
        FString::Printf(TEXT("O inventário foi atualizado e possui %d pilhas ocupadas."), Count));
}
void AApocalypseCharacter::HandleEquipmentChanged()
{
    if (!IsLocallyControlled() || !IsValid(VerityCompanion) || !VerityCompanion->GetDialogueSystem()) return;
    const int32 Count = EquipmentComponent ? EquipmentComponent->GetEquippedItems().Num() : 0;
    VerityCompanion->GetDialogueSystem()->NotifyGameEvent(TEXT("equipment_changed"),
        FString::Printf(TEXT("O equipamento foi atualizado. Há %d espaços equipados."), Count));
}
void AApocalypseCharacter::CreateInventoryWidgetIfNeeded()
{
    if (InventoryWidget || !IsLocallyControlled()) return;
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController) return;
    const TSubclassOf<UApocalypseInventoryWidget> WidgetClass = InventoryWidgetClass ? InventoryWidgetClass : UApocalypseInventoryWidget::StaticClass();
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