#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VeritySettingsWidget.generated.h"

class AVerityCompanionCharacter;
class UCheckBox;
class USlider;
class UEditableTextBox;
class UButton;
class UTextBlock;

UCLASS()
class APOCALYPSE_API UVeritySettingsWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    void SetCompanion(AVerityCompanionCharacter* InCompanion);
protected:
    virtual void NativeConstruct() override;
private:
    UPROPERTY(Transient) TObjectPtr<AVerityCompanionCharacter> Companion;
    UPROPERTY(Transient) TObjectPtr<UCheckBox> CompanionToggle;
    UPROPERTY(Transient) TObjectPtr<UCheckBox> AutoTalkToggle;
    UPROPERTY(Transient) TObjectPtr<UCheckBox> VoiceToggle;
    UPROPERTY(Transient) TObjectPtr<USlider> VolumeSlider;
    UPROPERTY(Transient) TObjectPtr<UEditableTextBox> MessageInput;
    UPROPERTY(Transient) TObjectPtr<UButton> SendButton;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> StatusText;
    UFUNCTION() void OnCompanionToggled(bool bChecked);
    UFUNCTION() void OnAutoTalkToggled(bool bChecked);
    UFUNCTION() void OnVoiceToggled(bool bChecked);
    UFUNCTION() void OnVolumeChanged(float Value);
    UFUNCTION() void OnSendClicked();
    UFUNCTION() void OnDialogueLine(const FString& Line);
    void UpdateStatus(const FString& Status);
};
