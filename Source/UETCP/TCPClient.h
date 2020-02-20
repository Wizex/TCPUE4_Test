// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core.h"
#include "Converter.h"
#include "TCPWrapper.h"

class UETCP_API FTCPClient : public FTCPWrapper
{
public:
	FTCPClient(const FString&, const uint32);
	
	FTCPClient() = delete;
	~FTCPClient() = default;

	bool SendMsg(FData&);
	bool Connect();
	bool Disconnect() final;

private:

	bool SendNonBlocking(const uint8*, const int32);
};
