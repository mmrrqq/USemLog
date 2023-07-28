// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "Events/SLPouringEvent.h"
#include "Individuals/Type/SLBaseIndividual.h"
#include "Owl/SLOwlExperimentStatics.h"
#include <iostream>
#include <string> 

// Constructor with initialization
FSLPouringEvent::FSLPouringEvent(const FString& InId, float InStart, float InEnd, uint64 InPairId,
	USLBaseIndividual* InIndividual1,
	USLBaseIndividual* InIndividual2,
	USLPouringEventTypes PouringEventType):
	ISLEvent(InId, InStart, InEnd),
	PairId(InPairId),
	Individual1(InIndividual1), 
	Individual2(InIndividual2),
	PouringEventTypes(PouringEventType)
{
}

// Constructor initialization without end time
FSLPouringEvent::FSLPouringEvent(const FString& InId, float InStart, uint64 InPairId,
	USLBaseIndividual* InIndividual1,
	USLBaseIndividual* InIndividual2,
	USLPouringEventTypes PouringEventType):
	ISLEvent(InId, InStart),
	PairId(InPairId),
	Individual1(InIndividual1),
	Individual2(InIndividual2),
	PouringEventTypes(PouringEventType)
{
}

/* Begin ISLEvent interface */
// Get an owl representation of the event
FSLOwlNode FSLPouringEvent::ToOwlNode() const
{
	// Create the Pouring event node
	FString EventName = "PouringSituation: " + FString(PouringEventTypes == USLPouringEventTypes::PouredInto ? "Poured in to" : "Poured out");
	
	FSLOwlNode EventIndividual = FSLOwlExperimentStatics::CreateEventIndividual(
		"log", Id, EventName);
	EventIndividual.AddChildNode(FSLOwlExperimentStatics::CreateStartTimeProperty("log", StartTime));
	EventIndividual.AddChildNode(FSLOwlExperimentStatics::CreateEndTimeProperty("log", EndTime));
	EventIndividual.AddChildNode(FSLOwlExperimentStatics::CreateInEpisodeProperty("log", EpisodeId));
	EventIndividual.AddChildNode(FSLOwlExperimentStatics::CreateInContactProperty("log", Individual1->GetIdValue()));
	EventIndividual.AddChildNode(FSLOwlExperimentStatics::CreateInContactProperty("log", Individual2->GetIdValue()));
	EventIndividual.AddChildNode(FSLOwlExperimentStatics::CreatePouredParticlesProperty("log", NumberOfParticles));
	return EventIndividual;
}

// Add the owl representation of the event to the owl document
void FSLPouringEvent::AddToOwlDoc(FSLOwlDoc* OutDoc)
{
	// Add timepoint individuals
	// We know that the document is of type FOwlExperiment,
	// we cannot use the safer dynamic_cast because RTTI is not enabled by default
	// if (FOwlEvents* EventsDoc = dynamic_cast<FOwlEvents*>(OutDoc))
	FSLOwlExperiment* EventsDoc = static_cast<FSLOwlExperiment*>(OutDoc);
	EventsDoc->RegisterTimepoint(StartTime);
	EventsDoc->RegisterTimepoint(EndTime);
	EventsDoc->RegisterObject(Individual1);
	EventsDoc->RegisterObject(Individual2);
	OutDoc->AddIndividual(ToOwlNode());
}

// Get event context data as string (ToString equivalent)
FString FSLPouringEvent::Context() const
{
	FString EventType = FString(PouringEventTypes == USLPouringEventTypes::PouredInto ? "Poured in to" : "Poured out");
	return FString::Printf(TEXT("%s - %lld"), *EventType , PairId);
}

// Get the tooltip data
FString FSLPouringEvent::Tooltip() const
{
	return FString::Printf(TEXT("\'O1\',\'%s\',\'Id\',\'%s\',\'O2\',\'%s\',\'Id\',\'%s\',\'O3\',\'%s\',\'Id\',\'%s\', \'Id\',\'%s\'"),
		*Individual1->GetClassValue(), *Individual1->GetIdValue(), *Individual2->GetClassValue(), *Individual2->GetIdValue(), *DestinationContainerName, *Individual2->GetIdValue(), *Id);
}

// Get the data as string
FString FSLPouringEvent::ToString() const
{
	return FString::Printf(TEXT("Individual1:[%s] Individual2:[%s] PairId:%lld"),
		*Individual1->GetInfo(), *Individual2->GetInfo(), PairId);
}

FString FSLPouringEvent::RESTCallToKnowRob(FSLKRRestClient* InFSLKRRestClient) const
{
	// Call REST method to create sub actions on KnowRob side...
	// somaClassName:somaIndividualName is sent at the moment for objects participated
	FString ObjectsPartcipated = TEXT("[") 
		+ Individual1->GetClassValue() + TEXT(":") + Individual1->GetParentActor()->GetActorLabel() 
		+ TEXT(",") 
		+ Individual2->GetClassValue() + TEXT(":") + Individual2->GetParentActor()->GetActorLabel() 
		+ TEXT("]");

	FString SubActionType = TEXT("soma:'") + FString(PouringEventTypes == USLPouringEventTypes::PouredInto ? "PouredInTo" : "PouredOut") 
		+ TEXT("'");
	FString TaskType = TEXT("soma:'Pouring'");

	/*float MaxPouringAngleX = -180, MaxPouringAngleY, MaxPouringAngleZ;
	float MinPouringAngleX = 180, MinPouringAngleY, MinPouringAngleZ;
	TArray<FVector> SourceContainerPoses;
	TArray<FVector> DestinationContainerPoses;*/


	// logic for selection of Source or Destination Container poses Array object..
	//if (PouringEventTypes == USLPouringEventTypes::PouredOut) {
	//	// pouring out event shows that the source container locations are stores in PouringPoseForSourceContainer
	//	
	//	for (std::tuple<FString, FTransform> PP : PouringPoseForSourceContainer) {
	//		// if given container name is same as the one stored in tuple
	//		if (ContainerName == std::get<0>(PP)) {
	//			// find min and max pouring angles
	//			if (MaxPouringAngleX < std::get<1>(PP).GetRotation().Euler().X) {
	//				MaxPouringAngleX = std::get<1>(PP).GetRotation().Euler().X;
	//				MaxPouringAngleY = std::get<1>(PP).GetRotation().Euler().Y;
	//				MaxPouringAngleZ = std::get<1>(PP).GetRotation().Euler().Z;

	//			}
	//			if (MinPouringAngleX > std::get<1>(PP).GetRotation().Euler().X) {
	//				MinPouringAngleX = std::get<1>(PP).GetRotation().Euler().X;
	//				MinPouringAngleY = std::get<1>(PP).GetRotation().Euler().Y;
	//				MinPouringAngleZ = std::get<1>(PP).GetRotation().Euler().Z;
	//			}

	//			// get all poses for source container
	//			SourceContainerPoses.Add(std::get<1>(PP).GetTranslation());

	//		}
	//	}
	//	
	//}
	//else {
	//	// pouring out event will have destination locations stored in PouringPoseForSourceContainer
	//	for (std::tuple<FString, FTransform> PP : PouringPoseForDestinationContainer) {
	//		DestinationContainerPoses.Add(std::get<1>(PP).GetTranslation());
	//	}
	//}
	




	/*UE_LOG(LogTemp, Warning, TEXT("%lf is the angle of source container: "), PouringAnglesForSourceContainer[0][0]);
	UE_LOG(LogTemp, Warning, TEXT("%lf is the pose of source container: "), PouringPoseForSourceContainer[0][0]);
	UE_LOG(LogTemp, Warning, TEXT("%lf is the pose of desti container: "), PouringPoseForDestinationContainer[0][0]);*/

	InFSLKRRestClient->SendCreateSubActionRequest(SubActionType, TaskType,
		ObjectsPartcipated, double(StartTime), double(EndTime));

	/*FString MaxAngleStr = TEXT("") + FString::SanitizeFloat(MaxPouringAngleX) + TEXT(",") + FString::SanitizeFloat(MaxPouringAngleY) + TEXT(",") + FString::SanitizeFloat(MaxPouringAngleZ);
	FString MinAngleStr = TEXT("") + FString::SanitizeFloat(MinPouringAngleX) + TEXT(",") + FString::SanitizeFloat(MinPouringAngleY) + TEXT(",") + FString::SanitizeFloat(MinPouringAngleZ);
	FString SourceContainerPosesStr = TEXT("[");
	for (FVector pose : SourceContainerPoses) {
		SourceContainerPosesStr += TEXT("(") + FString::SanitizeFloat(pose.X) + TEXT(",") + FString::SanitizeFloat(pose.Y) + TEXT(",") 
			+ FString::SanitizeFloat(pose.Z) + TEXT(")");
	}
	SourceContainerPosesStr += TEXT("]");

	FString DestContainerPosesStr = TEXT("[");
	for (FVector pose : DestinationContainerPoses) {
		DestContainerPosesStr += TEXT("(") + FString::SanitizeFloat(pose.X) + TEXT(",") + FString::SanitizeFloat(pose.Y) + TEXT(",")
			+ FString::SanitizeFloat(pose.Z) + TEXT(")");
	}
	DestContainerPosesStr += TEXT("]");*/

	/*if (PouringEventTypes == USLPouringEventTypes::PouredOut) {
		InFSLKRRestClient->SendPouringAdditionalRequest(SubActionType, MaxAngleStr, MinAngleStr,
			ContainerName, TEXT(""), SourceContainerPosesStr);
	}
	else {
		InFSLKRRestClient->SendPouringAdditionalRequest(SubActionType, TEXT(""), TEXT(""),
			TEXT(""), ContainerName, DestContainerPosesStr);
	}*/

	

	return TEXT("Succeed!");
}
/* End ISLEvent interface */