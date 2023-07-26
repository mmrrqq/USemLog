// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "Events/SLContactEventHandler.h"
#include "Monitors/SLContactMonitorInterface.h"
#include "Events/SLContactEvent.h"
#include "Events/SLSupportedByEvent.h"
#include "Individuals/Type/SLBaseIndividual.h"
#include "Utils/SLUuid.h"
#include "Individuals/Type/SLParticleIndividual.h"
#include "Events/SLPouringEvent.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine/World.h"

// Set parent
void FSLContactEventHandler::Init(UObject* InParent)
{
	if (!bIsInit)
	{
		// Check if parent is of right type
		Parent = Cast<ISLContactMonitorInterface>(InParent);
		if (Parent)
		{
			bIsInit = true;
		}

		
	}
}

// Bind to input delegates
void FSLContactEventHandler::Start()
{
	if (!bIsStarted && bIsInit)
	{
		if (Parent)
		{
			Parent->OnBeginSLContact.AddRaw(this, &FSLContactEventHandler::OnSLOverlapBegin);
			Parent->OnEndSLContact.AddRaw(this, &FSLContactEventHandler::OnSLOverlapEnd);

			Parent->OnBeginSLSupportedBy.AddRaw(this, &FSLContactEventHandler::OnSLSupportedByBegin);
			Parent->OnEndSLSupportedBy.AddRaw(this, &FSLContactEventHandler::OnSLSupportedByEnd);

			// TODO: Use pouring event start and end here. This is for some reason not working when the event is detected.
			Parent->OnPouringBegin.AddRaw(this, &FSLContactEventHandler::OnSLPouringBegin);
			Parent->OnPouringEnds.AddRaw(this, &FSLContactEventHandler::OnSLPouringEnds);
		}

		// Mark as started
		bIsStarted = true;
	}
}

// Terminate listener, finish and publish remaining events
void FSLContactEventHandler::Finish(float EndTime, bool bForced)
{
	if (!bIsFinished && (bIsInit || bIsStarted))
	{
		// Let parent first publish any pending (delayed) events
		if(!Parent->IsFinished())
		{
			Parent->Finish();
		}
		
		// End and broadcast all started events
		FinishAllEvents(EndTime);

		// TODO use dynamic delegates to be able to unbind from them
		// https://docs.unrealengine.com/en-us/Programming/UnrealArchitecture/Delegates/Dynamic
		// this would mean that the handler will need to inherit from UObject

		// Mark finished
		bIsStarted = false;
		bIsInit = false;
		bIsFinished = true;
	}
}

// Start new contact event
void FSLContactEventHandler::AddNewContactEvent(const FSLContactResult& InResult)
{
	// TODO: this code is temporary solution to increase the fps for simulation case when 100s of particles collide with objects
		// Start a semantic contact event
	TSharedPtr<FSLContactEvent> Event = MakeShareable(new FSLContactEvent(
		FSLUuid::NewGuidInBase64Url(), InResult.Time,
		FSLUuid::PairEncodeCantor(InResult.Self->GetUniqueID(), InResult.Other->GetUniqueID()),
		InResult.Self, InResult.Other));
	Event->EpisodeId = EpisodeId;
	// Add event to the pending contacts array
	StartedContactEvents.Emplace(Event);

	// Start a semantic Pouring event, if particles are involved in contact
	// TODO: Replace this with the Parent->OnPouringBegin.AddRaw
	AddNewPouringEvent(InResult);

	// TODO: add new spilling event as well when particles touch the table or other defined surfaces
}

// Publish finished event
bool FSLContactEventHandler::FinishContactEvent(USLBaseIndividual* InOther, float EndTime)
{
	// Use iterator to be able to remove the entry from the array
	for (auto EventItr(StartedContactEvents.CreateIterator()); EventItr; ++EventItr)
	{
		PouringEndTime = EndTime;
		// It is enough to compare against the other id when searching
		if ((*EventItr)->Individual2 == InOther)
		{
			// Set the event end time
			(*EventItr)->EndTime = EndTime;

			// Avoid publishing short events
			if (((*EventItr)->EndTime - (*EventItr)->StartTime) > ContactEventMin)
			{
				OnSemanticEvent.ExecuteIfBound(*EventItr);
			}
			
			
			if ((*EventItr)->Individual2->GetClass() == USLParticleIndividual::StaticClass()) {
				// find out how many particles are ending overlap with source container
				particlesOverlapEnded = particlesOverlapEnded + 1;

				// find out if the last particle that is dropped from the source container is it 5 seconds ago?
				// Then remove current pouring event from the List of events and create new Pouring event
				

				//UE_LOG(LogTemp, Warning, TEXT("%d total particles overlapped.."),
				//	particlesOverlapEnded);
			}

			// Remove event from the pending list
			EventItr.RemoveCurrent();

			// if for the last contact event, the end time is the pouring endtime
			if (PouringEndTime < EndTime) {
				PouringEndTime = EndTime;
			}

			return true;
		}
	}
	return false;
}

// Start new supported by event
void FSLContactEventHandler::AddNewSupportedByEvent(USLBaseIndividual* Supported, USLBaseIndividual* Supporting, float StartTime, const uint64 EventPairId)
{
	// Start a supported by event
	TSharedPtr<FSLSupportedByEvent> Event = MakeShareable(new FSLSupportedByEvent(
		FSLUuid::NewGuidInBase64Url(), StartTime, EventPairId, Supported, Supporting));
	Event->EpisodeId = EpisodeId;
	// Add event to the pending array
	StartedSupportedByEvents.Emplace(Event);
}

// Finish then publish the event
bool FSLContactEventHandler::FinishSupportedByEvent(const uint64 InPairId, float EndTime)
{
	// Use iterator to be able to remove the entry from the array
	for (auto EventItr(StartedSupportedByEvents.CreateIterator()); EventItr; ++EventItr)
	{
		// It is enough to compare against the other id when searching
		if ((*EventItr)->PairId == InPairId)
		{
			// Ignore short events
			if (EndTime - (*EventItr)->StartTime > SupportedByEventMin)
			{
				// Set end time and publish event
				(*EventItr)->EndTime = EndTime;
				OnSemanticEvent.ExecuteIfBound(*EventItr);
			}
			// Remove event from the pending list
			EventItr.RemoveCurrent();
			return true;
		}
	}
	return false;
}

// Terminate and publish pending contact events (this usually is called at end play)
void FSLContactEventHandler::FinishAllEvents(float EndTime)
{
	// Finish contact events
	for (auto& Ev : StartedContactEvents)
	{
		// Ignore short events
		if (EndTime - Ev->StartTime > ContactEventMin)
		{
			// Set end time and publish event
			Ev->EndTime = EndTime;
			OnSemanticEvent.ExecuteIfBound(Ev);
		}
	}
	StartedContactEvents.Empty();

	// Finish supported by events
	for (auto& Ev : StartedSupportedByEvents)
	{
		// Ignore short events
		if ((EndTime - Ev->StartTime) > SupportedByEventMin)
		{
			// Set end time and publish event
			Ev->EndTime = EndTime;
			OnSemanticEvent.ExecuteIfBound(Ev);
		}
	}
	StartedSupportedByEvents.Empty();

	// Finish pouring events
	for (auto& Ev : StartedPouringEvents)
	{
		// Ignore short events
		if (PouringEndTime - Ev->StartTime > PouringEventMin)
		{
			// Set end time and publish event
			Ev->EndTime = PouringEndTime;
			Ev->NumberOfParticles = particlesOverlapEnded;
			Ev->PouringPoseForSourceContainer = PouringPoseForSourceContainer;
			Ev->PouringPoseForDestinationContainer = PouringPoseForDestinationContainer;
			OnSemanticEvent.ExecuteIfBound(Ev);
		}
	}
	StartedPouringEvents.Empty();
}

// Start new Pouring event
void FSLContactEventHandler::AddNewPouringEvent(const FSLContactResult& InResult)
{
	// find out the angle of the containers and define which one is the source and which one is the destination container
	float ContainerEulerXAngle = InResult.Self->GetCachedPose().GetRotation().Euler().X;
	float ContainerEulerYAngle = InResult.Self->GetCachedPose().GetRotation().Euler().Y;

	FVector rot = InResult.Self->GetCachedPose().GetRotation().Euler();

	FVector pose = InResult.Self->GetCachedPose().GetTranslation();
	//std::tuple<float, float> PouringAngleForSourceContainer{ sourceContainerEulerXAngle, sourceContainerEulerYAngle};
	// Start a semantic Pouring event, check if the source container has required angles around X and Y axis in oder to consider it as source container
	if (InResult.Other->GetClass() == USLParticleIndividual::StaticClass() &&
		(ContainerEulerXAngle > 45.00 || ContainerEulerXAngle < -45.00 
			|| ContainerEulerYAngle > 45.00 || ContainerEulerYAngle < -45.00)) {
		// TODO: find out logic other than this to include multiple pouring events
		// TODO: Perform logic based upon time, ie how many particles are poured in a given second? And how long pouring event should be?
		// Is there a way for combining contineous pouring events? 
		if (StartedPouringEvents.Num() == 0 || SourceContainerName != InResult.Self->GetClassValue()) {
		//if (StartedPouringEvents.Num() == 0 || !HasPouringEventHappened(SourceContainers, InResult)) {
			TSharedPtr<FSLPouringEvent> Event = MakeShareable(new FSLPouringEvent(
				FSLUuid::NewGuidInBase64Url(), InResult.Time,
				FSLUuid::PairEncodeCantor(InResult.Self->GetUniqueID(), InResult.Other->GetUniqueID()),
				InResult.Self, InResult.Other, USLPouringEventTypes::PouredOut, InResult.Self->GetClassValue()));
			Event->EpisodeId = EpisodeId;
			// Add event to the pending Pourings array
			StartedPouringEvents.Emplace(Event);
			SourceContainers.Emplace(InResult.Self->GetClassValue());
			SourceContainerName = InResult.Self->GetClassValue();
		}
		// Add current pose for source container to the list of tuples
		PouringPoseForSourceContainer.Add({SourceContainerName, InResult.Self->GetCachedPose() });
		
		
	}
	else if (InResult.Other->GetClass() == USLParticleIndividual::StaticClass() &&
		(ContainerEulerXAngle < 45.00 || ContainerEulerXAngle > -45.00
			|| ContainerEulerYAngle < 45.00 || ContainerEulerYAngle > -45.00)) {
		// Start a semantic Pouring event, for destination container
		
		// TODO: find out logic other than this to include multiple pouring events
		if (StartedPouringEvents.Num() == 0 || DestinationContainerName != InResult.Self->GetClassValue()) {
			TSharedPtr<FSLPouringEvent> Event = MakeShareable(new FSLPouringEvent(
				FSLUuid::NewGuidInBase64Url(), InResult.Time,
				FSLUuid::PairEncodeCantor(InResult.Self->GetUniqueID(), InResult.Other->GetUniqueID()),
				InResult.Self, InResult.Other, USLPouringEventTypes::PouredInto, InResult.Self->GetClassValue()));
			Event->EpisodeId = EpisodeId;
			// Add event to the pending Pourings array
			StartedPouringEvents.Emplace(Event);
			DestinationContainers.Emplace(InResult.Self->GetClassValue());
			DestinationContainerName = InResult.Self->GetClassValue();
		}
		
		// Add current pose for desti container to the list of tuples
		PouringPoseForDestinationContainer.Add({ DestinationContainerName, InResult.Self->GetCachedPose() });

	}

}

/*
*  Check if for the given list of Pouring events the container name is the same, if yes then the event has started
*/

bool FSLContactEventHandler::HasPouringEventHappened(TArray <FString> Containers, const FSLContactResult& InResult) {
	for (FString Container : Containers) {
		if (Container == InResult.Self->GetClassValue()) {
			return true;
		}

	}
	return false;
}

// Publish finished event
bool FSLContactEventHandler::FinishPouringEvent(USLBaseIndividual* InOther, float EndTime)
{
	// Use iterator to be able to remove the entry from the array
	for (auto EventItr(StartedPouringEvents.CreateIterator()); EventItr; ++EventItr)
	{
		// It is enough to compare against the other id when searching
		if ((*EventItr)->Individual2 == InOther)
		{
			// Set the event end time
			(*EventItr)->EndTime = EndTime;

			// Avoid publishing short events
			if (((*EventItr)->EndTime - (*EventItr)->StartTime) > PouringEventMin)
			{
				OnSemanticEvent.ExecuteIfBound(*EventItr);
			}

			// Remove event from the pending list
			EventItr.RemoveCurrent();

			return true;
		}
	}
	return false;
}

// Event called when a semantic overlap event begins
void FSLContactEventHandler::OnSLOverlapBegin(const FSLContactResult& InResult)
{
	AddNewContactEvent(InResult);
}

// Event called when a semantic overlap event ends
void FSLContactEventHandler::OnSLOverlapEnd(USLBaseIndividual* Self, USLBaseIndividual* Other, float Time)
{
	FinishContactEvent(Other, Time);
}

// Event called when a supported by event begins
void FSLContactEventHandler::OnSLSupportedByBegin(USLBaseIndividual* Supported, USLBaseIndividual* Supporting, float StartTime, const uint64 PairId)
{
	AddNewSupportedByEvent(Supported, Supporting, StartTime, PairId);
}

// Event called when a 'possible' supported by event ends
void FSLContactEventHandler::OnSLSupportedByEnd(const uint64 PairId1, const uint64 PairId2, float EndTime)
{
	if(!FinishSupportedByEvent(PairId1, EndTime))
	{
		FinishSupportedByEvent(PairId2, EndTime);
	}
}

void FSLContactEventHandler::OnSLPouringBegin(const FSLContactResult& InResult)
{
	AddNewPouringEvent(InResult);
}

void FSLContactEventHandler::OnSLPouringEnds(USLBaseIndividual* Self, USLBaseIndividual* Other, float Time)
{
	FinishPouringEvent(Other, Time);
}