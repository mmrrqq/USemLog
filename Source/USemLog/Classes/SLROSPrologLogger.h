// Copyright 2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "Tickable.h"
#include "CoreMinimal.h"
#if SL_WITH_ROSBRIDGE
#include "ROSBridgeHandler.h"
#include "rosprolog_msgs/Query.h"
#include "rosprolog_msgs/NextSolution.h"
#include "rosprolog_msgs/Finish.h"
#include "SLROSServiceClient.h"
#endif // SL_WITH_ROSBRIDGE
#include "UObject/NoExportTypes.h"
#include "Events/ISLEventHandler.h"
#include "SLROSPrologLogger.generated.h"

/**
 * Raw (event) data logger,
 * it synchronizes(ticks) the async worker on saving the world state at given timepoints.
 * Inherit from FTickableGameObject to have it's own tick
 */
UCLASS()
class USEMLOG_API USLROSPrologLogger : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:

	// Constructor
	USLROSPrologLogger();

	// Destructor
	~USLROSPrologLogger();

#if SL_WITH_ROSBRIDGE

	void Disconnect();
	void Init(FString Host, uint32 port);

	void AddEvent(TSharedPtr<ISLEvent> Event);

	void SendPrologQuery(FString Id);

	void SendNextSolutionCommand(FString Id);

	void SendFinishCommand(FString Id);

	void ProcessResponse(TSharedPtr<FROSBridgeSrv::SrvResponse> InResponse, FString Type);
#endif // SL_WITH_ROSBRIDGE

protected:

	/** Begin FTickableGameObject interface */
	// Called after ticking all actors, DeltaTime is the time passed since the last call.
	virtual void Tick(float DeltaTime) override;

	// Return if object is ready to be ticked
	virtual bool IsTickable() const override;

	// Return the stat id to use for this tickable
	virtual TStatId GetStatId() const override;
	/** End FTickableGameObject interface */

private:

	// True if the object can be ticked (used by FTickableGameObject)
	bool bIsTickable;

#if SL_WITH_ROSBRIDGE

	// ROS Connection handlers
	TSharedPtr<FROSBridgeHandler> ROSHandler;
	TSharedPtr<SLROSServiceClient> ROSPrologQueryClient;
	TSharedPtr<SLROSServiceClient> ROSPrologNextSolutionClient;
	TSharedPtr<SLROSServiceClient> ROSPrologFinishClient;

	TMap<TSharedPtr<FROSBridgeSrv::SrvResponse>, FString> SentQueries;
	TMap<TSharedPtr<FROSBridgeSrv::SrvResponse>, FString> SentNextSolutionCommands;
	TMap<TSharedPtr<FROSBridgeSrv::SrvResponse>, FString> SentFinishCommands;

#endif // SL_WITH_ROSBRIDGE

	// Query Queue
	TMap<FString, FString> QueriesBuffer;
	TArray<FString> NextSolutionCommandsBuffer;
	TArray<FString> FinishCommandsBuffer;

};
