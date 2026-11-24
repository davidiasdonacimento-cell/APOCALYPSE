#include "World/WorldEnvironmentController.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "World/ApocalypseWorldSubsystem.h"

AWorldEnvironmentController::AWorldEnvironmentController()
{
    PrimaryActorTick.bCanEverTick = false;
    SunLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
    SetRootComponent(SunLight);
    SunLight->SetMobility(EComponentMobility::Movable);

    SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
    SkyLight->SetupAttachment(RootComponent);
    SkyLight->SetMobility(EComponentMobility::Movable);
    SkyLight->bRealTimeCapture = true;

    SkyAtmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
    SkyAtmosphere->SetupAttachment(RootComponent);

    HeightFog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("HeightFog"));
    HeightFog->SetupAttachment(RootComponent);
}

void AWorldEnvironmentController::BeginPlay()
{
    Super::BeginPlay();
    if (UWorld* World = GetWorld())
    {
        WorldSubsystem = World->GetSubsystem<UApocalypseWorldSubsystem>();
        if (WorldSubsystem)
        {
            WorldSubsystem->OnEnvironmentUpdated.AddDynamic(this, &AWorldEnvironmentController::ApplyEnvironment);
            ApplyEnvironment(WorldSubsystem->GetEnvironmentSnapshot());
        }
    }
}

void AWorldEnvironmentController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (WorldSubsystem)
        WorldSubsystem->OnEnvironmentUpdated.RemoveDynamic(this, &AWorldEnvironmentController::ApplyEnvironment);
    Super::EndPlay(EndPlayReason);
}

void AWorldEnvironmentController::ApplyEnvironment(const FApocalypseEnvironmentSnapshot& Snapshot)
{
    ApplyTimeOfDay(Snapshot.TimeOfDayHours);
    ApplyWeather(Snapshot);
}

void AWorldEnvironmentController::ApplyTimeOfDay(float Hours)
{
    const float SolarPitch = (Hours / 24.0f) * 360.0f - 90.0f;
    SetActorRotation(FRotator(SolarPitch, -35.0f, 0.0f));
    const bool bNight = (Hours >= NightStartHour || Hours < NightEndHour);
    const float DayFactor = bNight ? 0.0f : FMath::Clamp(FMath::Sin((Hours - 6.0f) / 12.0f * PI), 0.0f, 1.0f);
    SunLight->SetIntensity(FMath::Lerp(NightLightIntensity, DayLightIntensity, DayFactor));
    SunLight->SetLightColor(bNight ? FLinearColor(0.32f, 0.40f, 0.62f) : FLinearColor(1.0f, 0.88f, 0.72f));
}

void AWorldEnvironmentController::ApplyWeather(const FApocalypseEnvironmentSnapshot& Snapshot)
{
    HeightFog->SetFogDensity(FMath::Clamp(Snapshot.FogDensity, 0.0f, 0.2f));
    HeightFog->SetFogInscatteringColor(Snapshot.Weather == EApocalypseWeather::Fog
        ? FLinearColor(0.58f, 0.62f, 0.66f)
        : FLinearColor(0.72f, 0.78f, 0.86f));
}
