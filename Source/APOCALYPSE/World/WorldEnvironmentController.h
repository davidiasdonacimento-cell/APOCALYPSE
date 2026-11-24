#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/ApocalypseWorldTypes.h"
#include "WorldEnvironmentController.generated.h"

class UDirectionalLightComponent;
class USkyLightComponent;
class USkyAtmosphereComponent;
class UExponentialHeightFogComponent;
class UApocalypseWorldSubsystem;

UCLASS(Blueprintable)
class APOCALYPSE_API AWorldEnvironmentController : public AActor
{
    GENERATED_BODY()
public:
    AWorldEnvironmentController();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UFUNCTION() void ApplyEnvironment(const FApocalypseEnvironmentSnapshot& Snapshot);
    void ApplyTimeOfDay(float Hours);
    void ApplyWeather(const FApocalypseEnvironmentSnapshot& Snapshot);

    UPROPERTY(VisibleAnywhere, Category="Environment") TObjectPtr<UDirectionalLightComponent> SunLight;
    UPROPERTY(VisibleAnywhere, Category="Environment") TObjectPtr<USkyLightComponent> SkyLight;
    UPROPERTY(VisibleAnywhere, Category="Environment") TObjectPtr<USkyAtmosphereComponent> SkyAtmosphere;
    UPROPERTY(VisibleAnywhere, Category="Environment") TObjectPtr<UExponentialHeightFogComponent> HeightFog;
    UPROPERTY(EditAnywhere, Category="Environment|Lighting", meta=(ClampMin="0.0")) float DayLightIntensity = 100000.0f;
    UPROPERTY(EditAnywhere, Category="Environment|Lighting", meta=(ClampMin="0.0")) float NightLightIntensity = 0.05f;
    UPROPERTY(EditAnywhere, Category="Environment|Lighting", meta=(ClampMin="0.0", ClampMax="24.0")) float NightStartHour = 19.0f;
    UPROPERTY(EditAnywhere, Category="Environment|Lighting", meta=(ClampMin="0.0", ClampMax="24.0")) float NightEndHour = 5.0f;
    UPROPERTY() TObjectPtr<UApocalypseWorldSubsystem> WorldSubsystem;
};
