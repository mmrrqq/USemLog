// Copyright 2019, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SLMaskCalibrationTool.generated.h"

// Forward declarations
class ASkeletalMeshActor;
class AStaticMeshActor;
class UGameViewportClient;
class UMaterialInstanceDynamic;

/**
 * Renders the masked materials on the items and 
 * compares the rendered pixel values with the original ones
 */
UCLASS()
class USLMaskCalibrationTool : public UObject
{
	GENERATED_BODY()
	
public:
	// Ctor
	USLMaskCalibrationTool();

	// Dtor
	~USLMaskCalibrationTool();

	// Setup scanning room
	void Init(bool bMaskColorsOnlyDemo = false, const FString& InFolderName = FString());

	// Start scanning, set camera into the first pose and trigger the screenshot
	void Start();

	// Finish scanning
	void Finish();

	// Get init state
	bool IsInit() const { return bIsInit; };

	// Get started state
	bool IsStarted() const { return bIsStarted; };

	// Get finished state
	bool IsFinished() const { return bIsFinished; };

protected:
	// Request a screenshot
	void RequestScreenshot();

	// Called when the screenshot is captured
	void ScreenshotCB(int32 SizeX, int32 SizeY, const TArray<FColor>& Bitmap);

	// Move first item in position
	bool SetupFirstItem();

	// Move thenext item in position, return false if there are no more items
	bool SetupNextItem();


private:
	// Init hi-res screenshot resolution
	void InitScreenshotResolution(FIntPoint InResolution);

	// Init render parameters (resolution, view mode)
	void InitRenderParameters();

	// Load mesh that will be used to render all the mask colors on screen
	bool LoadMaskRenderMesh();

	// Load camera convenience actor
	bool LoadTargetCameraPoseActor();

	// Load the mask colors to their entities mapping (while hiding all the actors in the world)
	bool LoadMaskMappings();

	/* Legacy */
	// Create mask clones of the available entities, hide everything else
	bool SetupWorld(bool bOnlyDemo = false);

protected:
	// Set when initialized
	bool bIsInit;

	// Set when started
	bool bIsStarted;

	// Set when finished
	bool bIsFinished;

private:
	// Mesh used to load all the mask materials and rendered on screen
	UPROPERTY() // Avoid GC
	AStaticMeshActor* MaskRenderActor;

	// Convenience actor for setting the camera pose (SetViewTarget(InActor))
	UPROPERTY() // Avoid GC
	AStaticMeshActor* CameraPoseActor;

	// Mask color to actor mapping
	TMap<FColor, AStaticMeshActor*> MaskToEntity;

	// Mask color to skeletal actor bone mapping
	TMap<FColor, TPair<ASkeletalMeshActor*, FName>> MaskToSkeletalBone;


	/* Legacy */
	// Map from the cloned actors to the real ones
	TArray<TPair<AStaticMeshActor*, AStaticMeshActor*>> CloneToRealArray;

	// Used for triggering the screenshot request
	UGameViewportClient* ViewportClient;

	// Location on where to save the data locally (skip if empty)
	FString IncludeLocallyFolderName;

	// Current name of scan (for saving locally, and progress update purposes)
	FString CurrImgName;

	// Currently active item index in map
	int32 CurrItemIdx;

	// Currently active entity
	AStaticMeshActor* CurrEntity;
};
