// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotonActor.h"

#include "Photon.h"


using namespace ExitGames::Common;
using namespace ExitGames::LoadBalancing;


// Sets default values
APhotonActor::APhotonActor()
	:ServerAddress("ns.exitgames.com"),
	AppID("48f4b761-26cd-49a4-841f-aa0bf81124cf"),
	AppVersion("1.0")
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APhotonActor::BeginPlay()
{
	Super::BeginPlay();
	Setup();
}

// Called every frame
void APhotonActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Update();
}

void APhotonActor::Setup()
{
	ServerAddress = ServerAddress;
	AppID = AppID;
	AppVersion = AppVersion;

	mpListener = new LoadBalancingListener(this);
	mpClient = new ExitGames::LoadBalancing::Client(*mpListener, TCHAR_TO_UTF8(*AppID), TCHAR_TO_UTF8(*AppVersion), ExitGames::Photon::ConnectionProtocol::DEFAULT, false, RegionSelectionMode::BEST);
	mpListener->setLBC(mpClient);

	Console::get().writeLine(L"Connecting...");
	Console::get().writeLine(L"appID is set to " + ExitGames::Common::JString(TCHAR_TO_UTF8(*AppID)));
	bool connect = mpClient->connect(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString(L"UR") + GETTIMEMS()), ExitGames::Common::JString(L"UR") + GETTIMEMS(), TCHAR_TO_UTF8(*ServerAddress));
}

void APhotonActor::Update()
{
	// Need
	if (mpClient)
	{
		mpClient->service();
		mpListener->service();
	}
}

void APhotonActor::CreateRoom(FString name)
{
	mpListener->createRoom(ExitGames::Common::JString(TCHAR_TO_UTF8(*name)));
}

TArray<FString> APhotonActor::GetRoomList()
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

void APhotonActor::JoinRoom(FString name)
{
	mpClient->opJoinRoom(ToJString(name));
}

void APhotonActor::LeaveRoom()
{
	mpClient->opLeaveRoom();
}

void APhotonActor::SendLocalTransform(FTransform transform)
{
	// Location
	FVector location = transform.GetLocation();
	// Rotation
	FQuat rotation = transform.GetRotation();

	Hashtable data;
	float posture[] = { 
		static_cast<float>(location.X),
		static_cast<float>(location.Y),
		static_cast<float>(location.Z),
		static_cast<float>(rotation.X),
		static_cast<float>(rotation.Y),
		static_cast<float>(rotation.Z),
		static_cast<float>(rotation.W)
	};
	data.put((nByte)1, posture, 7);
	mpClient->opRaiseEvent(false, data, 2);
}


// ------------------------------------------------------------
JString APhotonActor::ToJString(FString fstr) {
	return ExitGames::Common::JString(TCHAR_TO_UTF8(*fstr));
}

FString APhotonActor::ToFString(ExitGames::Common::JString jstr) {
	std::string cstr = jstr.UTF8Representation().cstr();
	return cstr.c_str();
}