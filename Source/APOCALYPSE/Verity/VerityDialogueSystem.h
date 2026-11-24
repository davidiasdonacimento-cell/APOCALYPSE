#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VerityDialogueSystem.generated.h"

class UVerityMemorySystem;
class UVerityPersonalitySystem;
class UVerityVoiceSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVerityDialogueLine, const FString&, Line);

UCLASS(ClassGroup=(VERITY), meta=(BlueprintSpawnableComponent))
class APOCALYPSE_API UVerityDialogueSystem : public UActorComponent
{
    GENERATED_BODY()
public:
    UVerityDialogueSystem();
    UPROPERTY(BlueprintAssignable, Category="VERITY|Dialogue") FOnVerityDialogueLine OnDialogueLine;
    void Configure(const FString& Url, UVerityMemorySystem* InMemory, UVerityPersonalitySystem* InPersonality, UVerityVoiceSystem* InVoice);
    UFUNCTION(BlueprintCallable, Category="VERITY|Dialogue") void SendPlayerMessage(const FString& Message);
    UFUNCTION(BlueprintCallable, Category="VERITY|Dialogue") void SetAutomaticConversationEnabled(bool bEnabled);
    UFUNCTION(BlueprintCallable, Category="VERITY|Dialogue") void SetCombatActive(bool bActive);
    UFUNCTION(BlueprintPure, Category="VERITY|Dialogue") bool IsAutomaticConversationEnabled() const { return bAutomaticConversation; }
    UFUNCTION(BlueprintPure, Category="VERITY|Dialogue") FString GetLastLine() const { return LastLine; }
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
    FString BackendBaseUrl;
    TWeakObjectPtr<UVerityMemorySystem> Memory;
    TWeakObjectPtr<UVerityPersonalitySystem> Personality;
    TWeakObjectPtr<UVerityVoiceSystem> Voice;
    FTimerHandle ConversationTimer;
    FString LastLine;
    bool bAutomaticConversation = true;
    UPROPERTY(ReplicatedUsing=OnRep_CombatActive) bool bCombatActive = false;
    bool bRequestPending = false;
    UFUNCTION() void OnRep_CombatActive();
    float NextConversationDelay() const;
    void ScheduleNextConversation(float Delay);
    void StartAutomaticConversation();
    void RequestDialogue(const FString& Trigger, const FString& PlayerMessage);
    void DeliverLine(const FString& Line, bool bSpeak);
    FString BuildOfflineLine();
};
