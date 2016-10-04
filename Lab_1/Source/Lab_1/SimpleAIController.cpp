// Fill out your copyright notice in the Description page of Project Settings.

#include "Lab_1.h"
#include "SimpleAIController.h"

ASimpleAIController::ASimpleAIController()
{
    bDeliveringOrder = false;
    bIsNewDelivery = false;
    CurrentOrderNumber = -1;
}

float ASimpleAIController::Score(float distance, float time_left)
{
    return 1.f / (distance + 350.f * time_left + 1.f);
}

void ASimpleAIController::Tick(float DeltaSeconds)
{
    auto Orders = GetPizzaOrders();
    auto HouseLocations = GetHouseLocations();

    if (bDeliveringOrder) {
        float Distance = GetDistanceToDestination(CurrentDestination);
        if (Distance > 300.f && !bIsNewDelivery) {
            for (int i = 0; i < Orders.Num(); ++i) {
                float currentDistance = GetDistanceToDestination(HouseLocations[Orders[i].HouseNumber]);
                if (currentDistance < 400.f && (CurrentDestination - HouseLocations[Orders[i].HouseNumber]).Size() > 10) {
                    CurrentOrderNumber = Orders[i].OrderNumber;
                    CurrentDestination = HouseLocations[Orders[i].HouseNumber];
                    UE_LOG(LogTemp, Warning, TEXT("New delivery target: order %d to house %d"), Orders[i].OrderNumber, Orders[i].HouseNumber);
                    bIsNewDelivery = true;
                    break;
                }
            }
            SetNewMoveDestination(CurrentDestination);
            return;
        } else {
            UE_LOG(LogTemp, Warning, TEXT("Trying to deliver order %d, current distance: %1.3f"), CurrentOrderNumber, Distance);
            bool bDeliveredOrder = TryDeliverPizza(CurrentOrderNumber);
            if (bDeliveredOrder) {
                UE_LOG(LogTemp, Warning, TEXT("Delivered order %d"), CurrentOrderNumber);
                bDeliveringOrder = false;
                bIsNewDelivery = false;
                CurrentOrderNumber = -1;
            }
            return;
        }
    }

    if (Orders.Num() == 0) {
        return;
    }

    float score = 0;
    int order = 0;
    for (int i = 0; i < Orders.Num(); ++i) {
        float currentDistance = GetDistanceToDestination(HouseLocations[Orders[i].HouseNumber]);
        float currentScore = Score(currentDistance, 20.f - Orders[i].CurrentWaitTime);
        if (currentDistance < 300) {
            order = i;
            break;
        }
        if (score < currentScore) {
            score = currentScore;
            order = i;
        }
    }
    auto Order = Orders[order];

    int PizzaAmount = GetPizzaAmount();
    if (PizzaAmount == 0) {
        bool bGrabbedPizza = TryGrabPizza();
        // Failed to retrieve pizza, need to get closer to the bakery.
        if (!bGrabbedPizza) {
            return;
        }
    }

    auto HouseLocation = HouseLocations[Order.HouseNumber];
    bDeliveringOrder = true;
    CurrentOrderNumber = Order.OrderNumber;
    CurrentDestination = HouseLocation;
    SetNewMoveDestination(HouseLocation);
    UE_LOG(LogTemp, Warning, TEXT("Took new order %d to house %d"), Order.OrderNumber, Order.HouseNumber);
}
