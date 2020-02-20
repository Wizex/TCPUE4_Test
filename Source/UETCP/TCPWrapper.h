#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "Core.h"
#include "Converter.h"

DECLARE_DELEGATE_OneParam(FOnClientDataReceived, const FData&);

class UETCP_API FTCPWrapper
{
private:
	bool Setup(const FString&, const uint32);
	void ParseData();
	
public:
	FTCPWrapper(const FString&, const uint32);
	virtual ~FTCPWrapper();

	bool Receive();
	virtual bool Disconnect() = 0;

	FOnClientDataReceived& OnClientDataReceived() { return mOnClientDataReceived; }

protected:
	void Destroy(FSocket*);
	FTCPWrapper();
	
	TArray<uint8> mCachedData;

	int mNumBytesToReceive;

	FSocket* mClientSocket;

	TSharedPtr<FInternetAddr> mInternetAddr;

	FOnClientDataReceived mOnClientDataReceived;
};