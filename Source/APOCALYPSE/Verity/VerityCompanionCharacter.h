#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VerityCompanionCharacter.generated.h"

class UVerityCompanionAI;
class UVerityCombatSystem;
class UVerityNavigationSystem;
class UVerityMemorySystem;
class UVerityPersonalitySystem;
class UVerityDialogueSystem;
class UVerityVoiceSystem;
class USkeletalMesh;

UCLASS(config=Game)
class APOCALYPSE_API AVerityCompanionCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AVerityCompanionCharacter();
    void SetPlayerTarget(AActor* Player);
    UFUNCTION(BlueprintPure, Category="VERITY") UVerityDialogueSystem* GetDialogueSystem() const { return DialogueSystem; }
    UFUNCTION(BlueprintPure, Category="VERITY") UVerityVoiceSystem* GetVoiceSystem() const { return VoiceSystem; }
    UFUNCTION(BlueprintPure, Category="VERITY") UVerityNavigationSystem* GetNavigationSystem() const { return NavigationSystem; }
    UFUNCTION(BlueprintCallable, Category="VERITY") void SetCompanionEnabled(bool bEnabled);
    UFUNCTION(BlueprintPure, Category="VERITY") bool IsCompanionEnabled() const { return bCompanionEnabled; }
protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category="VERITY|AI", meta=(ToolTip="Optional OpenAI-compatible endpoint hosted locally on your own network. Leave empty for offline mode.")) FString LocalAIBaseUrl;
    UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category="VERITY|AI") FString LocalAIModel = TEXT("qwen2.5:1.5b");
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VERITY") TObjectPtr<UVerityCompanionAI> CompanionAI;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VERITY") TObjectPtr<UVerityCombatSystem> CombatSystem;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VERITY") TObjectPtr<UVerityNavigationSystem> NavigationSystem;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VERITY") TObjectPtr<UVerityMemorySystem> MemorySystem;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VERITY") TObjectPtr<UVerityPersonalitySystem> PersonalitySystem;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VERITY") TObjectPtr<UVerityDialogueSystem> DialogueSystem;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VERITY") TObjectPtr<UVerityVoiceSystem> VoiceSystem;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VERITY|Visual") TObjectPtr<USkeletalMesh> VerityMesh;
private:
    UPROPERTY(ReplicatedUsing=OnRep_CompanionEnabled) bool bCompanionEnabled = true;
    UFUNCTION() void OnRep_CompanionEnabled();
    UFUNCTION(Server, Reliable) void ServerSetCompanionEnabled(bool bEnabled);
    void ApplyCompanionEnabled();
};
