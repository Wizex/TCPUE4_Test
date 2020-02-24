// Fill out your copyright notice in the Description page of Project Settings.

#include "TCPClientWrapper.h"
#include "Ticker.h"

FTCPClientWrapper::FTCPClientWrapper(const FString& Ip, const uint32 Port) : FTCPClient(Ip, Port)
{
	FTicker& Ticker = FTicker::GetCoreTicker();
	TickDelegate = FTickerDelegate::CreateRaw(this, &FTCPClientWrapper::OnTick);
	Ticker.AddTicker(TickDelegate, 0.0f);
}

bool FTCPClientWrapper::OnTick(float DeltaTime)
{
	return Receive();
}

FTCPClientWrapper::~FTCPClientWrapper()
{
	FTicker::GetCoreTicker().RemoveTicker(TickDelegate.GetHandle());
}

