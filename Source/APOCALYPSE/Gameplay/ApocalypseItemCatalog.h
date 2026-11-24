#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ApocalypseItemCatalog.generated.h"
UENUM(BlueprintType)
enum class EApocalypseItemCategory : uint8 { Resource, Food, Water, Medical, Material, Quest };
USTRUCT(BlueprintType)
struct FApocalypseItemDefinition
{
    GENERATED_BODY()
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName ItemId = NAME_None;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FText DisplayName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EApocalypseItemCategory Category = EApocalypseItemCategory::Resource;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="999")) int32 MaxStack = 50;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.0")) float HungerRestoration = 0.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.0")) float ThirstRestoration = 0.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.0")) float HealthRestoration = 0.0f;
};
UCLASS(BlueprintType)
class APOCALYPSE_API UApocalypseItemCatalog : public UDataAsset
{
    GENERATED_BODY()
public:
    UApocalypseItemCatalog();
    const FApocalypseItemDefinition* FindItem(FName ItemId) const;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory") TArray<FApocalypseItemDefinition> Items;
};