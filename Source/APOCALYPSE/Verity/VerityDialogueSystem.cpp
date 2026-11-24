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

UVerityDialogueSystem::UVerityDialogueSystem() { PrimaryComponentTick.bCanEverTick = false; }
void UVerityDialogueSystem::Configure(const FString& Url, UVerityMemorySystem* InMemory, UVerityPersonalitySystem* InPersonality, UVerityVoiceSystem* InVoice)
{
    BackendBaseUrl = Url.TrimStartAndEnd();
    Memory = InMemory;
    Personality = InPersonality;
    Voice = InVoice;
    if (InVoice) InVoice->SetBackendBaseUrl(BackendBaseUrl);
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
    bCombatActive = bActive;
    if (bActive && Voice.IsValid()) Voice->StopSpeaking();
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
    if (BackendBaseUrl.IsEmpty()) { DeliverLine(BuildOfflineLine(), true); return; }

    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("trigger"), Trigger);
    Payload->SetStringField(TEXT("player_message"), PlayerMessage);
    Payload->SetStringField(TEXT("context"), Memory.IsValid() ? Memory->BuildContext(12) : FString());
    Payload->SetStringField(TEXT("system_prompt"), Personality.IsValid() ? Personality->BuildSystemPrompt() : TEXT("Fale em português brasileiro."));
    FString Body;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
    FJsonSerializer::Serialize(Payload, Writer);

    bRequestPending = true;
    TWeakObjectPtr<UVerityDialogueSystem> WeakThis(this);
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(BackendBaseUrl + TEXT("/v1/dialogue"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetTimeout(25.0f);
    Request->SetContentAsString(Body);
    Request->OnProcessRequestComplete().BindLambda([WeakThis](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bConnected)
    {
        UVerityDialogueSystem* Self = WeakThis.Get();
        if (!Self) return;
        Self->bRequestPending = false;
        if (!bConnected || !Response.IsValid() || Response->GetResponseCode() != 200)
        {
            Self->DeliverLine(Self->BuildOfflineLine(), true);
            return;
        }
        TSharedPtr<FJsonObject> Json;
        const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
        if (!FJsonSerializer::Deserialize(Reader, Json) || !Json.IsValid())
        {
            Self->DeliverLine(Self->BuildOfflineLine(), true);
            return;
        }
        FString Reply;
        if (!Json->TryGetStringField(TEXT("reply"), Reply) || Reply.TrimStartAndEnd().IsEmpty())
        {
            Self->DeliverLine(Self->BuildOfflineLine(), true);
            return;
        }
        Self->DeliverLine(Reply.Left(700), true);
    });
    Request->ProcessRequest();
}
FString UVerityDialogueSystem::BuildOfflineLine()
{
    const EVerityMood Mood = Personality.IsValid() ? Personality->GetMood() : EVerityMood::Friendly;
    return Personality.IsValid() ? Personality->GetOfflineLine(Mood) : TEXT("Estou aqui com você.");
}
void UVerityDialogueSystem::DeliverLine(const FString& Line, bool bSpeak)
{
    if (Line.TrimStartAndEnd().IsEmpty()) return;
    LastLine = Line;
    if (Memory.IsValid()) Memory->Remember(FString::Printf(TEXT("VERITY comentou: %s"), *Line.Left(220)));
    OnDialogueLine.Broadcast(LastLine);
    if (bSpeak && Voice.IsValid()) Voice->Speak(LastLine);
}
