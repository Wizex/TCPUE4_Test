// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "Ticker.h"
#include "TCPClient.h"

class UETCP_API FTCPServer
{
public:
    FTCPServer(const FString& Ip, const uint32 Port);
	
	FTCPServer() = delete;
	~FTCPServer();
	
	void Bind();
	void Listen(const uint32 Backlogs);
	bool OnTick(float DeltaTime);
	bool CloseConnections();
	bool SendMsg(FData& Data);
	void SetupSocket(const FString& Ip, const uint32 Port);

private:
	bool TryAccept();
	void Destroy();
	
	FSocket* mServerSocket;

	FTickerDelegate TickDelegate;

	TSharedPtr<FInternetAddr> mInternetAddr;

	FTCPClient mClient;
};
