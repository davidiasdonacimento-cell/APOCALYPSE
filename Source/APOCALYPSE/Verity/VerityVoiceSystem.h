#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VerityVoiceSystem.generated.h"

class UAudioComponent;
class USoundWaveProcedural;

UCLASS(ClassGroup=(VERITY), meta=(BlueprintSpawnableComponent))
class APOCALYPSE_API UVerityVoiceSystem : public UActorComponent
{
    GENERATED_BODY()
public:
    UVerityVoiceSystem();
    void SetBackendBaseUrl(const FString& Url);
    UFUNCTION(BlueprintCallable, Category="VERITY|Voice") void Speak(const FString& Text);
    UFUNCTION(BlueprintCallable, Category="VERITY|Voice") void StopSpeaking();
    UFUNCTION(BlueprintCallable, Category="VERITY|Voice") void SetVoiceEnabled(bool bEnabled);
    UFUNCTION(BlueprintCallable, Category="VERITY|Voice") void SetVoiceVolume(float Volume);
    UFUNCTION(BlueprintPure, Category="VERITY|Voice") bool IsVoiceEnabled() const { return bVoiceEnabled; }
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VERITY|Voice") bool bVoiceEnabled = true;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VERITY|Voice", meta=(ClampMin="0.0", ClampMax="1.0")) float VoiceVolume = 0.85f;
private:
    FString BackendBaseUrl;
    UPROPERTY(Transient) TObjectPtr<UAudioComponent> ActiveAudio;
    UPROPERTY(Transient) TObjectPtr<USoundWaveProcedural> ActiveWave;
    TMap<FString, TArray<uint8>> AudioCache;
    void PlayWav(const TArray<uint8>& Bytes);
};
