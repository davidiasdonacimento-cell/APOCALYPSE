#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainLobbyWidget.generated.h"
class UVerityChatSubsystem;
class UVerticalBox;
class UTextBlock;
class UEditableTextBox;
class UButton;
class UBorder;
UCLASS()
class APOCALYPSE_API UMainLobbyWidget : public UUserWidget
{
    GENERATED_BODY()
protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
private:
    UPROPERTY(Transient) TObjectPtr<UVerityChatSubsystem> Chat;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> ChatHistoryText;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> StatusText;
    UPROPERTY(Transient) TObjectPtr<UEditableTextBox> MessageInput;
    UPROPERTY(Transient) TObjectPtr<UEditableTextBox> PersonalContextInput;
    UPROPERTY(Transient) TObjectPtr<UButton> SendButton;
    UPROPERTY(Transient) TObjectPtr<UButton> EditLastButton;
    UPROPERTY(Transient) TObjectPtr<UButton> SaveContextButton;
    UPROPERTY(Transient) TObjectPtr<UButton> ClearChatButton;
    UPROPERTY(Transient) TObjectPtr<UButton> StartButton;
    UPROPERTY(Transient) TObjectPtr<UButton> ContinueButton;
    UFUNCTION() void OnSendClicked();
    UFUNCTION() void OnEditLastClicked();
    UFUNCTION() void OnSaveContextClicked();
    UFUNCTION() void OnClearChatClicked();
    UFUNCTION() void OnStartClicked();
    UFUNCTION() void OnContinueClicked();
    UFUNCTION() void OnChatUpdated();
    UFUNCTION() void OnGenerationChanged(bool bGenerating);
    void RefreshChat();
    void SetStatus(const FString& Message);
    void StartGame(bool bContinue);
};