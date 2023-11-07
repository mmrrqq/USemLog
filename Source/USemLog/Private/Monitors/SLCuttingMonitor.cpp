// Copyright 2017-2021, Institute for Artificial Intelligence - University of Bremen


#include "Monitors/SLCuttingMonitor.h"
#include "Individuals/SLIndividualComponent.h"
#include <Monitors/SLManipulatorMonitor.h>


/// <summary>
/// DO NOT USE : USE SLCutterAgentClass instead
/// </summary>


// Sets default values for this component's properties
USLCuttingMonitor::USLCuttingMonitor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USLCuttingMonitor::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Init listener
bool USLCuttingMonitor::Init()
{
	if (!bIsInit)
	{
		// Make sure the owner is semantically annotated
		if (UActorComponent* AC = GetOwner()->GetComponentByClass(USLIndividualComponent::StaticClass()))
		{
			OwnerIndividualComponent = CastChecked<USLIndividualComponent>(AC);
			if (!OwnerIndividualComponent->IsLoaded())
			{
				UE_LOG(LogTemp, Error, TEXT("%s::%d %s's individual component is not loaded.."), *FString(__FUNCTION__), __LINE__, *GetOwner()->GetName());
				return false;
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s::%d %s has no individual component.."), *FString(__FUNCTION__), __LINE__, *GetOwner()->GetName());
			return false;
		}
		// Set the individual object
		OwnerIndividualObject = OwnerIndividualComponent->GetIndividualObject();
		bIsInit = true;
		return true;
	}
	return false;
}

// Start listening to grasp events, update currently overlapping objects
void USLCuttingMonitor::Start()
{
	if (!bIsStarted && bIsInit)
	{
		// Get the manipulation monitor from the VR Pawn
		//if (USLManipulatorMonitor* Sibling = CastChecked<USLManipulatorMonitor>(
		//	GetOwner()->GetComponentByClass(USLManipulatorMonitor::StaticClass())))
		//{
		//not yet used
		//use it t get the important events and data to work with and when it did not work throw the error
		//Sibling->OnBeginManipulatorGrasp.AddUObject(this, &USLContainerMonitor::OnSLGraspBegin);
		//Sibling->OnEndManipulatorGrasp.AddUObject(this, &USLContainerMonitor::OnSLGraspEnd);
		//when everything works, continue
		//}
		//else
		//{
		//	UE_LOG(LogTemp, Error, TEXT("%s::%d Could not find sibling USLManipulatorMonitor .."), *FString(__func__), __LINE__);
		//}

		bIsStarted = true;
	}
}


void USLCuttingMonitor::Finish(bool bForced)
{
	if (!bIsFinished && (bIsInit || bIsStarted))
	{
		// Finish any active event
		FinishActiveEvents();

		// Mark as finished
		bIsStarted = false;
		bIsInit = false;
		bIsFinished = true;
	}
}


void USLCuttingMonitor::FinishActiveEvents()
{
	if (CurrGraspedIndividual)
	{
		// Fake a grasp abort end call
		//OnSLGraspEnd(OwnerIndividualObject, CurrGraspedIndividual, GetWorld()->GetTimeSeconds());
	}
}



// Called every frame
void USLCuttingMonitor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

