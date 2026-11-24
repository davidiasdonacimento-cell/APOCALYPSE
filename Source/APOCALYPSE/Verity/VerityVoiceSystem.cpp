#include "Verity/VerityVoiceSystem.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#endif

UVerityVoiceSystem::UVerityVoiceSystem()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UVerityVoiceSystem::Speak(const FString& Text)
{
    const FString Clean = Text.TrimStartAndEnd().Left(500);
    if (!bVoiceEnabled || Clean.IsEmpty()) return;
#if PLATFORM_ANDROID
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (!Env) return;
    jclass TTSClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/VerityTTS");
    if (!TTSClass) return;
    jmethodID SpeakMethod = Env->GetStaticMethodID(TTSClass, "speak", "(Ljava/lang/String;F)V");
    if (!SpeakMethod) { Env->DeleteLocalRef(TTSClass); return; }
    jstring JavaText = Env->NewStringUTF(TCHAR_TO_UTF8(*Clean));
    Env->CallStaticVoidMethod(TTSClass, SpeakMethod, JavaText, static_cast<jfloat>(VoiceVolume));
    Env->DeleteLocalRef(JavaText);
    Env->DeleteLocalRef(TTSClass);
#endif
}

void UVerityVoiceSystem::StopSpeaking()
{
#if PLATFORM_ANDROID
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (!Env) return;
    jclass TTSClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/VerityTTS");
    if (!TTSClass) return;
    jmethodID StopMethod = Env->GetStaticMethodID(TTSClass, "stop", "()V");
    if (StopMethod) Env->CallStaticVoidMethod(TTSClass, StopMethod);
    Env->DeleteLocalRef(TTSClass);
#endif
}

void UVerityVoiceSystem::SetVoiceEnabled(bool bEnabled)
{
    bVoiceEnabled = bEnabled;
    if (!bEnabled) StopSpeaking();
}

void UVerityVoiceSystem::SetVoiceVolume(float Volume)
{
    VoiceVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}
