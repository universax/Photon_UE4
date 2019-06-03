// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LoadBalancingListener.h"
#include "PhotonListnerComponent.h"
#include "PhotonActor.generated.h"

UCLASS()
class PHOTON_API APhotonActor : public AActor
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

	// Send local player event
	// Spawn

	// Transform
	UFUNCTION(BlueprintCallable, Category = "Photon | Debug")
		void SendLocalTransform(FVector pos);

	// Receive other player event
	// When other player joined
	// Spawn

	// Transform


private:
	UPhotonListnerComponent *listnerComponent = nullptr;

	ExitGames::LoadBalancing::Client* mpClient;
	LoadBalancingListener* mpListener;

	// Flags
	bool mIsJoinedRoom;

	// Util
	ExitGames::Common::JString ToJString(FString fstr);
	FString ToFString(ExitGames::Common::JString jstr);

};
