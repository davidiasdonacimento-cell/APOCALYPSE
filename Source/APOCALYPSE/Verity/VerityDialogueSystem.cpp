#include "Verity/VerityDialogueSystem.h"
#include "Verity/VerityMemorySystem.h"
#include "Verity/VerityPersonalitySystem.h"
#include "Verity/VerityVoiceSystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Dom/JsonObject.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

UVerityDialogueSystem::UVerityDialogueSystem()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UVerityDialogueSystem::Configure(const FString& InLocalAIUrl, const FString& InLocalAIModel, UVerityMemorySystem* InMemory, UVerityPersonalitySystem* InPersonality, UVerityVoiceSystem* InVoice)
{
    OptionalLocalAIUrl = InLocalAIUrl.TrimStartAndEnd();
    while (OptionalLocalAIUrl.EndsWith(TEXT("/"))) OptionalLocalAIUrl.LeftChopInline(1);
    LocalAIModel = InLocalAIModel.IsEmpty() ? TEXT("qwen2.5:1.5b") : InLocalAIModel;
    Memory = InMemory;
    Personality = InPersonality;
    Voice = InVoice;
}

void UVerityDialogueSystem::BeginPlay()
{
    Super::BeginPlay();
    const APawn* PlayerOwner = GetOwner() ? Cast<APawn>(GetOwner()->GetOwner()) : nullptr;
    if (PlayerOwner && PlayerOwner->IsLocallyControlled()) ScheduleNextConversation(18.0f);
}

void UVerityDialogueSystem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(ConversationTimer);
    Super::EndPlay(EndPlayReason);
}

float UVerityDialogueSystem::NextConversationDelay() const { return FMath::FRandRange(48.0f, 78.0f); }

void UVerityDialogueSystem::ScheduleNextConversation(float Delay)
{
    if (GetWorld()) GetWorld()->GetTimerManager().SetTimer(ConversationTimer, this, &UVerityDialogueSystem::StartAutomaticConversation, Delay, false);
}

void UVerityDialogueSystem::StartAutomaticConversation()
{
    if (!bAutomaticConversation) { ScheduleNextConversation(NextConversationDelay()); return; }
    if (bCombatActive) { ScheduleNextConversation(12.0f); return; }
    RequestDialogue(TEXT("ambient"), FString());
    ScheduleNextConversation(NextConversationDelay());
}

void UVerityDialogueSystem::SetAutomaticConversationEnabled(bool bEnabled)
{
    bAutomaticConversation = bEnabled;
    if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(ConversationTimer);
    if (bEnabled) ScheduleNextConversation(8.0f);
}

void UVerityDialogueSystem::SetCombatActive(bool bActive)
{
    if (GetOwner() && GetOwner()->HasAuthority()) bCombatActive = bActive;
    if (bActive && Voice.IsValid()) Voice->StopSpeaking();
}

void UVerityDialogueSystem::OnRep_CombatActive()
{
    if (bCombatActive && Voice.IsValid()) Voice->StopSpeaking();
}

void UVerityDialogueSystem::SendPlayerMessage(const FString& Message)
{
    const FString Clean = Message.TrimStartAndEnd();
    if (Clean.IsEmpty() || bRequestPending) return;
    if (Memory.IsValid()) Memory->Remember(FString::Printf(TEXT("O jogador disse: %s"), *Clean.Left(240)));
    RequestDialogue(TEXT("player_message"), Clean.Left(500));
}

void UVerityDialogueSystem::RequestDialogue(const FString& Trigger, const FString& PlayerMessage)
{
    if (bRequestPending) return;
    if (OptionalLocalAIUrl.IsEmpty())
    {
        DeliverLine(BuildOfflineLine(Trigger, PlayerMessage), true);
        return;
    }

    TSharedRef<FJsonObject> SystemMessage = MakeShared<FJsonObject>();
    SystemMessage->SetStringField(TEXT("role"), TEXT("system"));
    SystemMessage->SetStringField(TEXT("content"), Personality.IsValid() ? Personality->BuildSystemPrompt() :
        TEXT("Você é VERITY, companheira do APOCALYPSE. Fale em português brasileiro e use apenas fatos do contexto."));

    const FString Context = Memory.IsValid() ? Memory->BuildContext(12) : FString();
    TSharedRef<FJsonObject> UserMessage = MakeShared<FJsonObject>();
    UserMessage->SetStringField(TEXT("role"), TEXT("user"));
    UserMessage->SetStringField(TEXT("content"), FString::Printf(
        TEXT("Evento: %s\nMemória recente: %s\nMensagem do jogador: %s\nResponda com uma fala curta e natural."),
        *Trigger, *Context, PlayerMessage.IsEmpty() ? TEXT("(inicie uma conversa espontânea)") : *PlayerMessage));

    TArray<TSharedPtr<FJsonValue>> Messages;
    Messages.Add(MakeShared<FJsonValueObject>(SystemMessage));
    Messages.Add(MakeShared<FJsonValueObject>(UserMessage));
    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("model"), LocalAIModel);
    Payload->SetArrayField(TEXT("messages"), Messages);
    Payload->SetNumberField(TEXT("temperature"), 0.75);
    Payload->SetNumberField(TEXT("max_tokens"), 110);

    FString Body;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
    FJsonSerializer::Serialize(Payload, Writer);
    bRequestPending = true;

    TWeakObjectPtr<UVerityDialogueSystem> WeakThis(this);
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(OptionalLocalAIUrl + TEXT("/v1/chat/completions"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetTimeout(18.0f);
    Request->SetContentAsString(Body);
    Request->OnProcessRequestComplete().BindLambda([WeakThis, Trigger, PlayerMessage](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bConnected)
    {
        UVerityDialogueSystem* Self = WeakThis.Get();
        if (!Self) return;
        Self->bRequestPending = false;
        FString Reply;
        if (bConnected && Response.IsValid() && Response->GetResponseCode() == 200)
        {
            TSharedPtr<FJsonObject> Root;
            const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
            if (FJsonSerializer::Deserialize(Reader, Root) && Root.IsValid())
            {
                const TArray<TSharedPtr<FJsonValue>>* Choices = nullptr;
                if (Root->TryGetArrayField(TEXT("choices"), Choices) && Choices && Choices->Num() > 0)
                {
                    const TSharedPtr<FJsonObject> Choice = (*Choices)[0]->AsObject();
                    const TSharedPtr<FJsonObject> MessageObject = Choice.IsValid() ? Choice->GetObjectField(TEXT("message")) : nullptr;
                    if (MessageObject.IsValid()) MessageObject->TryGetStringField(TEXT("content"), Reply);
                }
            }
        }
        if (Reply.TrimStartAndEnd().IsEmpty()) Reply = Self->BuildOfflineLine(Trigger, PlayerMessage);
        Self->DeliverLine(Reply.Left(500), true);
    });
    if (!Request->ProcessRequest())
    {
        bRequestPending = false;
        DeliverLine(BuildOfflineLine(Trigger, PlayerMessage), true);
    }
}

FString UVerityDialogueSystem::BuildOfflineLine(const FString& Trigger, const FString& PlayerMessage) const
{
    const FString Message = PlayerMessage.ToLower();
    const FString Context = Memory.IsValid() ? Memory->BuildContext(5).ToLower() : FString();
    if (!PlayerMessage.IsEmpty())
    {
        if (Message.Contains(TEXT("oi")) || Message.Contains(TEXT("olá")) || Message.Contains(TEXT("ola")))
            return TEXT("Oi! Estou aqui. Vamos continuar explorando juntos.");
        if (Message.Contains(TEXT("obrigad"))) return TEXT("Não precisa agradecer. Somos uma equipe.");
        if (Message.Contains(TEXT("ajuda")) || Message.Contains(TEXT("medo"))) return TEXT("Estou ao seu lado. Vamos com calma e prestar atenção ao caminho.");
        if (Message.Contains(TEXT("zumbi")) || Message.Contains(TEXT("inimigo"))) return TEXT("Vou ficar atenta aos arredores. Se aparecer uma ameaça, vou reagir.");
        if (Message.Contains(TEXT("como você")) || Message.Contains(TEXT("como voce"))) return TEXT("Estou bem e focada. E você, pronto para continuar?");
        return TEXT("Entendi o que você disse. Vou levar isso em conta enquanto exploramos.");
    }
    if (Context.Contains(TEXT("ameaça")) || Context.Contains(TEXT("movimento")))
        return TEXT("Ainda estou observando os arredores. Não vamos baixar a guarda.");
    if (Trigger == TEXT("ambient"))
        return Personality.IsValid() ? Personality->GetOfflineLine(Personality->GetMood()) : TEXT("Estou aqui com você. Vamos continuar.");
    return TEXT("Estou acompanhando tudo. Vamos seguir juntos.");
}

void UVerityDialogueSystem::DeliverLine(const FString& Line, bool bSpeak)
{
    if (Line.TrimStartAndEnd().IsEmpty()) return;
    LastLine = Line;
    if (Memory.IsValid()) Memory->Remember(FString::Printf(TEXT("VERITY comentou: %s"), *Line.Left(220)));
    OnDialogueLine.Broadcast(LastLine);
    if (bSpeak && Voice.IsValid()) Voice->Speak(LastLine);
}

void UVerityDialogueSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UVerityDialogueSystem, bCombatActive);
}
