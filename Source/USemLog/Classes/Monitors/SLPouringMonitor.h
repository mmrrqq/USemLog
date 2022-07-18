// Copyright 2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Monitors/SLMonitorStructs.h"
#include "Monitors/SLContactMonitorBox.h"
#include "SLPouringMonitor.generated.h"


// Forward declaration
class USLBaseIndividual;

/** Notify the beginning and the end of a opening/closing container event */
DECLARE_MULTICAST_DELEGATE_OneParam(FSLPouringBeginSignature, const FSLContactResult&);

DECLARE_MULTICAST_DELEGATE_ThreeParams(FSLPouringEndSignature, USLBaseIndividual* /*Self*/, USLBaseIndividual* /*Particle*/, float /*Time*/);

/**
 * Checks for if the manipulated objects directly/indirectly opens / closes a container
 */
UCLASS( ClassGroup=(SL), meta=(BlueprintSpawnableComponent), DisplayName = "SL Pouring Monitor")
class USEMLOG_API USLPouringMonitor : public USLContactMonitorBox
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USLPouringMonitor();

	// Dtor
	~USLPouringMonitor();

	// Check if owner is valid and semantically annotated
	bool Init();

	// Start listening to grasp events, update currently overlapping objects
	void Start();

	// Stop publishing grasp events
	void Finish(float EndTime, bool bForced = false);

	// Get init state
	bool IsInit() const { return bIsInit; };

	// Get started state
	bool IsStarted() const { return bIsStarted; };

	// Get finished state
	bool IsFinished() const { return bIsFinished; };

private:
	// Called on Pouring begin
	void OnSLPouringBegin(const FSLContactResult& InResult);

	// Called on Pouring end
	void OnSLPouringEnd(USLBaseIndividual* Self, USLBaseIndividual* Other, float Time);
	
	// Finish any active events
	void FinishActiveEvents(float EndTime);
	 

public:
	// Pouring manipulation delegate
	FSLPouringBeginSignature OnPouringBegin;
	FSLPouringEndSignature OnPouringEnd;

private:
	// True if initialized
	uint8 bIsInit : 1;

	// True if started
	uint8 bIsStarted : 1;

	// True if finished
	uint8 bIsFinished : 1;

	// Semantic data component of the owner
	class USLIndividualComponent* OwnerIndividualComponent;

	// Semantic individual object
	USLBaseIndividual* OwnerIndividualObject;

	// Object currently grasped
	USLBaseIndividual* CurrPouredIndividual;

	// Pouring time
	float PouringTime;

};

