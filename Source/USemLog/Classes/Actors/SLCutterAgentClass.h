// Copyright 2017-2021, Institute for Artificial Intelligence - University of Bremen
// In order to recognize custom blueprint class with Actor type, 
// we need to create custom Agent class which acts as parent class for blueprint objects. See Pourig particles blueprint example.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "SLCutterAgentClass.generated.h"

//To be callable by blueprints it has to be DynamicMulticast
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCuttingSucceeded, float, input);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCuttingStarted, float, input);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCuttingAborted, float, input);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReserve);

UCLASS(ClassGroup = (SL), DisplayName = "SL Cutter Actor")
class USEMLOG_API ASLCutterAgentClass : public AStaticMeshActor
{
	GENERATED_BODY()

private:
	bool isCutting;

public:
	ASLCutterAgentClass();

	//Let the Event be Assignable (connect Listeners), and be Callable by blueprints
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Test")
		FCuttingSucceeded CuttingSucceededEvent;

	//Create a basic function
	UFUNCTION(Category = "EventTest")
		void ObjectCut(float val);


	//ToDo fill with parameters needed
	UFUNCTION(BlueprintCallable)
		bool CuttingSucceeded();

	UFUNCTION(BlueprintCallable)
		bool CuttingIntented();

	UFUNCTION(BlueprintCallable)
		bool CuttingStarted();

	UFUNCTION(BlueprintCallable)
		bool CuttingAborted();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};