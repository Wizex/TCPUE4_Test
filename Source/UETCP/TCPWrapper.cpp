// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPWrapper.h"

#include "TCPClient.h"
#include "Networking/Public/Common/TcpSocketBuilder.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include <algorithm>
#include "MessageString.h"

FTCPWrapper::FTCPWrapper() : mNumBytesToReceive(0), mClientSocket(nullptr) {}

FTCPWrapper::FTCPWrapper(const FString& Ip, const uint32 Port) : FTCPWrapper()
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

bool FTCPWrapper::Setup(const FString& Ip, const uint32 Port)
{
	FIPv4Address Ipv4;
	FIPv4Address::Parse(Ip, Ipv4);

	mInternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	mInternetAddr->SetIp(Ipv4.Value);
	mInternetAddr->SetPort(Port);

	return true;
}

bool FTCPWrapper::Receive()
{
	if(mClientSocket == nullptr)
	{
		return false;
	}
	if (mClientSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket is not connected"));
		return false;
	}
	uint32 PendingData = -1;

	int32 ReadBytes = 0;

	if (mClientSocket->HasPendingData(PendingData))
	{
		if (mNumBytesToReceive <= 0)
		{
			if (PendingData >= sizeof(mNumBytesToReceive))
			{
				if (mClientSocket->Recv((uint8*)(&mNumBytesToReceive), sizeof(mNumBytesToReceive), ReadBytes))
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
			if (!mClientSocket->Recv(mCachedData.GetData() + mCachedData.Num() - mNumBytesToReceive, mNumBytesToReceive, ReadBytes))
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to recieve data"));
				return false;
			}
			mNumBytesToReceive -= ReadBytes;
		}
	}

	if (mNumBytesToReceive <= 0 && ReadBytes > 0)
	{
		ParseData();
	}
	return true;
}

void FTCPWrapper::ParseData()
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

void FTCPWrapper::Destroy(FSocket* socket)
{
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(mClientSocket);
	mClientSocket = nullptr;
}

FTCPWrapper::~FTCPWrapper()
{
	if (mClientSocket != nullptr) {
		Destroy(mClientSocket);
	}
}

