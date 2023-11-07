// Copyright 2017-2021, Institute for Artificial Intelligence - University of Bremen

#include "Events/SLCleaningEventHandler.h"
#include <Utils/SLUuid.h>

SLCleaningEventHandler::SLCleaningEventHandler()
{
}

SLCleaningEventHandler::~SLCleaningEventHandler()
{
}



void SLCleaningEventHandler::Init(UObject* InAgentToObserve)
{
	if (!bIsInit)
	{
		// Check if parent is of right type
		AgentToObserve = Cast<ASLCleanerAgent>(InAgentToObserve);
		if (AgentToObserve)
		{
			bIsInit = true;
		}

	}
}

void SLCleaningEventHandler::Start()
{
	if (!bIsStarted && bIsInit)
	{

		UE_LOG(LogTemp, Error, TEXT("CUTTING EVENT HANDLER SUCCESSFULLY STARTED"));
		//TODO Listen to the events thrown by the cutting Monitor
		AgentToObserve->CleaningStartedEvent.AddRaw(this, &SLCleaningEventHandler::OnCleaningBegin);
		AgentToObserve->CleaningEndedEvent.AddRaw(this, &SLCleaningEventHandler::OnCleaningEnd);

		//Parent->OnBeginSLContact.AddRaw(this, &FSLContactEventHandler::OnSLOverlapBegin);
		bIsStarted = true;
	}
	else if (bIsInit) {
		UE_LOG(LogTemp, Error, TEXT("CUTTING EVENT HANDLER WAS NOT Initialized correctly"));
	}
}

void SLCleaningEventHandler::Finish(float EndTime, bool bForced)
{
	if (!bIsFinished && (bIsInit || bIsStarted))
	{

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

void SLCleaningEventHandler::FinishAllEvents(float EndTime)
{
	// Finish cutting events
// Ignore all events not finished
	StartedCleaningEvents.Empty();
}

void SLCleaningEventHandler::OnCleaningBegin(const FSLInteractionResult& cuttingInfo)
{
	AddNewSLCleaningEvent(cuttingInfo);
	UE_LOG(LogTemp, Display, TEXT("Cleaning event Started!!"));
}

void SLCleaningEventHandler::OnCleaningEnd(USLBaseIndividual* Other, float Time, int32 result)
{
	UE_LOG(LogTemp, Display, TEXT("Cleaning event Finished!!"));
	FinishSLCleaningEvent(Other, Time, static_cast<USLCleaningStatus>(result));
}

void SLCleaningEventHandler::AddNewSLCleaningEvent(const FSLInteractionResult& cuttingInfo)
{
	UE_LOG(LogTemp, Display, TEXT("Cleaning event Handler received event started"));

	TSharedPtr<FSLCleaningEvent> Event = MakeShareable(new FSLCleaningEvent(
		FSLUuid::NewGuidInBase64Url(), cuttingInfo.Time,
		FSLUuid::PairEncodeCantor(cuttingInfo.Self->GetUniqueID(), cuttingInfo.Other->GetUniqueID()),
		cuttingInfo.Self, cuttingInfo.Other));
	Event->EpisodeId = EpisodeId;

	// Add event to the pending Cutting array
	StartedCleaningEvents.Emplace(Event);
}

bool SLCleaningEventHandler::FinishSLCleaningEvent(USLBaseIndividual* InOther, float EndTime, USLCleaningStatus result)
{
	// Use iterator to be able to remove the entry from the array
		// TIndexedContainerIterator<TArray<TSharedPtr<FSLCuttingEvent>>, TSharedPtr<FSLCuttingEvent>, int32> 
	for (auto EventItr(StartedCleaningEvents.CreateIterator()); EventItr; ++EventItr)
	{
		// It is enough to compare against the other id when searching 
		if ((*EventItr)->Individual2 == InOther)
		{
			UE_LOG(LogTemp, Display, TEXT("Cutting event event Handler received event finished"));
			// Set the event end time
			(*EventItr)->EndTime = EndTime;

			// Set the result of the cutting. Is ist successfull or was it aborted? 
			// TODO: Where is it saved? 
			(*EventItr)->CuttingInformation = result;

			OnSemanticEvent.ExecuteIfBound(*EventItr);

			// Remove event from the pending list
			EventItr.RemoveCurrent();
			return true;
		}
	}
	return false;
}
