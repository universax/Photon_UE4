// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ListnerBase.h"
#include "LoadBalancingListener.h"
#include "Console.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhotonActor.generated.h"

// Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FJoinRoomDelegate, int32, playerNr, FString, playerName, bool, local);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPlayerTransformDelegate, int32, playerNr, FVector, pos, FRotator, rot);

UCLASS()
class PHOTON_API APhotonActor : public AActor, public ListnerBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APhotonActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



public:
	// Prop
	UPROPERTY(EditAnywhere, Category = "Photon | Common")
	FString ServerAddress;
	UPROPERTY(EditAnywhere, Category = "Photon | Common")
	FString AppID;
	UPROPERTY(EditAnywhere, Category = "Photon | Common")
	FString AppVersion;

	// Func
	UFUNCTION(BlueprintCallable, Category = "Photon | Common")
		void Setup();
	UFUNCTION(BlueprintCallable, Category = "Photon | Common")
		void Update();
	// For Debug
	UFUNCTION(BlueprintCallable, Category = "Photon | Debug")
		void CreateRoom(FString name);
	UFUNCTION(BlueprintCallable, Category = "Photon | Debug")
		TArray<FString> GetRoomList();
	UFUNCTION(BlueprintCallable, Category = "Photon | Debug")
		void JoinRoom(FString name);
	UFUNCTION(BlueprintCallable, Category = "Photon | Debug")
		bool GetIsInRoom() { return mpClient->getIsInGameRoom(); }


	// Callback from Listner
	UPROPERTY(BlueprintAssignable, Category = "Photon | Callback")
	FJoinRoomDelegate OnJoinRoomEventDelegate;
	virtual void OnJoinRoomEventAction(int playerNr, const ExitGames::Common::JString& playerName, bool local) {
		Console::get().writeLine(L"OnJoinRoomEventAction");
		if (!local)
		{
			// Spawn
			Console::get().writeLine(L"Spawn Remote Player.");
			OnJoinRoomEventDelegate.Broadcast(playerNr, ToFString(playerName), local);
		}
	}
	virtual void OnLeaveRoomEventAction(int playerNr) {
		Console::get().writeLine(L"OnLeaveRoomEventAction");
	}
	UPROPERTY(BlueprintAssignable, Category = "Photon | Callback")
		FPlayerTransformDelegate OnPlayerTransformDelegate;
	virtual void OnChangePlayerPos(int playerNr, float x, float y, float z, float qx, float qy, float qz, float qw) {
		//Console::get().writeLine(L"OnChangePlayerPos" + ToJString(FString::SanitizeFloat(x)) + ", " + ToJString(FString::SanitizeFloat(y)) + ", " + ToJString(FString::SanitizeFloat(z)));
		FVector p(x, y, z);
		FQuat q(qx, qy, qz, qw);
		FRotator r(q);
		OnPlayerTransformDelegate.Broadcast(playerNr, p, r);
	}

	// Send local player event
	// Spawn
	// Transform
	UFUNCTION(BlueprintCallable, Category = "Photon | Debug")
		void SendLocalTransform(FTransform transform);

	// Receive other player event
	// When other player joined
	// Spawn

	// Transform


private:
	ExitGames::LoadBalancing::Client* mpClient;
	LoadBalancingListener* mpListener;

	// Flags
	bool mIsJoinedRoom;

	// Util
	ExitGames::Common::JString ToJString(FString fstr);
	FString ToFString(ExitGames::Common::JString jstr);

};
