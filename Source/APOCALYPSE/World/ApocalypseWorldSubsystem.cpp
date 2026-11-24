#include "World/ApocalypseWorldSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "World/WorldRegionDefinition.h"
#include "World/WorldRegionRegistry.h"

void UApocalypseWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    TimeOfDayHours = FMath::Fmod(FMath::Max(0.0f, InitialTimeOfDayHours), 24.0f);
    WeatherRandom.Initialize(WorldSeed);
    RefreshActiveRegion();
}

void UApocalypseWorldSubsystem::Deinitialize()
{
    RegionRegistry = nullptr;
    ActiveRegion = nullptr;
    Super::Deinitialize();
}

void UApocalypseWorldSubsystem::Tick(float DeltaTime)
{
    if (!GetWorld() || GetWorld()->IsPaused() || DeltaTime <= 0.0f) return;

    const float SafeDayLengthSeconds = FMath::Max(300.0f, DayLengthMinutes * 60.0f);
    TimeOfDayHours = FMath::Fmod(TimeOfDayHours + (DeltaTime / SafeDayLengthSeconds) * 24.0f, 24.0f);

    RegionCheckAccumulator += DeltaTime;
    if (RegionCheckAccumulator >= RegionCheckIntervalSeconds)
    {
        RegionCheckAccumulator = 0.0f;
        RefreshActiveRegion();
    }

    WeatherCheckAccumulator += DeltaTime;
    if (WeatherCheckAccumulator >= WeatherCheckIntervalSeconds)
    {
        WeatherCheckAccumulator = 0.0f;
        UpdateWeatherFromRegion();
    }

    EnvironmentBroadcastAccumulator += DeltaTime;
    if (EnvironmentBroadcastAccumulator >= 1.0f)
    {
        EnvironmentBroadcastAccumulator = 0.0f;
        BroadcastEnvironment();
    }
}

TStatId UApocalypseWorldSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UApocalypseWorldSubsystem, STATGROUP_Tickables);
}

bool UApocalypseWorldSubsystem::IsTickable() const
{
    return GetWorld() != nullptr;
}

void UApocalypseWorldSubsystem::SetTimeOfDayHours(float Hours)
{
    TimeOfDayHours = FMath::Fmod(FMath::Max(0.0f, Hours), 24.0f);
    BroadcastEnvironment();
}

void UApocalypseWorldSubsystem::SetDayLengthMinutes(float Minutes)
{
    DayLengthMinutes = FMath::Clamp(Minutes, 5.0f, 240.0f);
}

void UApocalypseWorldSubsystem::SetWorldSeed(int32 Seed)
{
    WorldSeed = Seed;
    WeatherRandom.Initialize(WorldSeed);
}

void UApocalypseWorldSubsystem::SetWeather(EApocalypseWeather NewWeather)
{
    if (CurrentWeather == NewWeather) return;
    const EApocalypseWeather PreviousWeather = CurrentWeather;
    CurrentWeather = NewWeather;
    OnWeatherChanged.Broadcast(PreviousWeather, CurrentWeather);
    BroadcastEnvironment();
}

void UApocalypseWorldSubsystem::SetRegionRegistry(UWorldRegionRegistry* Registry)
{
    RegionRegistry = Registry;
    RefreshActiveRegion();
}

void UApocalypseWorldSubsystem::RefreshActiveRegion()
{
    if (!GetWorld() || !RegionRegistry) return;

    FVector PlayerLocation = FVector::ZeroVector;
    if (APlayerController* Controller = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* Pawn = Controller->GetPawn()) PlayerLocation = Pawn->GetActorLocation();
    }

    UWorldRegionDefinition* NewRegion = RegionRegistry->FindRegionAtLocation(PlayerLocation);
    if (ActiveRegion != NewRegion)
    {
        UWorldRegionDefinition* PreviousRegion = ActiveRegion;
        ActiveRegion = NewRegion;
        OnRegionChanged.Broadcast(PreviousRegion, ActiveRegion);
        BroadcastEnvironment();
    }
}

void UApocalypseWorldSubsystem::UpdateWeatherFromRegion()
{
    if (!ActiveRegion) return;
    const FApocalypseWeatherWeights& Weights = ActiveRegion->WeatherWeights;
    const float Total = Weights.Clear + Weights.Cloudy + Weights.Rain + Weights.Storm + Weights.Fog;
    if (Total <= KINDA_SMALL_NUMBER) return;

    const float Roll = WeatherRandom.FRandRange(0.0f, Total);
    float Cursor = Weights.Clear;
    EApocalypseWeather Selected = EApocalypseWeather::Clear;
    if (Roll < Cursor) Selected = EApocalypseWeather::Clear;
    else if (Roll < (Cursor += Weights.Cloudy)) Selected = EApocalypseWeather::Cloudy;
    else if (Roll < (Cursor += Weights.Rain)) Selected = EApocalypseWeather::Rain;
    else if (Roll < (Cursor += Weights.Storm)) Selected = EApocalypseWeather::Storm;
    else Selected = EApocalypseWeather::Fog;
    SetWeather(Selected);
}

FApocalypseEnvironmentSnapshot UApocalypseWorldSubsystem::GetEnvironmentSnapshot() const
{
    FApocalypseEnvironmentSnapshot Snapshot;
    Snapshot.TimeOfDayHours = TimeOfDayHours;
    Snapshot.NormalizedDayTime = GetNormalizedDayTime();
    Snapshot.Weather = CurrentWeather;

    if (ActiveRegion)
    {
        Snapshot.Biome = ActiveRegion->Biome;
        Snapshot.AmbientTemperatureCelsius = ActiveRegion->BaseTemperatureCelsius;
        Snapshot.WindStrength = ActiveRegion->BaseWindStrength;
        Snapshot.FogDensity = ActiveRegion->BaseFogDensity;
    }

    switch (CurrentWeather)
    {
        case EApocalypseWeather::Cloudy: Snapshot.FogDensity *= 1.25f; break;
        case EApocalypseWeather::Rain: Snapshot.FogDensity *= 1.6f; Snapshot.WindStrength *= 1.5f; break;
        case EApocalypseWeather::Storm: Snapshot.FogDensity *= 2.0f; Snapshot.WindStrength = FMath::Max(0.8f, Snapshot.WindStrength); break;
        case EApocalypseWeather::Fog: Snapshot.FogDensity = FMath::Max(0.08f, Snapshot.FogDensity * 3.0f); break;
        default: break;
    }
    return Snapshot;
}

void UApocalypseWorldSubsystem::BroadcastEnvironment()
{
    OnEnvironmentUpdated.Broadcast(GetEnvironmentSnapshot());
}
