// Copyright 2017-2020, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLIndividualComponent.generated.h"

// Forward declarations
class USLBaseIndividual;
class UDataAsset;

// Delegate notification when the component is being destroyed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLComponentDestroyedSignature, USLIndividualComponent*, DestroyedComponent);

// Notify every time the init status changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSLComponentInitChangeSignature, USLIndividualComponent*, Component, bool, bNewInitVal);

// Notify every time the loaded status changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSLComponentLoadedChangeSignature, USLIndividualComponent*, Component, bool, bNewLoadedVal);

// Notify every time the connected status changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSLComponentConnectedChangeSignature, USLIndividualComponent*, Component, bool, bNewConnectedVal);

// Notify when the individual object changes any value
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSLComponentValueChangeSignature, USLIndividualComponent*, Component, const FString&, Key, const FString&, Value);

// Notify listeners that the delegates have been cleared
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLComponentDelegatesClearedSignature, USLIndividualComponent*, Component);

/**
* Component storing the semantic individual information of its owner
*/
UCLASS( ClassGroup=(SL), meta=(BlueprintSpawnableComponent), DisplayName = "SL Individual")
class USEMLOG_API USLIndividualComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USLIndividualComponent();

protected:
	// Called after Scene is set, but before CreateRenderState_Concurrent or OnCreatePhysicsState are called
	virtual void OnRegister() override;

	// Called when a component is created (not loaded) (after post init).This can happen in the editor or during gameplay
	virtual void OnComponentCreated() override;

	// Called before destroying the object.
	virtual void BeginDestroy() override;

public:
	// Call init on the individual
	bool Init(bool bReset = false);

	// Check if component is initialized
	bool IsInit() const { return bIsInit; };

	// Load individual
	bool Load(bool bReset = false, bool bTryImport = false);

	// Check if component is loaded
	bool IsLoaded() const { return bIsLoaded; };

	// Listen to the individual object delegates
	bool Connect();

	// True if the component is listening to the individual object delegates (transient)
	bool IsConnected() const { return bIsConnected; };

	// Get the semantic individual object
	USLBaseIndividual* GetIndividualObject() const { return HasValidIndividual() ? IndividualObj : nullptr; };

	// Get the semantic individual using a cast class (nullptr if cast is unsuccessfull)
	template <typename ClassType>
	ClassType* GetCastedIndividualObject() const 
	{
		return HasValidIndividual() ? Cast<ClassType>(IndividualObj) : nullptr;
	};

	/* Functionalities */
	// Save data to owners tag
	bool ExportValues(bool bOverwrite);

	// Load data from owners tag
	bool ImportValues(bool bOverwrite);

	// Clear exported values
	bool ClearExportedValues();

	// Toggle between original and mask material is possible
	bool ToggleVisualMaskVisibility(bool bPrioritizeChildren);

	/* Values */
	/* Id */
	bool WriteId(bool bOverwrite);
	bool ClearId();

	/* Class */
	bool WriteClass(bool bOverwrite);
	bool ClearClass();

	/* Visual Mask */
	bool WriteVisualMaskClass(const FString& Value, bool bOverwrite, const TArray<FString>& ChildrenValues = TArray<FString>());
	bool ClearVisualMask();

protected:
	// Clear all references of the individual
	void InitReset();

	// Clear all data of the individual
	void LoadReset();

	// Clear any bound delegates (called when init is reset)
	void ClearDelegates();

	// Set the init flag, broadcast on new value
	void SetIsInit(bool bNewValue, bool bBroadcast = true);

	// Set the loaded flag, broadcast on new value
	void SetIsLoaded(bool bNewValue, bool bBroadcast = true);

	// Set the connected flag, broadcast on new value
	void SetIsConnected(bool bNewValue, bool bBroadcast = true);

private:
	// Create individual if not created and forward init call
	bool InitImpl();

	// Forward load call on individual
	bool LoadImpl(bool bTryImport = false);

	// Sync states with the individual
	bool BindDelegates();

	// Check if individual object is valid
	bool HasValidIndividual() const;

	// Create the individual object
	bool CreateIndividual();

	// Triggered on individual init flag change
	UFUNCTION()
	void OnIndividualInitChange(USLBaseIndividual* Individual, bool bNewValue);

	// Triggered on individual loaded flag change
	UFUNCTION()
	void OnIndividualLoadedChange(USLBaseIndividual* Individual, bool bNewValue);

	// Triggered on individual id change
	UFUNCTION()
	void OnIndividualNewId(USLBaseIndividual* Individual, const FString& NewId);

	// Triggered on individual class change
	UFUNCTION()
	void OnIndividualNewClass(USLBaseIndividual* Individual, const FString& NewClass);

	// Triggered on individual visual mask change
	UFUNCTION()
	void OnIndividualNewVisualMask(USLBaseIndividual* Individual, const FString& NewVisualMask);

	// Triggered when individual delegates are cleared (including this one)
	UFUNCTION()
	void OnIndividualDelegatesCleared(USLBaseIndividual* Individual);

public:
	// Called when the component is destroyed
	FSLComponentDestroyedSignature OnDestroyed;

	// Called when the init status changes
	FSLComponentInitChangeSignature OnInitChanged;

	// Called when the init status changes
	FSLComponentInitChangeSignature OnLoadedChanged;

	// Called when the connected status changes
	FSLComponentConnectedChangeSignature OnConnectedChanged;

	// Called when any value change from the individual object
	FSLComponentValueChangeSignature OnValueChanged;

	// Called when the delegates are cleared
	FSLComponentDelegatesClearedSignature OnDelegatesCleared;

private:
	// Semantic data
	UPROPERTY(VisibleAnywhere, Category = "Semantic Logger")
	USLBaseIndividual* IndividualObj;

	// True if the individual is succesfully created and initialized
	UPROPERTY(VisibleAnywhere, Category = "Semantic Logger")
	uint8 bIsInit : 1;

	// True if the individual is succesfully created and loaded
	UPROPERTY(VisibleAnywhere, Category = "Semantic Logger")
	uint8 bIsLoaded : 1;

	// True if the component is listening to the individual object delegates
	UPROPERTY(VisibleAnywhere, Transient, Category = "Semantic Logger")
	uint8 bIsConnected : 1;

public:
	// Semantic data
	UPROPERTY(EditAnywhere, Category = "Semantic Logger")
	TMap<FString, UDataAsset*> OptionalDataAssets;

	/* Constants */
	// Skeletal data asset map key
	static constexpr char SkelDataAssetKey[] = "SkeletalDataAsset";
};
