#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "World/ApocalypseWorldTypes.h"
#include "ApocalypseWorldSubsystem.generated.h"

class UWorldRegionDefinition;
class UWorldRegionRegistry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FApocalypseEnvironmentUpdated, const FApocalypseEnvironmentSnapshot&, Snapshot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FApocalypseRegionChanged, UWorldRegionDefinition*, PreviousRegion, UWorldRegionDefinition*, NewRegion);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FApocalypseWeatherChanged, EApocalypseWeather, PreviousWeather, EApocalypseWeather, NewWeather);

UCLASS()
class APOCALYPSE_API UApocalypseWorldSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;
    virtual bool IsTickable() const override;

    UFUNCTION(BlueprintCallable, Category="World|Time") void SetTimeOfDayHours(float Hours);
    UFUNCTION(BlueprintCallable, Category="World|Time") void SetDayLengthMinutes(float Minutes);
    UFUNCTION(BlueprintPure, Category="World|Time") float GetTimeOfDayHours() const { return TimeOfDayHours; }
    UFUNCTION(BlueprintPure, Category="World|Time") float GetNormalizedDayTime() const { return TimeOfDayHours / 24.0f; }
    UFUNCTION(BlueprintCallable, Category="World|Weather") void SetWeather(EApocalypseWeather NewWeather);\n    UFUNCTION(BlueprintCallable, Category="World|Weather") void SetWorldSeed(int32 Seed);
    UFUNCTION(BlueprintPure, Category="World|Weather") EApocalypseWeather GetWeather() const { return CurrentWeather; }
    UFUNCTION(BlueprintCallable, Category="World|Regions") void SetRegionRegistry(UWorldRegionRegistry* Registry);
    UFUNCTION(BlueprintCallable, Category="World|Regions") void RefreshActiveRegion();
    UFUNCTION(BlueprintPure, Category="World|Regions") UWorldRegionDefinition* GetActiveRegion() const { return ActiveRegion; }
    UFUNCTION(BlueprintPure, Category="World|Environment") FApocalypseEnvironmentSnapshot GetEnvironmentSnapshot() const;

    UPROPERTY(BlueprintAssignable, Category="World|Events") FApocalypseEnvironmentUpdated OnEnvironmentUpdated;
    UPROPERTY(BlueprintAssignable, Category="World|Events") FApocalypseRegionChanged OnRegionChanged;
    UPROPERTY(BlueprintAssignable, Category="World|Events") FApocalypseWeatherChanged OnWeatherChanged;

private:
    void UpdateWeatherFromRegion();
    void BroadcastEnvironment();

    UPROPERTY() TObjectPtr<UWorldRegionRegistry> RegionRegistry;
    UPROPERTY() TObjectPtr<UWorldRegionDefinition> ActiveRegion;
    UPROPERTY(EditDefaultsOnly, Category="World|Time", meta=(ClampMin="5.0", ClampMax="240.0")) float DayLengthMinutes = 60.0f;
    UPROPERTY(EditDefaultsOnly, Category="World|Time", meta=(ClampMin="0.0", ClampMax="24.0")) float InitialTimeOfDayHours = 8.0f;
    UPROPERTY(EditDefaultsOnly, Category="World|Weather", meta=(ClampMin="10.0")) float WeatherCheckIntervalSeconds = 120.0f;\n    UPROPERTY(EditDefaultsOnly, Category="World|Weather") int32 WorldSeed = 20261124;
    UPROPERTY(EditDefaultsOnly, Category="World|Regions", meta=(ClampMin="0.1")) float RegionCheckIntervalSeconds = 1.0f;
    float TimeOfDayHours = 8.0f;
    float EnvironmentBroadcastAccumulator = 0.0f;
    float WeatherCheckAccumulator = 0.0f;
    float RegionCheckAccumulator = 0.0f;
    EApocalypseWeather CurrentWeather = EApocalypseWeather::Clear;
    FRandomStream WeatherRandom;
};
