// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PhotonBPLibrary.h"
#include "Photon.h"
#include "Console.h"

using namespace ExitGames::Common;
using namespace ExitGames::LoadBalancing;


UPhotonBPLibrary::UPhotonBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer),
mpClient(NULL),
mpListener(NULL)
{
	mServerAddress = TEXT("ns.exitgames.com");
	mAppID = TEXT("<no-app-id>");
	mAppVersion = TEXT("1.0");
}

UObject* UPhotonBPLibrary::NewObjectFromBlueprint(UObject* WorldContextObject, TSubclassOf<UObject> UC)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	return NewObject<UPhotonBPLibrary>(UC);
}

void UPhotonBPLibrary::Setup(FString ServerAddress, FString AppID, FString AppVersion)
{
	mServerAddress = ServerAddress;
	mAppID = AppID;
	mAppVersion = AppVersion;

	mpListener = new LoadBalancingListener();
	mpClient = new ExitGames::LoadBalancing::Client(*mpListener, TCHAR_TO_UTF8(*mAppID), TCHAR_TO_UTF8(*mAppVersion), ExitGames::Photon::ConnectionProtocol::DEFAULT, false, RegionSelectionMode::BEST);
	mpListener->setLBC(mpClient);

	Console::get().writeLine(L"Connecting...");
	Console::get().writeLine(L"appID is set to " + ExitGames::Common::JString(TCHAR_TO_UTF8(*AppID)));
	bool connect = mpClient->connect(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString(L"UR") + GETTIMEMS()), ExitGames::Common::JString(L"UR") + GETTIMEMS(), TCHAR_TO_UTF8(*mServerAddress));
}

void UPhotonBPLibrary::Update()
{
	// Need
	if (mpClient)
	{
		mpClient->service();
		mpListener->service();
	}
}

void UPhotonBPLibrary::CreateRoom(FString name)
{
	mpListener->createRoom(ExitGames::Common::JString(TCHAR_TO_UTF8(*name)));
}

TArray<FString> UPhotonBPLibrary::GetRoomList()
{
	const JVector<Room*>& rooms = mpClient->getRoomList();
	//const JVector<Room*>& rooms = mpListener->getClient()->getRoomList();
	TArray<FString> names;
	for (unsigned int i = 0; i < rooms.getSize(); ++i) {
		JString name = rooms[i]->getName();
		names.Add(ToFString(name));
		Console::get().writeLine(L"Room: " + name);
	}
	return names;
}

void UPhotonBPLibrary::JoinRoom(FString name)
{
	mpClient->opJoinRoom(ToJString(name));
}




// ------------------------------------------------------------
JString UPhotonBPLibrary::ToJString(FString fstr) {
	return ExitGames::Common::JString(TCHAR_TO_UTF8(*fstr));
}

FString UPhotonBPLibrary::ToFString(ExitGames::Common::JString jstr) {
	std::string cstr = jstr.UTF8Representation().cstr();
	return cstr.c_str();
}