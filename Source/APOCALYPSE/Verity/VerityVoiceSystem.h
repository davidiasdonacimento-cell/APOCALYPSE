#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VerityVoiceSystem.generated.h"

UCLASS(ClassGroup=(VERITY), meta=(BlueprintSpawnableComponent))
class APOCALYPSE_API UVerityVoiceSystem : public UActorComponent
{
    GENERATED_BODY()
public:
    UVerityVoiceSystem();
    UFUNCTION(BlueprintCallable, Category="VERITY|Voice") void Speak(const FString& Text);
    UFUNCTION(BlueprintCallable, Category="VERITY|Voice") void StopSpeaking();
    UFUNCTION(BlueprintCallable, Category="VERITY|Voice") void SetVoiceEnabled(bool bEnabled);
    UFUNCTION(BlueprintCallable, Category="VERITY|Voice") void SetVoiceVolume(float Volume);
    UFUNCTION(BlueprintPure, Category="VERITY|Voice") bool IsVoiceEnabled() const { return bVoiceEnabled; }
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VERITY|Voice") bool bVoiceEnabled = true;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VERITY|Voice", meta=(ClampMin="0.0", ClampMax="1.0")) float VoiceVolume = 0.85f;
};
