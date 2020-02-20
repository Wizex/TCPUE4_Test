#include "TCPClient.h"
#include "Networking/Public/Common/TcpSocketBuilder.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "BufferArchive.h"
#include <algorithm>
#include "MessageString.h"

FTCPClient::FTCPClient(const FString& Ip, const uint32 Port) : FTCPWrapper(Ip, Port)
{
	mClientSocket = FTcpSocketBuilder("Socket").AsReusable().AsNonBlocking();
	if (mClientSocket == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Socket is NULL"));
	}
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
		Destroy(mClientSocket);
		return true;
	}
	return false;
}


