// Copyright 2019, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "Monitors/SLPouringMonitor.h"
#include "Monitors/SLContactMonitorBox.h"
#include "Individuals/SLIndividualComponent.h"
#include "Individuals/Type/SLBaseIndividual.h"
#include "Individuals/SLIndividualUtils.h"
#include "Individuals/type/SLParticleIndividual.h"
#include "./SLPouringParticleAgentClass.h"


#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

#include "PhysicsEngine/PhysicsConstraintActor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"


// Sets default values for this component's properties
USLPouringMonitor::USLPouringMonitor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Flags
	bIsInit = false;
	bIsStarted = false;
	bIsFinished = false;

	CurrPouredIndividual = nullptr;
	PouringTime = -1.f;
}

// Dtor
USLPouringMonitor::~USLPouringMonitor()
{
	if (!bIsFinished)
	{
		Finish(true);
	}
}

// Init listener
bool USLPouringMonitor::Init()
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
void USLPouringMonitor::Start()
{
	if (!bIsStarted && bIsInit)
	{
		if(USLContactMonitorBox* Sibling = CastChecked<USLContactMonitorBox>(
			GetOwner()->GetComponentByClass(USLContactMonitorBox::StaticClass())))
		{
			// TODO :Find our simple way to say that pouring has begine
			Sibling->OnBeginSLContact.AddUObject(this, &USLPouringMonitor::OnSLPouringBegin);
			Sibling->OnEndSLContact.AddUObject(this, &USLPouringMonitor::OnSLPouringEnd);

			bIsStarted = true;
		}
		else
		{	
			UE_LOG(LogTemp, Error, TEXT("%s::%d Could not find sibling USLContactMonitorBox .."), *FString(__func__), __LINE__);
		}
	}
}

// Finish active events
void USLPouringMonitor::Finish(float EndTime, bool bForced)
{
	if (!bIsFinished && (bIsInit || bIsStarted))
	{
		// Finish any active event
		FinishActiveEvents(EndTime);

		// Mark as finished
		bIsStarted = false;
		bIsInit = false;
		bIsFinished = true;
	}
}


// Called when pouring starts
void USLPouringMonitor::OnSLPouringBegin(const FSLContactResult& InResult)
{

	 // so everytime something overlaps something then this will be called. 
	 // We need to put some conditions here like, the other object should be of type Particles, otherwise we ignore all this. 
	if(InResult.Other->GetClass() == ASLPouringParticleAgentClass::StaticClass()) {
		// if there is already CurrPouredIndividual object then pouring is initiated
		if (!CurrPouredIndividual) {
			OnPouringBegin.Broadcast(InResult);
		}

		// Mark as poured
		CurrPouredIndividual = InResult.Other;
	}
 
}

// Called when grasp ends
void USLPouringMonitor::OnSLPouringEnd(USLBaseIndividual* Self, USLBaseIndividual* Other, float Time)
{
	OnPouringEnd.Broadcast(Self, Other, Time);
}

// Finish any active events
void USLPouringMonitor::FinishActiveEvents(float EndTime)
{
	if(CurrPouredIndividual)
	{
		// Fake a contact end call
		OnSLPouringEnd(OwnerIndividualObject, CurrPouredIndividual, EndTime);
	}	
}
 
 
