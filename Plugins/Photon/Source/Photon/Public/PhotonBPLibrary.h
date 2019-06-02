// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ModuleManager.h"
#include "Engine.h"
#include "LoadBalancingListener.h"
#include "PhotonBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS(ClassGroup = Photon, Blueprintable)
class UPhotonBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	// Func
	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Create Photon Object From Blueprint", CompactNodeTitle = "Create", Keywords = "new create blueprint"), Category = "Photon | Common")
		static UObject* NewObjectFromBlueprint(UObject* WorldContextObject, TSubclassOf<UObject> UC);

	UFUNCTION(BlueprintCallable, Category = "Photon | Common")
		void Setup(FString ServerAddress, FString AppID, FString AppVersion);

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
	ExitGames::LoadBalancing::Client* mpClient;
	LoadBalancingListener* mpListener;

	FString mServerAddress;
	FString mAppID;
	FString mAppVersion;
	
	// Flags
	bool mIsJoinedRoom;

	// Util
	ExitGames::Common::JString ToJString(FString fstr);
	FString ToFString(ExitGames::Common::JString jstr);
};
