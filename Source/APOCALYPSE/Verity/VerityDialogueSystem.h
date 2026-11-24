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
    void Configure(const FString& OptionalLocalAIUrl, const FString& InLocalAIModel, UVerityMemorySystem* InMemory, UVerityPersonalitySystem* InPersonality, UVerityVoiceSystem* InVoice);
    UFUNCTION(BlueprintCallable, Category="VERITY|Dialogue") void SendPlayerMessage(const FString& Message);
    UFUNCTION(BlueprintCallable, Category="VERITY|Dialogue") void NotifyGameEvent(const FString& EventName, const FString& Context);
    UFUNCTION(BlueprintCallable, Category="VERITY|Dialogue") void SetAutomaticConversationEnabled(bool bEnabled);
    UFUNCTION(BlueprintCallable, Category="VERITY|Dialogue") void SetCombatActive(bool bActive);
    UFUNCTION(BlueprintPure, Category="VERITY|Dialogue") bool IsAutomaticConversationEnabled() const { return bAutomaticConversation; }
    UFUNCTION(BlueprintPure, Category="VERITY|Dialogue") FString GetLastLine() const { return LastLine; }
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
    FString OptionalLocalAIUrl;
    FString LocalAIModel = TEXT("qwen2.5:1.5b");
    TWeakObjectPtr<UVerityMemorySystem> Memory;
    TWeakObjectPtr<UVerityPersonalitySystem> Personality;
    TWeakObjectPtr<UVerityVoiceSystem> Voice;
    FTimerHandle ConversationTimer;
    FString LastLine;
    FString QueuedEventName;
    FString QueuedEventContext;
    FString LastGameEventName;
    float LastGameEventTime = -1000.0f;
    bool bAutomaticConversation = true;
    UPROPERTY(ReplicatedUsing=OnRep_CombatActive) bool bCombatActive = false;
    bool bRequestPending = false;
    UFUNCTION() void OnRep_CombatActive();
    UFUNCTION(Client, Reliable) void ClientNotifyGameEvent(const FString& EventName, const FString& Context);
    float NextConversationDelay() const;
    void ScheduleNextConversation(float Delay);
    void StartAutomaticConversation();
    void HandleGameEvent(const FString& EventName, const FString& Context);
    void ProcessQueuedGameEvent();
    void RequestDialogue(const FString& Trigger, const FString& PlayerMessage, const FString& EventContext = FString());
    FString BuildLiveGameContext() const;
    void DeliverLine(const FString& Line, bool bSpeak);
    FString BuildOfflineLine(const FString& Trigger, const FString& PlayerMessage) const;
};