// Fill out your copyright notice in the Description page of Project Settings.

#include "Lab_1.h"
#include "CollaborativeAIController.h"


ACollaborativeAIController::ACollaborativeAIController()
{
    bDeliveringOrder = false;
    CurrentOrderNumber = -1;
}

void ACollaborativeAIController::Tick(float DeltaSeconds)
{
    // auto Orders = GetPizzaOrders();
    // auto HouseLocations = GetHouseLocations();

    int PizzaAmount = GetPizzaAmount();
    if (PizzaAmount == 0) {
        bool bGrabbedPizza = TryGrabPizza();
    }

    if (bDeliveringOrder) {
        float Distance = GetDistanceToDestination(CurrentDestination);
        if (Distance < 300.f) {
            UE_LOG(LogTemp, Warning, TEXT("Trying to deliver order %d, current distance: %1.3f"), CurrentOrderNumber, Distance);
            bool bDeliveredOrder = TryDeliverPizza(CurrentOrderNumber);
            if (bDeliveredOrder) {
                UE_LOG(LogTemp, Warning, TEXT("Delivered order %d"), CurrentOrderNumber);
                bDeliveringOrder = false;
                CurrentOrderNumber = -1;
            }
        }
    }
}

int ACollaborativeAIController::GetCurrentOrderNumber()
{
    return CurrentOrderNumber;
}

void ACollaborativeAIController::SetCurrentOrderNumber(int Num, FVector Dest)
{
    CurrentOrderNumber = Num;
    CurrentDestination = Dest;
    bDeliveringOrder = true;
    SetNewMoveDestination(CurrentDestination);
    UE_LOG(LogTemp, Warning, TEXT("~> %d"), CurrentOrderNumber);
}
