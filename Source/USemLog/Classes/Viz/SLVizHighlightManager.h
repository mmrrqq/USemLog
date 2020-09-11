// Copyright 2017-2020, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Viz/SLVizStructs.h"
#include "SLVizHighlightManager.generated.h"

// Forward declarations
class USLVizAssets;
class UMeshComponent;
class UMaterialInterface;

/**
 * Stores the original materials for re-applying them
 * and the dynamic material for allowing dynamic color updates
 */
USTRUCT()
struct USEMLOG_API FSLVizHighlightData
{
	GENERATED_BODY()

	// Array of the original materials
	UPROPERTY()
	TArray<UMaterialInterface*> OriginalMaterials;

	// The dynamic material used for the highlight
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	// Default ctor
	FSLVizHighlightData() {};

	// Init ctor
	FSLVizHighlightData(const TArray<UMaterialInterface*>& InMaterials) : OriginalMaterials(InMaterials) {};
};


/**
 * Manages highliting of individuals without the use of markers
 * keeps track of modified and original materials of actors
 */
UCLASS(ClassGroup = (SL), DisplayName = "SL Viz Highlight Manager")
class USEMLOG_API ASLVizHighlightManager : public AInfo
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	ASLVizHighlightManager();

protected:
	// Do any object-specific cleanup required immediately after loading an object. (called only once when loading the map)
	virtual void PostLoad() override;

	// When an actor is created in the editor or during gameplay, this gets called right before construction. This is not called for components loaded from a level. 
	virtual void PostActorCreated() override;

	// Called when actor removed from game or game ended
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called when this actor is explicitly being destroyed during gameplay or in the editor, not called during level streaming or gameplay ending 
	virtual void Destroyed() override;

	// Load container with the vizual assets
	bool LoadAssetsContainer();

	// Callback function registered with global world delegates to reset materials to their original values
	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

	// Callback function that will fire every time a world is destroyed.
	void OnPreWorldFinishDestroy(UWorld* World);

	// PIE event
	void OnPIETestEvent(bool bIsSimulating);

	// Make sure the original materials are applied if the manager is destroyed or the level closed etc.
	void RestoreOriginalMaterials();

public:
	// Highlight the given mesh component
	void Highlight(UMeshComponent* MC,
		const TArray<int32>& MaterialIndexes = TArray<int32>(),
		const FSLVizVisualParams& VisualParams = FSLVizVisualParams());

	// Update the visual of the given mesh component
	void UpdateVisual(const FSLVizVisualParams& VisualParams);

	// Clear highlight of a static mesh
	void ClearHighlight(UMeshComponent* MC);

	// Clear all highlights
	void ClearAllHighlights();

private:
	// Bind delegates
	void BindDelgates();

	// Remove any bound delegates
	void RemoveDelegates();

	// Create a dynamic material instance
	UMaterialInstanceDynamic* CreateTransientMID(ESLVizMaterialType InMaterialType);


protected:
	// List of the highlighted static meshes with their original materials
	UPROPERTY()
	//TMap<UStaticMeshComponent*, TArray<UMaterialInterface*>> HighlightedStaticMeshes;
	TMap<UMeshComponent*, FSLVizHighlightData> HighlightedStaticMeshes;

private:
	// Viz assets container
	USLVizAssets* VizAssetsContainer;

	/* Constants */
	constexpr static TCHAR* AssetsContainerPath = TEXT("SLVizAssets'/USemLog/Viz/SL_VizAssetsContainer.SL_VizAssetsContainer'");
};
