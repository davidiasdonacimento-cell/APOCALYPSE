#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "VerityAIController.generated.h"

UCLASS()
class APOCALYPSE_API AVerityAIController : public AAIController
{
    GENERATED_BODY()
public:
    AVerityAIController();
protected:
    virtual void OnPossess(APawn* InPawn) override;
};
