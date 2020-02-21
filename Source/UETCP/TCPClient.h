// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core.h"
#include "Converter.h"
#include "Sockets.h"

DECLARE_DELEGATE_OneParam(FOnClientDataReceived, const FData&);

class UETCP_API FTCPClient
{
public:
	FTCPClient(const FString&, const uint32);
	FTCPClient(FSocket*);
	
	~FTCPClient();

	bool Receive();
	bool SendMsg(FData&);
	bool Connect();
	bool Disconnect();

private:
	FTCPClient();
	
	bool SendNonBlocking(const uint8*, const int32);
	void Destroy();
	bool Setup(const FString&, const uint32);
	bool Setup(FSocket*);
	void ParseData();
	
	FOnClientDataReceived& OnClientDataReceived() { return mOnClientDataReceived; }

	TArray<uint8> mCachedData;

	int mNumBytesToReceive;

	FSocket* mClientSocket;

	TSharedPtr<FInternetAddr> mInternetAddr;

	FOnClientDataReceived mOnClientDataReceived;
};
