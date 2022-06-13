#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Individuals/Type/SLBaseIndividual.h"
#include "SLParticleIndividual.generated.h"
 

/**
 *
 */
UCLASS(ClassGroup = SL)
class USEMLOG_API USLParticleIndividual : public USLBaseIndividual
{
    GENERATED_BODY()

public:
    // Ctor
	USLParticleIndividual();

     
protected:
	// Update its location according to the gaze data
	void Update();

	// Listen if it can listen to gaze data
	void Init();

	// True is it can
	bool bIsInit;
 
      
};

