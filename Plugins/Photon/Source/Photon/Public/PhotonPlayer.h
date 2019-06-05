// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LoadBalancingListener.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhotonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PHOTON_API UPhotonPlayer : public UObject
{
	GENERATED_BODY()
public:
	UPhotonPlayer(const FObjectInitializer& ObjectInitializer):
		Super(ObjectInitializer)
	{}
	~UPhotonPlayer(){}

	void SetPlayer(ExitGames::LoadBalancing::Player* p) { mPlayer = p; }

	UFUNCTION(BlueprintCallable, Category = "Photon | Player")
		FString GetName() { return ToFString(mPlayer->getName()); }
	UFUNCTION(BlueprintCallable, Category = "Photon | Player")
		FString GetUserID() { return ToFString(mPlayer->getUserID()); }
	UFUNCTION(BlueprintCallable, Category = "Photon | Player")
		int GetPlayerNr() { return mPlayer->getNumber(); }
	UFUNCTION(BlueprintCallable, Category = "Photon | Player")
		bool GetIsInActive() { return mPlayer->getIsInactive(); }

private:
	ExitGames::LoadBalancing::Player* mPlayer;

	FString ToFString(ExitGames::Common::JString jstr) {
		std::string cstr = jstr.UTF8Representation().cstr();
		return cstr.c_str();
	}
};
