// Copyright 2017-2021, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLCuttingMonitor.generated.h"


/// <summary>
/// DO NOT USE : USE SLCutterAgentClass instead
/// </summary>

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class USEMLOG_API USLCuttingMonitor : public UActorComponent
{
	GENERATED_BODY()

public:
	bool bIsInit = false;
	bool bIsStarted = false;
	bool bIsFinished = false;

private:
	// Semantic data component of the owner
	class USLIndividualComponent* OwnerIndividualComponent;

	// Semantic individual object
	USLBaseIndividual* OwnerIndividualObject;

	// Object currently grasped
	USLBaseIndividual* CurrGraspedIndividual;


public:	
	// Sets default values for this component's properties
	USLCuttingMonitor();

	bool Init();

	void Start();

	void Finish(bool bForced);

	void FinishActiveEvents();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
