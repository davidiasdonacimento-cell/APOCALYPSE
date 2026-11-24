#include "UI/VeritySettingsWidget.h"
#include "Verity/VerityCompanionCharacter.h"
#include "Verity/VerityDialogueSystem.h"
#include "Verity/VerityVoiceSystem.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Blueprint/WidgetTree.h"

void UVeritySettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (!WidgetTree) return;
    UVerticalBox* Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
    WidgetTree->RootWidget = Root;

    UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Title->SetText(FText::FromString(TEXT("VERITY • COMPANHEIRA IA")));
    Root->AddChild(Title);

    auto AddToggle = [this, Root](const TCHAR* Label, UCheckBox*& OutCheckBox)
    {
        UTextBlock* LabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        LabelText->SetText(FText::FromString(Label));
        Root->AddChild(LabelText);
        OutCheckBox = WidgetTree->ConstructWidget<UCheckBox>(UCheckBox::StaticClass());
        Root->AddChild(OutCheckBox);
    };
    AddToggle(TEXT("Companheira ativada"), CompanionToggle);
    AddToggle(TEXT("Conversas espontâneas"), AutoTalkToggle);
    AddToggle(TEXT("Voz em português"), VoiceToggle);

    UTextBlock* VolumeLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    VolumeLabel->SetText(FText::FromString(TEXT("Volume da voz")));
    Root->AddChild(VolumeLabel);
    VolumeSlider = WidgetTree->ConstructWidget<USlider>(USlider::StaticClass());
    VolumeSlider->SetValue(0.85f);
    Root->AddChild(VolumeSlider);

    MessageInput = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
    MessageInput->SetHintText(FText::FromString(TEXT("Escreva para conversar com a VERITY...")));
    Root->AddChild(MessageInput);
    SendButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
    UTextBlock* SendLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    SendLabel->SetText(FText::FromString(TEXT("Enviar mensagem")));
    SendButton->AddChild(SendLabel);
    Root->AddChild(SendButton);
    StatusText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Root->AddChild(StatusText);

    CompanionToggle->OnCheckStateChanged.AddDynamic(this, &UVeritySettingsWidget::OnCompanionToggled);
    AutoTalkToggle->OnCheckStateChanged.AddDynamic(this, &UVeritySettingsWidget::OnAutoTalkToggled);
    VoiceToggle->OnCheckStateChanged.AddDynamic(this, &UVeritySettingsWidget::OnVoiceToggled);
    VolumeSlider->OnValueChanged.AddDynamic(this, &UVeritySettingsWidget::OnVolumeChanged);
    SendButton->OnClicked.AddDynamic(this, &UVeritySettingsWidget::OnSendClicked);
    UpdateStatus(TEXT("Aguardando VERITY..."));
}

void UVeritySettingsWidget::NativeDestruct()
{
    if (Companion && Companion->GetDialogueSystem())
        Companion->GetDialogueSystem()->OnDialogueLine.RemoveDynamic(this, &UVeritySettingsWidget::OnDialogueLine);
    Super::NativeDestruct();
}

void UVeritySettingsWidget::SetCompanion(AVerityCompanionCharacter* InCompanion)
{
    Companion = InCompanion;
    if (!Companion || !CompanionToggle || !AutoTalkToggle || !VoiceToggle) return;
    CompanionToggle->SetIsChecked(Companion->IsCompanionEnabled());
    AutoTalkToggle->SetIsChecked(Companion->GetDialogueSystem()->IsAutomaticConversationEnabled());
    VoiceToggle->SetIsChecked(Companion->GetVoiceSystem()->IsVoiceEnabled());
    Companion->GetDialogueSystem()->OnDialogueLine.AddUniqueDynamic(this, &UVeritySettingsWidget::OnDialogueLine);
    UpdateStatus(TEXT("VERITY conectada."));
}

void UVeritySettingsWidget::OnCompanionToggled(bool bChecked)
{
    if (Companion) Companion->SetCompanionEnabled(bChecked);
    UpdateStatus(bChecked ? TEXT("VERITY ativada.") : TEXT("VERITY desativada."));
}
void UVeritySettingsWidget::OnAutoTalkToggled(bool bChecked)
{
    if (Companion) Companion->GetDialogueSystem()->SetAutomaticConversationEnabled(bChecked);
}
void UVeritySettingsWidget::OnVoiceToggled(bool bChecked)
{
    if (Companion) Companion->GetVoiceSystem()->SetVoiceEnabled(bChecked);
}
void UVeritySettingsWidget::OnVolumeChanged(float Value)
{
    if (Companion) Companion->GetVoiceSystem()->SetVoiceVolume(Value);
}
void UVeritySettingsWidget::OnSendClicked()
{
    if (!Companion || !MessageInput) return;
    const FString Message = MessageInput->GetText().ToString();
    Companion->GetDialogueSystem()->SendPlayerMessage(Message);
    MessageInput->SetText(FText::GetEmpty());
}
void UVeritySettingsWidget::OnDialogueLine(const FString& Line) { UpdateStatus(Line); }
void UVeritySettingsWidget::UpdateStatus(const FString& Status)
{
    if (StatusText) StatusText->SetText(FText::FromString(Status.Left(160)));
}
