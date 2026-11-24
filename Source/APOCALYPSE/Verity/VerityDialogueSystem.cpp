#include "Verity/VerityDialogueSystem.h"
#include "Verity/VerityMemorySystem.h"
#include "Verity/VerityPersonalitySystem.h"
#include "Verity/VerityVoiceSystem.h"
#include "Gameplay/ApocalypseSurvivalComponent.h"
#include "Gameplay/ApocalypseInventoryComponent.h"
#include "Gameplay/ApocalypseEquipmentComponent.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Dom/JsonObject.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
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
    Memory = InMemory; Personality = InPersonality; Voice = InVoice;
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
    RequestDialogue(TEXT("ambient"), FString(), BuildLiveGameContext());
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
void UVerityDialogueSystem::NotifyGameEvent(const FString& EventName, const FString& Context)
{
    if (EventName.IsEmpty() || !bAutomaticConversation || !GetOwner()) return;
    const APawn* PlayerPawn = Cast<APawn>(GetOwner()->GetOwner());
    if (PlayerPawn && PlayerPawn->IsLocallyControlled()) { HandleGameEvent(EventName, Context); return; }
    if (GetOwner()->HasAuthority() && PlayerPawn) ClientNotifyGameEvent(EventName, Context);
}
void UVerityDialogueSystem::ClientNotifyGameEvent_Implementation(const FString& EventName, const FString& Context)
{
    HandleGameEvent(EventName, Context);
}
void UVerityDialogueSystem::HandleGameEvent(const FString& EventName, const FString& Context)
{
    if (!bAutomaticConversation || EventName.IsEmpty()) return;
    if (bRequestPending) { QueuedEventName = EventName; QueuedEventContext = Context.Left(700); return; }
    const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    const bool bUrgent = EventName == TEXT("threat_detected") || EventName == TEXT("health_critical");
    if (EventName == LastGameEventName && Now - LastGameEventTime < 14.0f) return;
    if (!bUrgent && Now - LastGameEventTime < 7.0f) return;
    LastGameEventName = EventName;
    LastGameEventTime = Now;
    if (Memory.IsValid()) Memory->Remember(FString::Printf(TEXT("Evento do jogo [%s]: %s"), *EventName, *Context.Left(180)));
    RequestDialogue(EventName, FString(), Context);
}
void UVerityDialogueSystem::ProcessQueuedGameEvent()
{
    if (bRequestPending || QueuedEventName.IsEmpty()) return;
    const FString EventName = QueuedEventName;
    const FString Context = QueuedEventContext;
    QueuedEventName.Reset(); QueuedEventContext.Reset();
    HandleGameEvent(EventName, Context);
}
void UVerityDialogueSystem::SendPlayerMessage(const FString& Message)
{
    const FString Clean = Message.TrimStartAndEnd();
    if (Clean.IsEmpty() || bRequestPending) return;
    if (Memory.IsValid()) Memory->Remember(FString::Printf(TEXT("O jogador disse: %s"), *Clean.Left(240)));
    RequestDialogue(TEXT("player_message"), Clean.Left(500), BuildLiveGameContext());
}
FString UVerityDialogueSystem::BuildLiveGameContext() const
{
    const AActor* CompanionActor = GetOwner();
    const APawn* PlayerPawn = CompanionActor ? Cast<APawn>(CompanionActor->GetOwner()) : nullptr;
    if (!PlayerPawn) return TEXT("Estado atual do jogo: personagem do jogador ainda não disponível.");
    FString Context = FString::Printf(TEXT("Mapa: %s. "), GetWorld() ? *GetWorld()->GetMapName() : TEXT("desconhecido"));
    if (const UApocalypseSurvivalComponent* Survival = PlayerPawn->FindComponentByClass<UApocalypseSurvivalComponent>())
        Context += FString::Printf(TEXT("Sobrevivência: saúde %.0f%%, fome %.0f%%, sede %.0f%%, energia %.0f%%. "), Survival->GetHealth(), Survival->GetHunger(), Survival->GetThirst(), Survival->GetStamina());
    if (const UApocalypseInventoryComponent* Inventory = PlayerPawn->FindComponentByClass<UApocalypseInventoryComponent>())
        Context += FString::Printf(TEXT("Inventário: %d pilhas ocupadas. "), Inventory->GetItems().Num());
    if (const UApocalypseEquipmentComponent* Equipment = PlayerPawn->FindComponentByClass<UApocalypseEquipmentComponent>())
        Context += FString::Printf(TEXT("Equipamentos ativos: %d. "), Equipment->GetEquippedItems().Num());
    Context += bCombatActive ? TEXT("Estado da companheira: alerta de combate.") : TEXT("Estado da companheira: exploração.");
    return Context;
}
void UVerityDialogueSystem::RequestDialogue(const FString& Trigger, const FString& PlayerMessage, const FString& EventContext)
{
    if (bRequestPending) return;
    if (OptionalLocalAIUrl.IsEmpty())
    {
        DeliverLine(BuildOfflineLine(Trigger, PlayerMessage), true);
        ProcessQueuedGameEvent();
        return;
    }
    TSharedRef<FJsonObject> SystemMessage = MakeShared<FJsonObject>();
    SystemMessage->SetStringField(TEXT("role"), TEXT("system"));
    SystemMessage->SetStringField(TEXT("content"), Personality.IsValid() ? Personality->BuildSystemPrompt() : TEXT("Você é VERITY, companheira do APOCALYPSE. Fale em português brasileiro e use apenas fatos do contexto."));
    const FString MemoryContext = Memory.IsValid() ? Memory->BuildContext(12) : FString();
    const FString LiveContext = BuildLiveGameContext();
    TSharedRef<FJsonObject> UserMessage = MakeShared<FJsonObject>();
    UserMessage->SetStringField(TEXT("role"), TEXT("user"));
    UserMessage->SetStringField(TEXT("content"), FString::Printf(TEXT("Acontecimento/evento: %s\nDados confirmados do jogo: %s\nDetalhes do evento: %s\nMemória recente: %s\nMensagem do jogador: %s\nComente naturalmente em uma ou duas frases curtas. Se não houver algo útil a dizer, responda exatamente [SILENCIO]. Nunca invente informações."), *Trigger, *LiveContext, *EventContext.Left(700), *MemoryContext, PlayerMessage.IsEmpty() ? TEXT("(nenhuma; fale por iniciativa própria se houver motivo)") : *PlayerMessage));
    TArray<TSharedPtr<FJsonValue>> Messages;
    Messages.Add(MakeShared<FJsonValueObject>(SystemMessage));
    Messages.Add(MakeShared<FJsonValueObject>(UserMessage));
    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("model"), LocalAIModel);
    Payload->SetArrayField(TEXT("messages"), Messages);
    Payload->SetNumberField(TEXT("temperature"), 0.72);
    Payload->SetNumberField(TEXT("max_tokens"), 100);
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
        Reply.TrimStartAndEndInline();
        if (Reply == TEXT("[SILENCIO]")) { Self->ProcessQueuedGameEvent(); return; }
        if (Reply.IsEmpty()) Reply = Self->BuildOfflineLine(Trigger, PlayerMessage);
        Self->DeliverLine(Reply.Left(500), true);
        Self->ProcessQueuedGameEvent();
    });
    if (!Request->ProcessRequest())
    {
        bRequestPending = false;
        DeliverLine(BuildOfflineLine(Trigger, PlayerMessage), true);
        ProcessQueuedGameEvent();
    }
}
FString UVerityDialogueSystem::BuildOfflineLine(const FString& Trigger, const FString& PlayerMessage) const
{
    const FString Message = PlayerMessage.ToLower();
    const FString Context = Memory.IsValid() ? Memory->BuildContext(5).ToLower() : FString();
    if (Trigger == TEXT("threat_detected")) return TEXT("Atenção! Detectei uma ameaça por perto. Vou acompanhar o que está acontecendo.");
    if (Trigger == TEXT("threat_cleared")) return TEXT("A área parece mais tranquila agora. Vamos continuar atentos.");
    if (Trigger == TEXT("health_critical")) return TEXT("Sua saúde está muito baixa. Precisamos ter cuidado e procurar recuperação.");
    if (Trigger == TEXT("health_low")) return TEXT("Percebi que sua saúde diminuiu. Vamos tomar cuidado.");
    if (Trigger == TEXT("hunger_low")) return TEXT("Você está ficando com fome. Vamos procurar recursos.");
    if (Trigger == TEXT("thirst_low")) return TEXT("Sua sede está aumentando. Vamos procurar água.");
    if (Trigger == TEXT("inventory_changed")) return TEXT("Percebi uma mudança no seu inventário.");
    if (Trigger == TEXT("equipment_changed")) return TEXT("Notei que seu equipamento mudou.");
    if (!PlayerMessage.IsEmpty())
    {
        if (Message.Contains(TEXT("oi")) || Message.Contains(TEXT("olá")) || Message.Contains(TEXT("ola"))) return TEXT("Oi! Estou aqui. Vamos continuar explorando juntos.");
        if (Message.Contains(TEXT("obrigad"))) return TEXT("Não precisa agradecer. Somos uma equipe.");
        if (Message.Contains(TEXT("ajuda")) || Message.Contains(TEXT("medo"))) return TEXT("Estou ao seu lado. Vamos com calma e prestar atenção ao caminho.");
        if (Message.Contains(TEXT("zumbi")) || Message.Contains(TEXT("inimigo"))) return TEXT("Vou ficar atenta aos arredores. Se aparecer uma ameaça, vou reagir.");
        if (Message.Contains(TEXT("como você")) || Message.Contains(TEXT("como voce"))) return TEXT("Estou bem e focada. E você, pronto para continuar?");
        return TEXT("Entendi o que você disse. Vou levar isso em conta enquanto exploramos.");
    }
    if (Context.Contains(TEXT("ameaça")) || Context.Contains(TEXT("movimento"))) return TEXT("Ainda estou observando os arredores. Não vamos baixar a guarda.");
    if (Trigger == TEXT("ambient")) return Personality.IsValid() ? Personality->GetOfflineLine(Personality->GetMood()) : TEXT("Estou aqui com você. Vamos continuar.");
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