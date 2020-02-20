// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "Core.h"
#include "Converter.h"

DECLARE_DELEGATE_OneParam(FOnClientDataReceived, const FData&);

class UETCP_API FTCPClient
{
public:
	FTCPClient(const FString&, const uint32);

	bool Receive();
	bool SendMsg(FData&);
	bool Connect();
	bool Disconnect();
	bool Setup(const FString&, const uint32);

	FOnClientDataReceived& OnClientDataReceived() { return mOnClientDataReceived; }

protected:
	FTCPClient();

	bool SendNonBlocking(const uint8*, const int32);

	TArray<uint8> mCachedData;

	int mNumBytesToReceive;

	FSocket* mSocket;

	TSharedPtr<FInternetAddr> mInternetAddr;

	FOnClientDataReceived mOnClientDataReceived;
};
