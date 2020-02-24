// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TCPClient.h"

class UETCP_API FTCPClientWrapper : public FTCPClient
{
public:
	FTCPClientWrapper() = delete;
	FTCPClientWrapper(const FString& Ip, const uint32 Port);
	
	~FTCPClientWrapper();

public:
	bool OnTick(float DeltaTime);

private:
	FTickerDelegate TickDelegate;
};
