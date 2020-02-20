// Fill out your copyright notice in the Description page of Project Settings.

#include "TCPServer.h"
#include "MessageString.h"
#include "Networking/Public/Common/TcpSocketBuilder.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include <iostream>
#include "Networking/Public/Common/UdpSocketReceiver.h"

FTCPServer::FTCPServer(const FString& Ip, const uint32 Port) : bAccepted(false), mServerSocket(nullptr), mAcceptedSocket(nullptr), mNumBytesToReceive(0)
{
	if (Setup(Ip, Port) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to setup TCP socket"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Success to setup TCP socket"));
	}
}

bool FTCPServer::Setup(const FString& Ip, const uint32 Port)
{
	FIPv4Address Ipv4;
	FIPv4Address::Parse(Ip, Ipv4);

	mServerSocket = FTcpSocketBuilder("ServerSocket").AsReusable().AsNonBlocking();
	if (mServerSocket == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Socket is NULL"));
		return false;
	}
	mInternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	mInternetAddr->SetIp(Ipv4.Value);
	mInternetAddr->SetPort(Port);

	FTicker& Ticker = FTicker::GetCoreTicker();
	TickDelegate = FTickerDelegate::CreateRaw(this, &FTCPServer::OnTick);
	Ticker.AddTicker(TickDelegate, 0.0f);

	return true;
}

bool FTCPServer::Bind()
{
	if (mServerSocket == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Socket is NULL"));
		return false;
	}
	if (mServerSocket->Bind(*mInternetAddr) != true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to bind server"));
		UE_LOG(LogTemp, Log, TEXT("Socket closed"));
		Disconnect();
		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("Success to bind server"));
	return true;
}

bool FTCPServer::Listen(const uint32 BackLogs)
{
	if(mServerSocket->Listen(BackLogs) != true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to listen for connection"));
		UE_LOG(LogTemp, Log, TEXT("Socket closed"));
		Disconnect();
		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("Listen success"));
	return true;
}

bool FTCPServer::TryAccept()
{
	if (mServerSocket != nullptr) 
	{
		bool Pending;
		if (mServerSocket->WaitForPendingConnection(Pending, 0.1))
		{
			if (Pending)
			{
				FSocket* ConnectionSocket = mServerSocket->Accept(TEXT("TCP client"));

				if (ConnectionSocket != nullptr)
				{
					bAccepted = true;
					UE_LOG(LogTemp, Log, TEXT("Connection accepted"));

					mAcceptedSocket = ConnectionSocket;
				}
			}
		}
	}
	return true;
}

bool FTCPServer::Receive()
{
	if (mAcceptedSocket == nullptr)
	{
		return false;
	}
	uint32 PendingData = -1;

	int32 ReadBytes = 0;
	
	if (mAcceptedSocket->HasPendingData(PendingData))
	{
		if (mNumBytesToReceive <= 0)
		{
			if (PendingData >= sizeof(mNumBytesToReceive))
			{
				if (mAcceptedSocket->Recv((uint8*)(&mNumBytesToReceive), sizeof(mNumBytesToReceive), ReadBytes))
				{
					mCachedData.SetNum(mNumBytesToReceive);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to recieve data"));
					return false;
				}
			}
		}
		
		while (ReadBytes > 0 && mNumBytesToReceive > 0)
		{
			if (!mAcceptedSocket->Recv(mCachedData.GetData() + mCachedData.Num() - mNumBytesToReceive, mNumBytesToReceive, ReadBytes))
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to recieve data"));
				return false;
			}
			mNumBytesToReceive -= ReadBytes;
		}
	}

	if (mNumBytesToReceive <= 0 && ReadBytes > 0) 
	{
		UE_LOG(LogTemp, Log, TEXT("Data received"));
		
		FData DataStruct;

		FArrayReader Reader;
		Reader.Append(mCachedData);

		Reader << DataStruct; 

		FArrayReader R2;
		R2.Append(DataStruct.SerializedData);
		
		FMessageString Msg;
		R2 << Msg;

		UE_LOG(LogTemp, Log, TEXT("Message:%s"), *Msg.Str);
		
		mOnClientDataReceived.ExecuteIfBound(DataStruct);

		mCachedData.Reset();
	}
	return true;
}

bool FTCPServer::OnTick(float DeltaTime)
{
	TryAccept();
	Receive();
	
	return true;
}

bool FTCPServer::Disconnect()
{
	if (mServerSocket != nullptr)
	{
		if (!mServerSocket->Close())
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to close socket"));
			return false;
		}
		UE_LOG(LogTemp, Log, TEXT("Socket closed"));
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(mServerSocket);
		mServerSocket = nullptr;
		return false;
	}
	return false;
}

FTCPServer::~FTCPServer()
{
	FTicker::GetCoreTicker().RemoveTicker(TickDelegate.GetHandle());
}



