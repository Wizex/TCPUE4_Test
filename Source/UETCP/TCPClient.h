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
	FTCPClient(const FString& Ip, const uint32 Port);
	FTCPClient(FSocket* Socket);

	FTCPClient() = delete;
	~FTCPClient();

	bool Receive();
	bool SendMsg(FData& Data);
	bool Connect();
	bool Disconnect();
	void SetSocket(FSocket* Socket);
	
private:
	bool SendNonBlocking(const uint8* Msg, const int32 Size);
	void Destroy();
	void Setup(const FString& Ip, const uint32 Port);
	void ParseData();
	
	FOnClientDataReceived& OnClientDataReceived() { return mOnClientDataReceived; }

	TArray<uint8> mCachedData;

	int mNumBytesToReceive = 0;

	FSocket* mClientSocket = nullptr;

	TSharedPtr<FInternetAddr> mInternetAddr;

	FOnClientDataReceived mOnClientDataReceived;
};
