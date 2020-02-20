// Fill out your copyright notice in the Description page of Project Settings.

#include "TCPClient.h"
#include "Networking/Public/Common/TcpSocketBuilder.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "BufferArchive.h"
#include <algorithm>
#include "MessageString.h"

FTCPClient::FTCPClient() : mNumBytesToReceive(0), mSocket(nullptr) {}

FTCPClient::FTCPClient(const FString& Ip, const uint32 Port) : FTCPClient()
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

bool FTCPClient::Setup(const FString& Ip, const uint32 Port)
{
	FIPv4Address Ipv4;
	FIPv4Address::Parse(Ip, Ipv4);

	mSocket = FTcpSocketBuilder("Socket").AsReusable().AsNonBlocking();
	if (mSocket == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Socket is NULL"));
		return false;
	}
	mInternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	mInternetAddr->SetIp(Ipv4.Value);
	mInternetAddr->SetPort(Port);

	return true;
}

bool FTCPClient::Connect()
{
	if (mSocket == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Socket is NULL"));
		return false;
	}
	if (mSocket->Connect(*mInternetAddr) != true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to connect socket"));
		UE_LOG(LogTemp, Log, TEXT("Socket closed"));
		return Disconnect();
	}
	UE_LOG(LogTemp, Log, TEXT("Success to connect socket"));
	return true;
}

bool FTCPClient::SendMsg(FData& data)
{
	if (mSocket == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Socket is NULL"));
		return false;
	}
	if (mSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket is not connected"));
		UE_LOG(LogTemp, Log, TEXT("Socket closed"));
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(mSocket);
		mSocket = nullptr;
		return false;
	}

	FArrayWriter Writer;

	FData::StaticStruct()->SerializeBin(Writer, &data);

	TArray<uint8> Data;
	Data.SetNum(4);
	const int32 Size = Writer.Num();
	std::copy((uint8*)&Size, (uint8*)(&Size) + 4, Data.GetData());
	Data.Append(Writer);

	return SendNonBlocking(Data.GetData(), Data.Num());
}

bool FTCPClient::SendNonBlocking(const uint8* Msg, const int32 Size)
{
	int32 BytesToSent = Size;
	while (BytesToSent > 0)
	{
		int32 BytesSend = 0;
		if (!mSocket->Send(Msg, BytesToSent, BytesSend))
		{
			UE_LOG(LogTemp, Log, TEXT("Failed to sent data"));
			return false;
		}
		BytesToSent -= BytesSend;
		Msg += BytesSend;
	}
	UE_LOG(LogTemp, Log, TEXT("Message sent"));

	return true;
}

bool FTCPClient::Receive()
{
	if (mSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket is not connected"));
		return false;
	}
	uint32 PendingData = -1;

	int32 ReadBytes = 0;

	if (mSocket->HasPendingData(PendingData))
	{
		if (mNumBytesToReceive <= 0)
		{
			if (PendingData >= sizeof(mNumBytesToReceive))
			{
				if (mSocket->Recv((uint8*)(&mNumBytesToReceive), sizeof(mNumBytesToReceive), ReadBytes))
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
			if (!mSocket->Recv(mCachedData.GetData() + mCachedData.Num() - mNumBytesToReceive, mNumBytesToReceive, ReadBytes))
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

bool FTCPClient::Disconnect()
{
	if (mSocket != nullptr)
	{
		if (mSocket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
		{
			if (!mSocket->Close())
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to close socket"));
				return false;
			}
			UE_LOG(LogTemp, Log, TEXT("Socket closed"));
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(mSocket);
			mSocket = nullptr;
		}
		return true;
	}
	return false;
}
