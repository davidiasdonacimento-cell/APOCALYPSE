#include "Verity/VerityCompanionCharacter.h"
#include "Verity/VerityAIController.h"
#include "Verity/VerityCompanionAI.h"
#include "Verity/VerityCombatSystem.h"
#include "Verity/VerityNavigationSystem.h"
#include "Verity/VerityMemorySystem.h"
#include "Verity/VerityPersonalitySystem.h"
#include "Verity/VerityDialogueSystem.h"
#include "Verity/VerityVoiceSystem.h"
#include "Net/UnrealNetwork.h"

AVerityCompanionCharacter::AVerityCompanionCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    SetReplicateMovement(true);
    AIControllerClass = AVerityAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    CompanionAI = CreateDefaultSubobject<UVerityCompanionAI>(TEXT("VerityCompanionAI"));
    CombatSystem = CreateDefaultSubobject<UVerityCombatSystem>(TEXT("VerityCombatSystem"));
    NavigationSystem = CreateDefaultSubobject<UVerityNavigationSystem>(TEXT("VerityNavigationSystem"));
    MemorySystem = CreateDefaultSubobject<UVerityMemorySystem>(TEXT("VerityMemorySystem"));
    PersonalitySystem = CreateDefaultSubobject<UVerityPersonalitySystem>(TEXT("VerityPersonalitySystem"));
    DialogueSystem = CreateDefaultSubobject<UVerityDialogueSystem>(TEXT("VerityDialogueSystem"));
    VoiceSystem = CreateDefaultSubobject<UVerityVoiceSystem>(TEXT("VerityVoiceSystem"));
    GetCharacterMovement()->MaxWalkSpeed = 390.0f;
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    bUseControllerRotationYaw = false;
}
void AVerityCompanionCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (VerityMesh) GetMesh()->SetSkeletalMesh(VerityMesh);
    DialogueSystem->Configure(LocalAIBaseUrl, LocalAIModel, MemorySystem, PersonalitySystem, VoiceSystem);
    ApplyCompanionEnabled();
}
void AVerityCompanionCharacter::SetPlayerTarget(AActor* Player)
{
    if (CompanionAI) CompanionAI->SetPlayerTarget(Player);
    if (NavigationSystem) NavigationSystem->SetFollowTarget(Player);
    if (Player) SetOwner(Player);
}
void AVerityCompanionCharacter::SetCompanionEnabled(bool bEnabled)
{
    bCompanionEnabled = bEnabled;
    ApplyCompanionEnabled();
    if (!HasAuthority()) ServerSetCompanionEnabled(bEnabled);
}
void AVerityCompanionCharacter::ServerSetCompanionEnabled_Implementation(bool bEnabled)
{
    bCompanionEnabled = bEnabled;
    ApplyCompanionEnabled();
    ForceNetUpdate();
}
void AVerityCompanionCharacter::ApplyCompanionEnabled()
{
    if (CompanionAI) CompanionAI->SetAIEnabled(bCompanionEnabled);
    if (CombatSystem) CombatSystem->SetCombatEnabled(bCompanionEnabled);
    if (NavigationSystem) NavigationSystem->SetFollowingEnabled(bCompanionEnabled);
    SetActorHiddenInGame(!bCompanionEnabled);
    SetActorEnableCollision(bCompanionEnabled);
    if (!bCompanionEnabled && VoiceSystem) VoiceSystem->StopSpeaking();
}
void AVerityCompanionCharacter::OnRep_CompanionEnabled() { ApplyCompanionEnabled(); }
void AVerityCompanionCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AVerityCompanionCharacter, bCompanionEnabled);
}
