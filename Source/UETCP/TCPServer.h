// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "Ticker.h"
#include "TCPClient.h"

class UETCP_API FTCPServer
{
public:
    FTCPServer(const FString&, const uint32);
	
	FTCPServer() = delete;
	~FTCPServer();
	
	bool Bind();
	bool Listen(const uint32);
	bool OnTick(float);
	bool Disconnect();
	bool SendMsg(FData&);

private:
	bool TryAccept();
	void Destroy();

	bool bAccepted;
	
	FSocket* mServerSocket;

	FTickerDelegate TickDelegate;

	TSharedPtr<FInternetAddr> mInternetAddr;

	FTCPClient* mClient;
};
