// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PizzaOrder.h"
#include "CollaborativeAIController.h"
#include "StrategyManager.generated.h"

class ALab_1Character;
class ALab_1GameMode;


struct DestinationPair {
    int a;
    FVector b;
    DestinationPair(int a, FVector b): a(a), b(b) {}
};


UCLASS()
class LAB_1_API AStrategyManager : public AActor
{
private:

    GENERATED_BODY()

    TArray<FVector> GetHouseLocations();

    TArray<FPizzaOrder> GetPizzaOrders();

    float GetHouseTimeLeft(int HouseNumber);
    
    ALab_1GameMode* GetGameMode();
    
private:

    float ComputeHouseScore(float Distance, float TimeLeft);

    // Returns array of order num and house position for each pawn
    TArray<TArray<DestinationPair>> ComputeDestinations();

    // Picks one (the best) destination from a non-empty Destinations array
    DestinationPair ChooseDestination(const TArray<DestinationPair>& Destinations);

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Strategy)
    TArray<ALab_1Character*> Pawns;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Strategy)
    bool RecalculateEveryTick;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Strategy)
    float RecalculateDelay;

public:

    float RecalculateTime;

    // Sets default values for this actor's properties
    AStrategyManager();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void Tick( float DeltaSeconds ) override;

};
