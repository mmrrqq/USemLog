#include "Individuals/Type/SLParticleIndividual.h"

USLParticleIndividual::USLParticleIndividual() {
	
}

 

// Connect to the gaze data
void USLParticleIndividual::Init()
{
	if (bIsInit)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s::%d %s is already initialized.."), *FString(__FUNCTION__), __LINE__, *GetName());
		return;
	}
	 
	 
}

// Update pose according to the gaze data
void USLParticleIndividual::Update()
{ 
}