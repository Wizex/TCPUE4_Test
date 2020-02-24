// Fill out your copyright notice in the Description page of Project Settings.

#include "TCPServer.h"
#include "MessageString.h"
#include "Networking/Public/Common/TcpSocketBuilder.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"

FTCPServer::FTCPServer(const FString& Ip, const uint32 Port) : mServerSocket(nullptr), mClient(nullptr)
{
	SetupSocket(Ip, Port);

	mServerSocket = FTcpSocketBuilder("Socket").AsReusable().AsNonBlocking();

	check(mServerSocket);

	FTicker& Ticker = FTicker::GetCoreTicker();
	mTickDelegate = FTickerDelegate::CreateRaw(this, &FTCPServer::OnTick);
	Ticker.AddTicker(mTickDelegate, 0.0f);
}

void FTCPServer::SetupSocket(const FString& Ip, const uint32 Port)
{
	FIPv4Address Ipv4;
	FIPv4Address::Parse(Ip, Ipv4);

	mInternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	mInternetAddr->SetIp(Ipv4.Value);
	mInternetAddr->SetPort(Port);
}

void FTCPServer::Bind()
{
	if (!mServerSocket->Bind(*mInternetAddr) || mServerSocket == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to bind server"));
		UE_LOG(LogTemp, Log, TEXT("Socket closed"));
		CloseConnections();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Success to bind server"));
	}
}

void FTCPServer::Listen(const uint32 BackLogs)
{
	if (!mServerSocket->Listen(BackLogs))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to listen for connection"));
		UE_LOG(LogTemp, Log, TEXT("Socket closed"));
		CloseConnections();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Listen success"));
	}
}

bool FTCPServer::SendMsg(FData& Data)
{
	return mClient.SendMsg(Data);
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
					UE_LOG(LogTemp, Log, TEXT("Connection accepted"));

					mClient.SetSocket(ConnectionSocket);
				}
			}
		}
	}
	return true;
}

bool FTCPServer::OnTick(float DeltaTime)
{
	TryAccept();
	mClient.Receive();
	return true;
}

void FTCPServer::Destroy()
{
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(mServerSocket);
	mServerSocket = nullptr;
}

bool FTCPServer::CloseConnections()
{
	mClient.Disconnect();
	if (!mServerSocket->Close())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to close socket"));
		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("Socket closed"));
	Destroy();
	return true;
}

FTCPServer::~FTCPServer()
{
	FTicker::GetCoreTicker().RemoveTicker(mTickDelegate.GetHandle());
	if (mServerSocket != nullptr) 
	{
		Destroy();
	}
}




