// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseAIController.h"
#include "CollaborativeAIController.generated.h"

UCLASS()
class LAB_1_API ACollaborativeAIController : public ABaseAIController
{
    GENERATED_BODY()

public:
    ACollaborativeAIController();

    int GetCurrentOrderNumber();

    void SetCurrentOrderNumber(int Num, FVector Dest);

    bool IsDeliveringOrder() { return bDeliveringOrder; }

protected:
    bool bDeliveringOrder;
    int CurrentOrderNumber;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Controller)
    FVector CurrentDestination;

protected:
    virtual void Tick(float DeltaSeconds) override;

};
