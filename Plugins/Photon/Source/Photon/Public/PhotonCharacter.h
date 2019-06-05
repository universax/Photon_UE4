// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LoadBalancingListener.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PhotonCharacter.generated.h"

UCLASS()
class PHOTON_API APhotonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APhotonCharacter();

	void SetPlayer(ExitGames::LoadBalancing::Player* p) { mPlayer = p; }

	UFUNCTION(BlueprintCallable, Category = "Photon | Player")
		FString GetName() { return ToFString(mPlayer->getName()); }
	UFUNCTION(BlueprintCallable, Category = "Photon | Player")
		FString GetUserID() { return ToFString(mPlayer->getUserID()); }
	UFUNCTION(BlueprintCallable, Category = "Photon | Player")
		int GetPlayerNr() { 
			if (mPlayer)
			{
				return mPlayer->getNumber();
			}
			return 0;
		}
	UFUNCTION(BlueprintCallable, Category = "Photon | Player")
		bool GetIsInActive() { return mPlayer->getIsInactive(); }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	ExitGames::LoadBalancing::Player* mPlayer;

	FString ToFString(ExitGames::Common::JString jstr) {
		std::string cstr = jstr.UTF8Representation().cstr();
		return cstr.c_str();
	}
};
