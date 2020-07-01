// Copyright 2017-2020, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLIndividualVisualManager.generated.h"

// Forward declaration
class USLIndividualTextInfoComponent;

/**
* Manages all individual visual info
*/
UCLASS()
class USEMLOG_API ASLIndividualVisualManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLIndividualVisualManager();

protected:
#if WITH_EDITOR
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// If true, actor is ticked even if TickType == LEVELTICK_ViewportsOnly
	virtual bool ShouldTickIfViewportsOnly() const override;

	// Load components from world
	int32 Init(bool bReset = false);

	// Checks if the manager is initalized
	bool IsInit() const { return bIsInit; };

	// Re-load and re-register components
	int32 Reload() { return Init(true); };

	// Add new visual info components to the actors
	int32 AddVisualInfoComponents();
	int32 AddVisualInfoComponents(const TArray<AActor*>& Actors);

	// Remove info components from world and clear manager's cache
	int32 DestroyVisualInfoComponents();
	int32 DestroyVisualInfoComponents(const TArray<AActor*>& Actors);

	// Refresh all components
	int32 ResetVisualInfoComponents();
	int32 ResetVisualInfoComponents(const TArray<AActor*>& Actors);

	/* Functionalities */
	// Toggle visiblity
	int32 ToggleVisualInfoComponentsVisibility();
	int32 ToggleVisualInfoComponentsVisibility(const TArray<AActor*>& Actors);

	// Point text towards camera
	int32 PointToCamera();
	int32 PointToCamera(const TArray<AActor*>& Actors);

	// Enable/disable tick update
	void ToggleTickUpdate();

private:
	// Find the text component of the actor, return nullptr if none found
	USLIndividualTextInfoComponent* GetTextInfoComponent(AActor* Actor) const;

	// Create and add new individual component to actor
	USLIndividualTextInfoComponent* AddNewTextInfoComponent(AActor* Actor);

	// Check if actor already has an info component
	bool HasInfoComponent(AActor* Actor) const;

	// Info component can be created if there is an individual component present
	bool HasIndividualComponentSibling(AActor* Actor) const;

	// Removes component from the actors instanced components, triggers destroy
	void DestroyInfoComponent(USLIndividualTextInfoComponent* Component);

	// Cache component, bind delegates
	bool RegisterInfoComponent(USLIndividualTextInfoComponent* Component);

	// Remove component from cache, unbind delegates
	bool UnregisterInfoComponent(USLIndividualTextInfoComponent* Component);

	// Unregister and clear all cached components (return the number of cleared components)
	int32 ClearCache();

	/* Delegate functions */
	// Remove destroyed individuals from array
	UFUNCTION()
	void OnInfoComponentDestroyed(USLIndividualTextInfoComponent* DestroyedComponent);

	UFUNCTION()
	// Triggered by external destruction of semantic owner
	void OnSemanticOwnerDestroyed(AActor* DestroyedActor);

private:
	// Marks manager as initialized
	bool bIsInit;

	// Cached components
	TSet<USLIndividualTextInfoComponent*> RegisteredInfoComponents;
	TMap<AActor*, USLIndividualTextInfoComponent*> InfoComponentOwners;

	// bind to init and load delegates as well
	//TSet<USLIndividualTextInfoComponent*> InitIndividualComponents;
	//TSet<USLIndividualTextInfoComponent*> LoadedIndividualComponents;

	/* Buttons */
	UPROPERTY(EditAnywhere, Category = "Sematic Logger|Editor")
	bool bToggleTickUpdate;
};
