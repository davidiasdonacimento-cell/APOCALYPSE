#include "UI/MainLobbyWidget.h"
#include "Verity/VerityChatSubsystem.h"
#include "Game/ApocalypseGameMode.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Spacer.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UMainLobbyWidget::NativeConstruct()
{
    Super::NativeConstruct();
    Chat = GetGameInstance() ? GetGameInstance()->GetSubsystem<UVerityChatSubsystem>() : nullptr;
    if (Chat)
    {
        Chat->OnChatUpdated.AddUniqueDynamic(this, &UMainLobbyWidget::OnChatUpdated);
        Chat->OnGenerationStateChanged.AddUniqueDynamic(this, &UMainLobbyWidget::OnGenerationChanged);
    }
    if (!WidgetTree) return;
    UVerticalBox* Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
    WidgetTree->RootWidget = Root;
    UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Title->SetText(FText::FromString(TEXT("APOCALYPSE  |  LOBBY PRINCIPAL")));
    Root->AddChild(Title);
    UTextBlock* Subtitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Subtitle->SetText(FText::FromString(TEXT("A partida só começa quando você selecionar INICIAR JOGO.")));
    Root->AddChild(Subtitle);
    UHorizontalBox* Menu = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
    Root->AddChild(Menu);
    auto AddMenuButton = [this, Menu](const TCHAR* Label, FOnButtonClickedEvent::FDelegate Callback) -> UButton*
    {
        UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
        UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        Text->SetText(FText::FromString(Label)); Button->AddChild(Text); Button->OnClicked.Add(Callback); Menu->AddChild(Button); return Button;
    };
    StartButton = AddMenuButton(TEXT("INICIAR JOGO"), FOnButtonClickedEvent::FDelegate::CreateUObject(this, &UMainLobbyWidget::OnStartClicked));
    ContinueButton = AddMenuButton(TEXT("CONTINUAR"), FOnButtonClickedEvent::FDelegate::CreateUObject(this, &UMainLobbyWidget::OnContinueClicked));
    AddMenuButton(TEXT("CONFIGURAÇÕES"), FOnButtonClickedEvent::FDelegate::CreateLambda([this]() { SetStatus(TEXT("Configurações de voz e IA: use os controles da área VERITY. Configurações avançadas serão adicionadas quando o sistema de opções do projeto existir.")); }));
    AddMenuButton(TEXT("PERSONALIZAR"), FOnButtonClickedEvent::FDelegate::CreateLambda([this]() { SetStatus(TEXT("Personalização de personagem depende dos modelos e cosméticos que ainda serão adicionados ao projeto.")); }));
    if (ContinueButton) ContinueButton->SetVisibility(Chat && Chat->HasCampaignProgress() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    UTextBlock* VerityTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    VerityTitle->SetText(FText::FromString(TEXT("VERITY  •  CONVERSA")));
    Root->AddChild(VerityTitle);
    ChatHistoryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    ChatHistoryText->SetAutoWrapText(true);
    Root->AddChild(ChatHistoryText);
    MessageInput = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
    MessageInput->SetHintText(FText::FromString(TEXT("Escreva uma mensagem para a VERITY...")));
    Root->AddChild(MessageInput);
    UHorizontalBox* ChatActions = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
    Root->AddChild(ChatActions);
    auto AddChatButton = [this, ChatActions](const TCHAR* Label, FOnButtonClickedEvent::FDelegate Callback) -> UButton*
    {
        UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
        UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        Text->SetText(FText::FromString(Label)); Button->AddChild(Text); Button->OnClicked.Add(Callback); ChatActions->AddChild(Button); return Button;
    };
    SendButton = AddChatButton(TEXT("ENVIAR"), FOnButtonClickedEvent::FDelegate::CreateUObject(this, &UMainLobbyWidget::OnSendClicked));
    EditLastButton = AddChatButton(TEXT("EDITAR MINHA ÚLTIMA MENSAGEM"), FOnButtonClickedEvent::FDelegate::CreateUObject(this, &UMainLobbyWidget::OnEditLastClicked));
    ClearChatButton = AddChatButton(TEXT("LIMPAR CONVERSA"), FOnButtonClickedEvent::FDelegate::CreateUObject(this, &UMainLobbyWidget::OnClearChatClicked));
    UTextBlock* PersonalTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    PersonalTitle->SetText(FText::FromString(TEXT("MEU TEXTO PESSOAL (OPCIONAL)")));
    Root->AddChild(PersonalTitle);
    PersonalContextInput = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
    PersonalContextInput->SetHintText(FText::FromString(TEXT("Escreva um contexto que a VERITY deve considerar. Pode deixar vazio.")));
    if (Chat) PersonalContextInput->SetText(FText::FromString(Chat->GetPersonalContext()));
    Root->AddChild(PersonalContextInput);
    SaveContextButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
    UTextBlock* SaveContextLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    SaveContextLabel->SetText(FText::FromString(TEXT("SALVAR TEXTO PESSOAL"))); SaveContextButton->AddChild(SaveContextLabel);
    SaveContextButton->OnClicked.AddDynamic(this, &UMainLobbyWidget::OnSaveContextClicked); Root->AddChild(SaveContextButton);
    StatusText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass()); Root->AddChild(StatusText);
    RefreshChat();
    SetStatus(TEXT("Lobby pronto. A VERITY está aguardando sua mensagem."));
}
void UMainLobbyWidget::NativeDestruct()
{
    if (Chat) { Chat->OnChatUpdated.RemoveDynamic(this, &UMainLobbyWidget::OnChatUpdated); Chat->OnGenerationStateChanged.RemoveDynamic(this, &UMainLobbyWidget::OnGenerationChanged); }
    Super::NativeDestruct();
}
void UMainLobbyWidget::OnSendClicked()
{
    if (!Chat || !MessageInput) return;
    const FString Message = MessageInput->GetText().ToString();
    if (Message.TrimStartAndEnd().IsEmpty()) return;
    Chat->SendMessage(Message); MessageInput->SetText(FText::GetEmpty());
    SetStatus(TEXT("Mensagem enviada. Aguardando resposta..."));
}
void UMainLobbyWidget::OnEditLastClicked()
{
    if (!Chat || !MessageInput) return;
    const FString Replacement = MessageInput->GetText().ToString();
    if (Replacement.TrimStartAndEnd().IsEmpty()) { SetStatus(TEXT("Digite a versão corrigida da mensagem no campo acima e toque novamente em EDITAR.")); return; }
    Chat->EditLastPlayerMessage(Replacement); MessageInput->SetText(FText::GetEmpty());
}
void UMainLobbyWidget::OnSaveContextClicked()
{
    if (!Chat || !PersonalContextInput) return;
    Chat->SetPersonalContext(PersonalContextInput->GetText().ToString());
    SetStatus(TEXT("Texto pessoal salvo neste perfil local. Você pode alterá-lo ou apagar tudo."));
}
void UMainLobbyWidget::OnClearChatClicked()
{
    if (!Chat) return;
    if (!bAwaitingClearConfirmation)
    {
        bAwaitingClearConfirmation = true;
        SetStatus(TEXT("Confirmação: toque novamente em LIMPAR CONVERSA para apagar o histórico salvo."));
        return;
    }
    bAwaitingClearConfirmation = false;
    Chat->ClearHistoryAfterConfirmation(true);
    SetStatus(TEXT("Histórico desta conversa foi limpo."));
}
void UMainLobbyWidget::OnStartClicked() { StartGame(false); }
void UMainLobbyWidget::OnContinueClicked() { StartGame(true); }
void UMainLobbyWidget::StartGame(bool bContinue)
{
    if (AApocalypseGameMode* Mode = GetWorld() ? Cast<AApocalypseGameMode>(GetWorld()->GetAuthGameMode()) : nullptr)
    {
        Mode->StartGameForPlayer(GetOwningPlayer(), bContinue);
    }
    else SetStatus(TEXT("Não foi possível iniciar: GameMode do APOCALYPSE não encontrado."));
}
void UMainLobbyWidget::OnChatUpdated() { RefreshChat(); }
void UMainLobbyWidget::OnGenerationChanged(bool bGenerating) { SetStatus(bGenerating ? TEXT("VERITY está pensando...") : TEXT("Resposta recebida.")); }
void UMainLobbyWidget::RefreshChat()
{
    if (!ChatHistoryText) return;
    ChatHistoryText->SetText(FText::FromString(Chat && Chat->HasConversation() ? Chat->GetFormattedHistory() : TEXT("VERITY: Oi! Estou no lobby. Podemos conversar antes de começar a partida.")));
}
void UMainLobbyWidget::SetStatus(const FString& Message)
{
    if (StatusText) StatusText->SetText(FText::FromString(Message));
}