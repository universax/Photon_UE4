// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common-cpp/inc/Common.h"

/**
 * 
 */
class ListnerBase
{
public:
	virtual ~ListnerBase(void) {}
	//virtual void updateState(int state, const ExitGames::Common::JString& stateStr, const ExitGames::Common::JString& joinedRoomName) = 0;
	virtual void initPlayers(void) {}
	virtual void OnJoinRoomEventAction(int playerNr, const ExitGames::Common::JString& playerName, bool local) = 0;
	virtual void OnLeaveRoomEventAction(int playerNr) = 0;
	//virtual void changePlayerColor(int playerNr, int color) = 0;
	virtual void OnChangePlayerPos(int playerNr, float x, float y, float z, float qx, float qy, float qz, float qw) = 0;
	//virtual void setupScene(int gridSize) = 0;
	//virtual void updateRoomList(const ExitGames::Common::JVector<ExitGames::Common::JString>& roomNames) = 0;
};