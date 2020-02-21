// Fill out your copyright notice in the Description page of Project Settings.

#include "TCPServer.h"
#include "MessageString.h"
#include "Networking/Public/Common/TcpSocketBuilder.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Networking/Public/Common/UdpSocketReceiver.h"

FTCPServer::FTCPServer(const FString& Ip, const uint32 Port) : bAccepted(false), mServerSocket(nullptr), mClient(nullptr)
{
	FIPv4Address Ipv4;
	FIPv4Address::Parse(Ip, Ipv4);

	mInternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	mInternetAddr->SetIp(Ipv4.Value);
	mInternetAddr->SetPort(Port);
	
	mServerSocket = FTcpSocketBuilder("Socket").AsReusable().AsNonBlocking();
	if (mServerSocket == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Socket is NULL"));
	}
	
	FTicker& Ticker = FTicker::GetCoreTicker();
	TickDelegate = FTickerDelegate::CreateRaw(this, &FTCPServer::OnTick);
	Ticker.AddTicker(TickDelegate, 0.0f);
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

bool FTCPServer::SendMsg(FData& Data)
{
	return mClient->SendMsg(Data);
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

					if(mClient != nullptr)
					{
						mClient->Disconnect();
					}
					mClient = new FTCPClient(ConnectionSocket);
					bAccepted = true;
				}
			}
		}
	}
	return true;
}

bool FTCPServer::OnTick(float DeltaTime)
{
	TryAccept();
	if (bAccepted) 
	{
		mClient->Receive();
	}
	return true;
}

void FTCPServer::Destroy()
{
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(mServerSocket);
	mServerSocket = nullptr;
}

bool FTCPServer::Disconnect()
{
	if (mServerSocket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
	{
		if (!mServerSocket->Close())
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to close socket"));
			return false;
		}
		UE_LOG(LogTemp, Log, TEXT("Socket closed"));
		Destroy();
		return true;
	}
	return false;
}

FTCPServer::~FTCPServer()
{
	FTicker::GetCoreTicker().RemoveTicker(TickDelegate.GetHandle());
	if (mServerSocket != nullptr) 
	{
		Destroy();
	}
}




