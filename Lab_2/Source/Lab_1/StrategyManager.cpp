// Fill out your copyright notice in the Description page of Project Settings.

#include "Lab_1.h"
#include "Lab_1Character.h"
#include "Kismet/GameplayStatics.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "StrategyManager.h"
#include "Lab_1GameMode.h"

#include <limits>


float AStrategyManager::ComputeHouseScore(float Distance, float TimeLeft)
{
    return fmax(Distance - 150.f, 1.f) * (TimeLeft * TimeLeft);
}

TArray<TArray<DestinationPair>> AStrategyManager::ComputeDestinations()
{
    auto Orders = GetPizzaOrders();
    auto HouseLocations = GetHouseLocations();

    TArray<TArray<DestinationPair>> Destinations;
    TArray<TArray<float>> Scores;
    TArray<TArray<int>> OrderNums;
    TArray<bool> HouseIsBurning;
    TArray<bool> PawnIsBusy;

    Destinations.SetNumZeroed(Pawns.Num());
    HouseIsBurning.SetNumZeroed(HouseLocations.Num(), false);

    OrderNums.SetNumZeroed(Pawns.Num(), false);
    Scores.SetNumZeroed(Pawns.Num(), false);
    for (int i = 0; i < Pawns.Num(); ++i) {
        OrderNums[i].SetNumZeroed(HouseLocations.Num(), false);
        Scores[i].SetNumZeroed(HouseLocations.Num(), false);
        for (int j = 0; j < HouseLocations.Num(); ++j) {
            OrderNums[i][j] = -1;
            Scores[i][j] = 0;
        }
    }

    for (auto& Order: Orders) {
        for (int i = 0; i < Pawns.Num(); ++i) {
            ACollaborativeAIController* controller = dynamic_cast<ACollaborativeAIController*>(Pawns[i]->Controller);
            if (!controller) {
                UE_LOG(LogTemp, Error, TEXT("Wrong controller"));
                break;
            }
            float Distance = controller->GetDistanceToDestination(HouseLocations[Order.HouseNumber]);
            float TimeLeft = GetHouseTimeLeft(Order.HouseNumber);
            float Score = ComputeHouseScore(Distance, TimeLeft);
            if (Scores[i][Order.HouseNumber] > Score || OrderNums[i][Order.HouseNumber] == -1) {
                Scores[i][Order.HouseNumber] = Score;
                OrderNums[i][Order.HouseNumber] = Order.OrderNumber;
            }
            HouseIsBurning[Order.HouseNumber] = true;
        }
    }

    int HouseIndex, PawnIndex;
    do {
        HouseIndex = -1;
        PawnIndex = -1;
        float MinScore = 0;

        for (int i = 0; i < Scores.Num(); ++i) {
            for (int j = 0; j < Scores[i].Num(); ++j) {
                if (HouseIsBurning[j] && !Destinations[i].Num() && (Scores[i][j] < MinScore || HouseIndex == -1)) {
                    MinScore = Scores[i][j];
                    PawnIndex = i;
                    HouseIndex = j;
                }
            }
        }

        if (HouseIndex != -1) {
            HouseIsBurning[HouseIndex] = false;
            DestinationPair o_O(OrderNums[PawnIndex][HouseIndex], HouseLocations[HouseIndex]);
            Destinations[PawnIndex].Add(o_O);
        }
    } while (HouseIndex != -1);

    return Destinations;
}

DestinationPair AStrategyManager::ChooseDestination(const TArray<DestinationPair>& Destinations)
{
    return Destinations[0];
}

// Sets default values
AStrategyManager::AStrategyManager()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    RecalculateEveryTick = 0;
    RecalculateDelay = 0.1;
    RecalculateTime = RecalculateDelay;
}

// Called when the game starts or when spawned
void AStrategyManager::BeginPlay()
{
    Super::BeginPlay();
}

ALab_1GameMode* AStrategyManager::GetGameMode()
{
    return Cast<ALab_1GameMode>(UGameplayStatics::GetGameMode(this));
}

TArray<FVector> AStrategyManager::GetHouseLocations()
{
     auto* MyGameMode = GetGameMode();
    return MyGameMode->GetHouseLocations();
}

TArray<FPizzaOrder> AStrategyManager::GetPizzaOrders()
{
     auto* MyGameMode = GetGameMode();
    return MyGameMode->GetPizzaOrders();
}

float AStrategyManager::GetHouseTimeLeft(int HouseNumber)
{
     auto* MyGameMode = GetGameMode();
    return MyGameMode->GetHouseTimeLeft(HouseNumber);
}

// Called every frame
void AStrategyManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    bool NeedRecalculate = false;

    RecalculateTime += DeltaTime;
    if (RecalculateTime > RecalculateDelay) {
        RecalculateTime -= RecalculateDelay;
        if (RecalculateEveryTick) {
            NeedRecalculate = true;
        }
    }

    for (int i = 0; i < Pawns.Num(); ++i) {
        ACollaborativeAIController* controller = dynamic_cast<ACollaborativeAIController*>(Pawns[i]->Controller);
        if (!controller) {
            UE_LOG(LogTemp, Error, TEXT("Wrong controller"));
            break;
        }
        if (!controller->IsDeliveringOrder()) {
            NeedRecalculate = true;
            break;
        }
    }

    if (NeedRecalculate) {
        auto Destinations = ComputeDestinations();

        for (int i = 0; i < Pawns.Num(); ++i) {
            if (Destinations[i].Num()) {
                ACollaborativeAIController* controller = dynamic_cast<ACollaborativeAIController*>(Pawns[i]->Controller);
                if (!controller) {
                    UE_LOG(LogTemp, Error, TEXT("Wrong controller"));
                    return;
                }
                auto Destination = ChooseDestination(Destinations[i]);
                UE_LOG(LogTemp, Warning, TEXT("%d -> %d"), i, Destination.a);
                controller->SetCurrentOrderNumber(Destination.a, Destination.b);
            }
        }
    }
}
