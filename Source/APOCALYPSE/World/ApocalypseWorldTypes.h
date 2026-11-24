#pragma once
#include "CoreMinimal.h"
#include "ApocalypseWorldTypes.generated.h"

UENUM(BlueprintType)
enum class EApocalypseBiome : uint8
{
    TemperateForest, DenseForest, Rural, Urban, Industrial, Coastal, Mountain, Swamp
};

UENUM(BlueprintType)
enum class EApocalypseWeather : uint8
{
    Clear, Cloudy, Rain, Storm, Fog
};

UENUM(BlueprintType)
enum class EApocalypsePOIType : uint8
{
    Settlement, SafeZone, Hospital, PoliceStation, MilitarySite, IndustrialSite, Landmark, ResourceSite
};

USTRUCT(BlueprintType)
struct APOCALYPSE_API FApocalypseWeatherWeights
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weather", meta=(ClampMin="0.0")) float Clear = 0.35f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weather", meta=(ClampMin="0.0")) float Cloudy = 0.30f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weather", meta=(ClampMin="0.0")) float Rain = 0.20f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weather", meta=(ClampMin="0.0")) float Storm = 0.05f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weather", meta=(ClampMin="0.0")) float Fog = 0.10f;
};

USTRUCT(BlueprintType)
struct APOCALYPSE_API FApocalypseEnvironmentSnapshot
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly, Category="World") float TimeOfDayHours = 8.0f;
    UPROPERTY(BlueprintReadOnly, Category="World") float NormalizedDayTime = 0.333333f;
    UPROPERTY(BlueprintReadOnly, Category="World") EApocalypseWeather Weather = EApocalypseWeather::Clear;
    UPROPERTY(BlueprintReadOnly, Category="World") EApocalypseBiome Biome = EApocalypseBiome::TemperateForest;
    UPROPERTY(BlueprintReadOnly, Category="World") float AmbientTemperatureCelsius = 18.0f;
    UPROPERTY(BlueprintReadOnly, Category="World") float WindStrength = 0.15f;
    UPROPERTY(BlueprintReadOnly, Category="World") float FogDensity = 0.015f;
};
