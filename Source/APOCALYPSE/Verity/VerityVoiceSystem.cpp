#include "Verity/VerityVoiceSystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Sound/SoundWaveProcedural.h"
#include "Components/AudioComponent.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Dom/JsonObject.h"

UVerityVoiceSystem::UVerityVoiceSystem() { PrimaryComponentTick.bCanEverTick = false; }
void UVerityVoiceSystem::SetBackendBaseUrl(const FString& Url) { BackendBaseUrl = Url.TrimStartAndEnd(); }
void UVerityVoiceSystem::SetVoiceEnabled(bool bEnabled) { bVoiceEnabled = bEnabled; if (!bEnabled) StopSpeaking(); }
void UVerityVoiceSystem::SetVoiceVolume(float Volume) { VoiceVolume = FMath::Clamp(Volume, 0.0f, 1.0f); if (ActiveAudio) ActiveAudio->SetVolumeMultiplier(VoiceVolume); }
void UVerityVoiceSystem::StopSpeaking() { if (ActiveAudio) ActiveAudio->Stop(); }

void UVerityVoiceSystem::Speak(const FString& Text)
{
    if (!bVoiceEnabled || Text.TrimStartAndEnd().IsEmpty()) return;
    if (const TArray<uint8>* Cached = AudioCache.Find(Text)) { PlayWav(*Cached); return; }
    if (BackendBaseUrl.IsEmpty()) return;

    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("text"), Text.Left(700));
    Payload->SetStringField(TEXT("language"), TEXT("pt-BR"));
    Payload->SetStringField(TEXT("voice"), TEXT("nova"));
    FString Body;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
    FJsonSerializer::Serialize(Payload, Writer);

    TWeakObjectPtr<UVerityVoiceSystem> WeakThis(this);
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(BackendBaseUrl + TEXT("/v1/voice"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetTimeout(35.0f);
    Request->SetContentAsString(Body);
    Request->OnProcessRequestComplete().BindLambda([WeakThis, Text](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bConnected)
    {
        UVerityVoiceSystem* Self = WeakThis.Get();
        if (!Self || !bConnected || !Response.IsValid() || Response->GetResponseCode() != 200 || !Self->bVoiceEnabled) return;
        const TArray<uint8>& Bytes = Response->GetContent();
        if (Bytes.Num() < 44 || Bytes.Num() > 8 * 1024 * 1024) return;
        Self->AudioCache.Add(Text, Bytes);
        if (Self->AudioCache.Num() > 24)
        {
            auto It = Self->AudioCache.CreateIterator();
            if (It) { It.RemoveCurrent(); }
        }
        Self->PlayWav(Bytes);
    });
    Request->ProcessRequest();
}

void UVerityVoiceSystem::PlayWav(const TArray<uint8>& Bytes)
{
    if (Bytes.Num() < 44 || FMemory::Memcmp(Bytes.GetData(), "RIFF", 4) != 0 ||
        FMemory::Memcmp(Bytes.GetData() + 8, "WAVE", 4) != 0) return;

    int32 Channels = 1;
    int32 SampleRate = 24000;
    int32 BitsPerSample = 16;
    int32 DataOffset = INDEX_NONE;
    int32 DataSize = 0;
    for (int32 Offset = 12; Offset + 8 <= Bytes.Num();)
    {
        const uint8* Header = Bytes.GetData() + Offset;
        uint32 ChunkSize = 0;
        FMemory::Memcpy(&ChunkSize, Header + 4, sizeof(uint32));
        if (ChunkSize > static_cast<uint32>(Bytes.Num()) || Offset + 8 + ChunkSize > Bytes.Num()) break;
        if (FMemory::Memcmp(Header, "fmt ", 4) == 0 && ChunkSize >= 16)
        {
            uint16 Format = 0, ChannelCount = 0, Bits = 0;
            uint32 Rate = 0;
            FMemory::Memcpy(&Format, Header + 8, sizeof(uint16));
            FMemory::Memcpy(&ChannelCount, Header + 10, sizeof(uint16));
            FMemory::Memcpy(&Rate, Header + 12, sizeof(uint32));
            FMemory::Memcpy(&Bits, Header + 22, sizeof(uint16));
            if (Format != 1) return;
            Channels = ChannelCount;
            SampleRate = static_cast<int32>(Rate);
            BitsPerSample = Bits;
        }
        else if (FMemory::Memcmp(Header, "data", 4) == 0)
        {
            DataOffset = Offset + 8;
            DataSize = static_cast<int32>(ChunkSize);
            break;
        }
        Offset += 8 + static_cast<int32>(ChunkSize) + (ChunkSize & 1);
    }
    if (DataOffset == INDEX_NONE || DataSize <= 0 || BitsPerSample != 16 ||
        (Channels != 1 && Channels != 2) || SampleRate < 8000 || SampleRate > 96000) return;

    StopSpeaking();
    ActiveWave = NewObject<USoundWaveProcedural>(this);
    ActiveWave->SetSampleRate(SampleRate);
    ActiveWave->NumChannels = Channels;
    ActiveWave->Duration = static_cast<float>(DataSize) / (SampleRate * Channels * 2);
    ActiveWave->SoundGroup = SOUNDGROUP_Voice;
    ActiveWave->QueueAudio(Bytes.GetData() + DataOffset, DataSize);

    ActiveAudio = NewObject<UAudioComponent>(GetOwner());
    ActiveAudio->bAutoActivate = false;
    ActiveAudio->SetSound(ActiveWave);
    ActiveAudio->SetVolumeMultiplier(VoiceVolume);
    if (GetOwner() && GetOwner()->GetRootComponent())
        ActiveAudio->SetupAttachment(GetOwner()->GetRootComponent());
    ActiveAudio->RegisterComponent();
    ActiveAudio->Play();
}
