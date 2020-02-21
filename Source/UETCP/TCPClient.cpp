#include "TCPClient.h"
#include "Networking/Public/Common/TcpSocketBuilder.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "BufferArchive.h"
#include <algorithm>
#include "MessageString.h"

FTCPClient::FTCPClient() : mNumBytesToReceive(0), mClientSocket(nullptr) {}

FTCPClient::FTCPClient(const FString& Ip, const uint32 Port) : FTCPClient()
{
	if (Setup(Ip, Port) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to setup TCP socket"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Success to setup TCP socket"));

		mClientSocket = FTcpSocketBuilder("Socket").AsReusable().AsNonBlocking();
		if (mClientSocket == nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("Socket is NULL"));
		}
	}
}

FTCPClient::FTCPClient(FSocket* Socket) : FTCPClient()
{
	if (Setup(Socket)== false)
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

	mInternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	mInternetAddr->SetIp(Ipv4.Value);
	mInternetAddr->SetPort(Port);

	return true;
}

bool FTCPClient::Setup(FSocket* Socket)
{
	mClientSocket = Socket;

	return true;
}

bool FTCPClient::Connect()
{
	if (mClientSocket == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Socket is NULL"));
		return false;
	}
	if (mClientSocket->Connect(*mInternetAddr) != true)
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
	if (mClientSocket == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Socket is NULL"));
		return false;
	}
	if (mClientSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket is not connected"));
		UE_LOG(LogTemp, Log, TEXT("Socket closed"));
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(mClientSocket);
		mClientSocket = nullptr;
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
		if (!mClientSocket->Send(Msg, BytesToSent, BytesSend))
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

void FTCPClient::ParseData()
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

bool FTCPClient::Disconnect()
{
	if (mClientSocket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
	{
		if (!mClientSocket->Close())
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

void FTCPClient::Destroy()
{
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(mClientSocket);
	mClientSocket = nullptr;
}

FTCPClient::~FTCPClient()
{
	if (mClientSocket != nullptr) 
	{
		Destroy();
	}
}



