#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VerityProfileSaveGame.h"
#include "VerityChatSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVerityChatUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVerityGenerationStateChanged, bool, bGenerating);

UCLASS(Config=Game)
class APOCALYPSE_API UVerityChatSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    UPROPERTY(BlueprintAssignable, Category="VERITY|Chat") FVerityChatUpdated OnChatUpdated;
    UPROPERTY(BlueprintAssignable, Category="VERITY|Chat") FVerityGenerationStateChanged OnGenerationStateChanged;
    UFUNCTION(BlueprintCallable, Category="VERITY|Chat") void SendMessage(const FString& Message);
    UFUNCTION(BlueprintCallable, Category="VERITY|Chat") void EditLastPlayerMessage(const FString& Replacement);
    UFUNCTION(BlueprintCallable, Category="VERITY|Chat") void SetPersonalContext(const FString& Context);
    UFUNCTION(BlueprintPure, Category="VERITY|Chat") FString GetPersonalContext() const;
    UFUNCTION(BlueprintPure, Category="VERITY|Chat") FString GetFormattedHistory() const;
    UFUNCTION(BlueprintPure, Category="VERITY|Chat") bool HasConversation() const;
    UFUNCTION(BlueprintPure, Category="VERITY|Chat") bool IsGenerating() const { return bGenerating; }
    UFUNCTION(BlueprintCallable, Category="VERITY|Chat") void ClearHistoryAfterConfirmation(bool bConfirmed);
    UFUNCTION(BlueprintCallable, Category="VERITY|Chat") void AddPersistentMemory(const FString& Memory);
    UFUNCTION(BlueprintPure, Category="VERITY|Chat") bool HasCampaignProgress() const;
    UFUNCTION(BlueprintCallable, Category="VERITY|Chat") void SetCampaignProgress(bool bHasProgress);
protected:
    UPROPERTY(Config, EditDefaultsOnly, Category="VERITY|AI", meta=(ToolTip="Optional OpenAI-compatible endpoint on the player's private LAN. Empty means offline fallback.")) FString LocalAIBaseUrl;
    UPROPERTY(Config, EditDefaultsOnly, Category="VERITY|AI") FString LocalAIModel = TEXT("qwen2.5:1.5b");
private:
    UPROPERTY(Transient) TObjectPtr<UVerityProfileSaveGame> Profile;
    FString SaveSlotName = TEXT("APOCALYPSE_LOCAL_PROFILE_0");
    bool bGenerating = false;
    void LoadProfile();
    void SaveProfile();
    void CompleteGeneration(const FString& Reply, const FString& Fallback);
    FString BuildOfflineReply(const FString& Message) const;
    void RequestGenerativeReply(const FString& Message);
};