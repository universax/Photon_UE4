// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotonCharacter.h"

// Sets default values
APhotonCharacter::APhotonCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APhotonCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhotonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APhotonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

