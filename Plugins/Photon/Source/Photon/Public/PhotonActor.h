// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Photon.h"

#include "ListnerBase.h"
#include "LoadBalancingListener.h"
#include "Console.h"

#include "PhotonCharacter.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhotonActor.generated.h"

// Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FJoinRoomDelegate, APhotonCharacter*, JoinedPlayer, bool, IsLocal, const TArray<APhotonCharacter*>&, PlayerList);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPlayerTransformDelegate, int32, playerNr, FVector, pos, FRotator, rot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLeaveRoomDelegate, int32, playerNr);


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

	// Room
	UFUNCTION(BlueprintCallable, Category = "Photon | Room")
		void CreateRoom(FString name);
	UFUNCTION(BlueprintCallable, Category = "Photon | Room")
		TArray<FString> GetRoomList();
	UFUNCTION(BlueprintCallable, Category = "Photon | Room")
		void JoinRoom(FString name);
	UFUNCTION(BlueprintCallable, Category = "Photon | Room")
		void LeaveRoom();
	UFUNCTION(BlueprintCallable, Category = "Photon | Room")
		bool GetIsInRoom() { return mpClient->getIsInGameRoom(); }

	// Player
	UFUNCTION(BlueprintCallable, Category = "Photon | Player")
		void SendLocalTransform(FTransform transform);

	// ------------------------------------------------- Callback from Listner
	UPROPERTY(BlueprintAssignable, Category = "Photon | Callback")
	FJoinRoomDelegate OnJoinRoomEventDelegate;
	virtual void OnJoinRoomEventAction(int playerNr, const ExitGames::Common::JString& playerName, bool local) {
		Console::get().writeLine(L"OnJoinRoomEventAction");

		APhotonCharacter* joinedPlayer = NewObject<APhotonCharacter>();
		TArray<APhotonCharacter*> photonPlayers;


		ExitGames::LoadBalancing::MutableRoom room = mpClient->getCurrentlyJoinedRoom();
		ExitGames::Common::JVector<ExitGames::LoadBalancing::Player*> players = room.getPlayers();
		for (int i = 0; i < (int)room.getPlayerCount(); i++) {
			APhotonCharacter* p = NewObject<APhotonCharacter>();
			p->SetPlayer(players[i]);
			photonPlayers.AddUnique(p);

			// It's me
			if (playerNr == players[i]->getNumber())
			{
				joinedPlayer->SetPlayer(players[i]);
			}
		}
		OnJoinRoomEventDelegate.Broadcast(joinedPlayer, local, photonPlayers);
	}
	UPROPERTY(BlueprintAssignable, Category = "Photon | Callback")
	FLeaveRoomDelegate OnLeaveRoomEventDelegate;
	virtual void OnLeaveRoomEventAction(int playerNr) {
		Console::get().writeLine(L"OnLeaveRoomEventAction");
		OnLeaveRoomEventDelegate.Broadcast(playerNr);
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
	// -------------------------------------------------

	

private:
	ExitGames::LoadBalancing::Client* mpClient;
	LoadBalancingListener* mpListener;

	// Flags
	bool mIsJoinedRoom;

	// Util
	ExitGames::Common::JString ToJString(FString fstr);
	FString ToFString(ExitGames::Common::JString jstr);

};
