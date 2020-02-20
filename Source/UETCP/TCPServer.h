// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "Ticker.h"
#include "TCPWrapper.h"

class UETCP_API FTCPServer : public FTCPWrapper
{
public:
    FTCPServer(const FString&, const uint32);
	
	FTCPServer() = delete;
	~FTCPServer();
	
	bool Bind();
	bool Listen(const uint32);
	bool OnTick(float);
	bool Disconnect() final;

private:
	bool TryAccept();

	FSocket* mServerSocket;

	FTickerDelegate TickDelegate;
};
