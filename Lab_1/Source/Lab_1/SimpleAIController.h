// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseAIController.h"
#include "SimpleAIController.generated.h"

/**
 * 
 */
UCLASS()
class LAB_1_API ASimpleAIController : public ABaseAIController
{
    GENERATED_BODY()

public:
    ASimpleAIController();

private:

    float Score(float distance, float time_left);

protected:
    virtual void Tick(float DeltaSeconds) override;

    bool bDeliveringOrder;
    bool bIsNewDelivery;
    int CurrentOrderNumber;
    FVector CurrentDestination;
};
