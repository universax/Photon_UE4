#include "LoadBalancingListener.h"
#include <iostream>
#include <stdlib.h>
#include "Console.h"

#ifdef _EG_ANDROID_PLATFORM
#	include <stdarg.h>
#	include <android/log.h>
#endif

using namespace ExitGames::Common;
using namespace ExitGames::LoadBalancing;

static int randomColor(int from=0, int to=256)
{
	int r = from + rand() % (to - from);
	int g = from + rand() % (to - from);
	int b = from + rand() % (to - from);
	return (r << 16) + (g << 8) + b;
}

const JString PeerStatesStr[] = {
	L"Uninitialized",
	L"PeerCreated",
	L"ConnectingToNameserver",
	L"ConnectedToNameserver",
	L"DisconnectingFromNameserver",
	L"Connecting",
	L"Connected",
	L"WaitingForCustomAuthenticationNextStepCall",
	L"Authenticated",
	L"JoinedLobby",
	L"DisconnectingFromMasterserver",
	L"ConnectingToGameserver",
	L"ConnectedToGameserver",
	L"AuthenticatedOnGameServer",
	L"Joining",
	L"Joined",
	L"Leaving",
	L"Left",
	L"DisconnectingFromGameserver",
	L"ConnectingToMasterserver",
	L"ConnectedComingFromGameserver",
	L"AuthenticatedComingFromGameserver",
	L"Disconnecting",
	L"Disconnected"
};

// TODO: update PeerStatesStr when PeerStates changes
// Checker below checks count match only
class PeerStatesStrChecker
{
public:
	PeerStatesStrChecker(void)
	{
		DEBUG_ASSERT(sizeof(PeerStatesStr)/sizeof(JString) == PeerStates::Disconnected + 1);
	}
} checker;

LocalPlayer::LocalPlayer(void) : x(0), y(0), color(randomColor(100)), lastUpdateTime(0)
{
	
}

LoadBalancingListener::LoadBalancingListener(ListnerBase* listnerBase) : ue4Listner(listnerBase), mpLbc(NULL), mLocalPlayerNr(0)
{
}

LoadBalancingListener::~LoadBalancingListener(void)
{
}

void LoadBalancingListener::setLBC(ExitGames::LoadBalancing::Client* pLbc)
{
	this->mpLbc = pLbc;
}

void LoadBalancingListener::connect(const JString& userName)
{
	mpLbc->connect(AuthenticationValues().setUserID(JString()+GETTIMEMS()), userName);
}


void LoadBalancingListener::debugReturn(int debugLevel, const JString& string)
{
	Console::get().debugReturn(debugLevel, string);
}

void LoadBalancingListener::connectionErrorReturn(int errorCode)
{
	Console::get().writeLine(JString(L"connection failed with error ") + errorCode);
	updateState();
}

void LoadBalancingListener::clientErrorReturn(int errorCode)
{
	Console::get().writeLine(JString(L"received error ") + errorCode + L" from client");
	updateState();
}

void LoadBalancingListener::warningReturn(int warningCode)
{
	Console::get().writeLine(JString(L"received warning ") + warningCode + " from client");
}

void LoadBalancingListener::serverErrorReturn(int errorCode)
{
	Console::get().writeLine(JString(L"received error ") + errorCode + " from server");
	updateState();
}

void LoadBalancingListener::joinRoomEventAction(int playerNr, const JVector<int>& playernrs, const Player& player)
{
	Console::get().writeLine(JString("player ") + playerNr + L" " + player.getName() + L" has joined the game");
	ue4Listner->OnJoinRoomEventAction(playerNr, player.getName(), player.getNumber() == mpLbc->getLocalPlayer().getNumber());
}

void LoadBalancingListener::leaveRoomEventAction(int playerNr, bool isInactive)
{
	if(isInactive)
		Console::get().writeLine(JString(L"player ") + playerNr + L" has suspended the game");
	else
	{
		Console::get().writeLine(JString(L"player ") + playerNr + L" has abandoned the game");
		ue4Listner->OnLeaveRoomEventAction(playerNr);
	}
}

void LoadBalancingListener::customEventAction(int playerNr, nByte eventCode, const Object& eventContentObj)
{
	ExitGames::Common::Hashtable eventContent = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContentObj).getDataCopy();
	if(eventCode == 1)
	{
		Console::get().writeLine(L"Event 1");
		//Object const* obj = eventContent.getValue("1");
		//if(!obj) obj = eventContent.getValue((nByte)1);
		//if(!obj) obj = eventContent.getValue(1);
		//if(!obj) obj = eventContent.getValue(1.0);
		//if(obj)
		//{
		//	int color = (int)(obj->getType() == TypeCode::DOUBLE ?  ((ValueObject<double>*)obj)->getDataCopy() : ((ValueObject<int>*)obj)->getDataCopy());
		//}
		//else
		//	Console::get().writeLine(L"bad color event");
	}
	else if(eventCode == 2)
	{
		//Console::get().writeLine(L"Event 2");
		Object const* obj = eventContent.getValue("1");
		if(!obj) obj = eventContent.getValue((nByte)1);
		if(!obj) obj = eventContent.getValue(1);
		if(!obj) obj = eventContent.getValue(1.0);
		if(obj && obj->getDimensions() == 1 && obj->getSizes()[0] == 7)
		{
			float x = 0; float y = 0; float z = 0;
			float qx(0), qy(0), qz(0), qw(0);
			if (obj->getType() == TypeCode::EG_FLOAT)
			{
				float* data = ((ValueObject<float*>*)obj)->getDataCopy();
				x = (float)data[0];
				y = (float)data[1];
				z = (float)data[2];
				qx = (float)data[3];
				qy = (float)data[4];
				qz = (float)data[5];
				qw = (float)data[6];
			}
			ue4Listner->OnChangePlayerPos(playerNr, x, y, z, qx, qy, qz, qw);
		}
		else
			Console::get().writeLine(L"Bad position event");
	}
}

void LoadBalancingListener::connectReturn(int errorCode, const JString& errorString, const JString& region, const JString& cluster)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"connected to cluster " + cluster + L" of region " + region);
		//mpLbc->opJoinRandomRoom();
	}
	else
		Console::get().writeLine(JString(L"Warn: connect failed ") + errorCode + L" " + errorString);
}

void LoadBalancingListener::disconnectReturn(void)
{
	updateState();
	Console::get().writeLine(L"disconnected");
}

void LoadBalancingListener::createRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"room has been created");
		//afterRoomJoined(localPlayerNr);
	}
	else
		Console::get().writeLine(L"Warn: opCreateRoom() failed: " + errorString);
}

void LoadBalancingListener::joinOrCreateRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"room has been entered");
		//afterRoomJoined(localPlayerNr);
	}
	else
		Console::get().writeLine(L"Warn: opJoinOrCreateRoom() failed: " + errorString);
}

void LoadBalancingListener::joinRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"game room has been successfully joined");
		afterRoomJoined(localPlayerNr);
	}
	else
		Console::get().writeLine(L"opJoinRoom() failed: " + errorString);
}

void LoadBalancingListener::joinRandomRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::NO_MATCH_FOUND)
		createRoom();
	else if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"game room has been successfully joined");
		afterRoomJoined(localPlayerNr);
	}
	else
		Console::get().writeLine(L"opJoinRandomRoom() failed: " + errorString);
}

void LoadBalancingListener::leaveRoomReturn(int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
		Console::get().writeLine(L"game room has been successfully left");
	else
		Console::get().writeLine(L"opLeaveRoom() failed: " + errorString);
}

void LoadBalancingListener::gotQueuedReturn(void)
{
	updateState();
}

void LoadBalancingListener::joinLobbyReturn(void)
{
	Console::get().writeLine(L"joined lobby");
	updateState();
}

void LoadBalancingListener::leaveLobbyReturn(void)
{
	Console::get().writeLine(L"left lobby");
	updateState();
}

void LoadBalancingListener::onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>&, const ExitGames::Common::JVector<ExitGames::Common::JString>&)
{
	Console::get().writeLine(L"===================== onAvailableRegions");

}

void LoadBalancingListener::onLobbyStatsUpdate(const JVector<LobbyStatsResponse>& res)
{
	Console::get().writeLine(L"===================== lobby stats update");
	for(unsigned int i=0; i<res.getSize(); ++i)
		Console::get().writeLine(res[i].toString());

	// lobby stats request test
	JVector<LobbyStatsRequest> ls;
	ls.addElement(LobbyStatsRequest());
	ls.addElement(LobbyStatsRequest(L"AAA"));
	ls.addElement(LobbyStatsRequest(L"BBB"));
	ls.addElement(LobbyStatsRequest(L"CCC", LobbyType::DEFAULT));
	ls.addElement(LobbyStatsRequest(L"AAA", LobbyType::SQL_LOBBY));
	mpLbc->opLobbyStats(ls);
}

void LoadBalancingListener::onLobbyStatsResponse(const ExitGames::Common::JVector<LobbyStatsResponse>& res)
{
	Console::get().writeLine(L"===================== lobby stats response");
	for(unsigned int i=0; i<res.getSize(); ++i)
		Console::get().writeLine(res[i].toString());
}

void LoadBalancingListener::onRoomListUpdate()
{
	const JVector<Room*>& rooms = mpLbc->getRoomList();
	JVector<JString> names(rooms.getSize());
	for(unsigned int i=0; i<rooms.getSize(); ++i)
		names.addElement(rooms[i]->getName());
}

void LoadBalancingListener::onRoomPropertiesChange(const ExitGames::Common::Hashtable& changes)
{
	
}

void LoadBalancingListener::updateState()
{
	int state = mpLbc->getState();
}

bool LoadBalancingListener::updateGridSize(const ExitGames::Common::Hashtable& props)
{
	if(props.contains(L"s"))
	{
		const Object* v = props.getValue(L"s");
		switch(v->getType())
		{
		case TypeCode::INTEGER:
			mGridSize = ((ValueObject<int>*)v)->getDataCopy();
			return true;
		case TypeCode::DOUBLE:
			mGridSize = (int)((ValueObject<double>*)v)->getDataCopy();
			return true;
		}
	}
	return false;
}

void LoadBalancingListener::afterRoomJoined(int localPlayerNr)
{
	Console::get().writeLine(JString(L"afterRoomJoined: localPlayerNr=") + localPlayerNr);
	this->mLocalPlayerNr = localPlayerNr;
	MutableRoom& myRoom = mpLbc->getCurrentlyJoinedRoom();
	Hashtable props = myRoom.getCustomProperties();
	updateGridSize(props);
	if(props.contains(L"m"))
		mMap = ((ValueObject<JString>*)props.getValue(L"m"))->getDataCopy();


	const JVector<Player*>& players = myRoom.getPlayers();
	for(unsigned int i=0; i<players.getSize(); ++i)
	{
		const Player* p = players[i];
	}
	raiseColorEvent();
}

void LoadBalancingListener::createRoom()
{
	JString name = 
#ifdef __UNREAL__
		JString(L"UE-")
#else
		JString(L"native-")
#endif
		 + (rand()%100);
	Hashtable props;
	props.put(L"s", mGridSize);
	props.put(L"m", mMap);
	mpLbc->opCreateRoom(name, RoomOptions().setCustomRoomProperties(props));
	Console::get().writeLine(L"Creating room " + name);
}

void LoadBalancingListener::createRoom(JString roomName)
{
	Hashtable props;
	props.put(L"s", mGridSize);
	props.put(L"m", mMap);
	mpLbc->opCreateRoom(roomName, RoomOptions().setCustomRoomProperties(props));
	Console::get().writeLine(L"Creating room " + roomName);
}

void LoadBalancingListener::service(void)
{
	//unsigned long t = GETTIMEMS();
	//if((t - mLocalPlayer.lastUpdateTime) > PLAYER_UPDATE_INTERVAL_MS)
	//{
	//	mLocalPlayer.lastUpdateTime = t;
	//	if(mpLbc->getState() == PeerStates::Joined && mAutomove)
	//		moveLocalPlayer();
	//}
}

void LoadBalancingListener::moveLocalPlayer(void)
{
	//setLocalPlayerPos(x, y);
}

bool LoadBalancingListener::setLocalPlayerPos(int x, int y)
{
//	if(x >= 0 && x < mGridSize && y >= 0 && y < mGridSize)
//	{
//		int prevGroup = getGroupByPos();
//		mLocalPlayer.x = x;
//		mLocalPlayer.y = y;
//		if(prevGroup != getGroupByPos())
//			updateGroups();
//		Hashtable data;
//
//#ifdef __UNREAL__
//		nByte coords[] ={static_cast<nByte>(mLocalPlayer.y), static_cast<nByte>(mGridSize-1-mLocalPlayer.x)}; // convert from engine to demo coordinate system on send (necessary so that builds of this demo for different game engines are compatible to each other)
//#else
//		nByte coords[] ={static_cast<nByte>(mLocalPlayer.x), static_cast<nByte>(mLocalPlayer.y)};
//#endif
//		data.put((nByte)1, coords, 2);
//
//
//		mpLbc->opRaiseEvent(false, data, 2, RaiseEventOptions().setInterestGroup(mSendGroup?mSendGroup:mUseGroups?getGroupByPos():0));
//		return true;
//	}
//	else
		return false;
}

void LoadBalancingListener::raiseColorEvent(void)
{
	Hashtable data;
	data.put((nByte)1, mLocalPlayer.color);
	//mpLbc->opRaiseEvent(true, data, 1, RaiseEventOptions().setEventCaching(ExitGames::Lite::EventCache::ADD_TO_ROOM_CACHE).setInterestGroup(mSendGroup?mSendGroup:mUseGroups?getGroupByPos():0));
}
