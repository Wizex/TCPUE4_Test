// Fill out your copyright notice in the Description page of Project Settings.

#include "MyUserWidget.h"
#include "MessageString.h"

void UMyUserWidget::NativeOnInitialized() 
{
	Super::NativeOnInitialized();

	StartClientButton->OnClicked.AddUniqueDynamic(this, &UMyUserWidget::StartClientEvent);
	CloseClientButton->OnClicked.AddUniqueDynamic(this, &UMyUserWidget::CloseClientEvent);
	SentButton->OnClicked.AddUniqueDynamic(this, &UMyUserWidget::SentClientEvent);
	
	StartServerButton->OnClicked.AddUniqueDynamic(this, &UMyUserWidget::StartServerEvent);
	CloseServerButton->OnClicked.AddUniqueDynamic(this, &UMyUserWidget::CloseServerEvent);
}

void UMyUserWidget::StartClientEvent()
{
	mClient = MakeUnique<FTCPClient>("127.0.0.1", 5555);
	mClient->Connect();
}

void UMyUserWidget::StartServerEvent()
{
	mServer = MakeUnique<FTCPServer>("127.0.0.1", 5555);
	mServer->Bind();
	mServer->Listen(1);
	mServer->OnTick(0);
}

void UMyUserWidget::SentClientEvent()
{
	if (mClient != nullptr) 
	{
		FMessageString Message;
		Message.Str = TextBox->GetText().ToString();
		FData Data = FConverter::Convert(Message, 5);

		mClient->SendMsg(Data);
	}
}

void UMyUserWidget::CloseServerEvent()
{
	if (mServer != nullptr)
	{
		mServer->CloseConnections();
	}
}

void UMyUserWidget::CloseClientEvent()
{
	if (mClient != nullptr) 
	{
		mClient->Disconnect();
	}
}
