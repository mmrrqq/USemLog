// Copyright 2017-2021, Institute for Artificial Intelligence - University of Bremen


#include "SLPouringParticleAgentClass.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Sets default values
ASLPouringParticleAgentClass::ASLPouringParticleAgentClass()
{ 
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = mesh;

}

// Called when the game starts or when spawned
void ASLPouringParticleAgentClass::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASLPouringParticleAgentClass::StaticClass(), FoundActors);

	UE_LOG(LogTemp, Log, TEXT("This is total number of found actors of type ASLPouringParticleAgentClass: %d"), FoundActors.Num());

	// TODO0: Modify the SLParticleIndividual such that it can log its info to the semanticmap.owl file
	// TODO1: get all particles with given class ASLPouringParticleAgentClass
	// TODO2: for each particle, check if it collides with the invisible surface of the container, ignore its own collision with other particles
	// TODO3: if it leaves the container that means it is now in pouring mode
	// TODO4: log its location every second(10 times) into tf file
	// TODO5: 
	
}

// Called every frame
void ASLPouringParticleAgentClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

