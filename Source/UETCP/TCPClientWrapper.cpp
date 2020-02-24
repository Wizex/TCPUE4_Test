// Fill out your copyright notice in the Description page of Project Settings.

#include "TCPClientWrapper.h"
#include "Ticker.h"

FTCPClientWrapper::FTCPClientWrapper(const FString& Ip, const uint32 Port) : FTCPClient(Ip, Port)
{
	FTicker& Ticker = FTicker::GetCoreTicker();
	mTickDelegate = FTickerDelegate::CreateRaw(this, &FTCPClientWrapper::OnTick);
	Ticker.AddTicker(mTickDelegate, 0.0f);
}

bool FTCPClientWrapper::OnTick(float DeltaTime)
{
	Receive();
	return true;
}

FTCPClientWrapper::~FTCPClientWrapper()
{
	FTicker::GetCoreTicker().RemoveTicker(mTickDelegate.GetHandle());
}

