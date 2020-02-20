// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "Ticker.h"
#include "Data.h"

DECLARE_DELEGATE_OneParam(FOnClientDataReceived, const FData&);

class UETCP_API FTCPServer
{
public:
    FTCPServer(const FString&, const uint32);

	~FTCPServer();
	
	bool Bind();
	bool Listen(const uint32);
	bool Disconnect();
	bool OnTick(float);

private:
	bool TryAccept();
	bool Setup(const FString&, const uint32);

	bool Receive();

	bool bAccepted;

	TArray<uint8> mCachedData;

	int mNumBytesToReceive;
	
	FSocket* mServerSocket;
	FSocket* mAcceptedSocket;
	TSharedPtr<FInternetAddr> mInternetAddr;

	FTickerDelegate TickDelegate;
	
	FOnClientDataReceived mOnClientDataReceived;
};
