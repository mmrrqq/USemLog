// Copyright 2017-2021, Institute for Artificial Intelligence - University of Bremen


#include "Actors/SLCutterAgentClass.h"
// Copyright 2017-2021, Institute for Artificial Intelligence - University of Bremen


ASLCutterAgentClass::ASLCutterAgentClass()
{

}


void ASLCutterAgentClass::BeginPlay()
{
	Super::BeginPlay();

	// Let the function listen to the Event
	CuttingSucceededEvent.AddDynamic(this, &ASLCutterAgentClass::ObjectCut);

}

void ASLCutterAgentClass::ObjectCut(float val)
{
	//Write in Console 
	UE_LOG(LogTemp, Warning, TEXT("%s::%d Sucessfully triggered Event CutterActor: %d"),
		*FString(__func__), __LINE__, val);

}

bool ASLCutterAgentClass::CuttingStarted() {
	//Write in Console 
	UE_LOG(LogTemp, Warning, TEXT("%s::%d Sucessfully started cutting Event"));
	

	return true;
	// Start new Cutting event


}
bool ASLCutterAgentClass::CuttingAborted() {
	//Write in Console 
	UE_LOG(LogTemp, Warning, TEXT("%s::%d Sucessfully aborted cutting Event"));
	return true;
}
bool ASLCutterAgentClass::CuttingSucceeded() {
	//Write in Console 
	UE_LOG(LogTemp, Warning, TEXT("%s::%d Sucessfully finished cutting Event"));
	CuttingSucceededEvent.Broadcast(1);

	return true;
}

bool ASLCutterAgentClass::CuttingIntented() {
	return true;
}

