#include "Verity/VerityAIController.h"
#include "GameFramework/Character.h"

AVerityAIController::AVerityAIController()
{
    bAttachToPawn = true;
}

void AVerityAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
}
