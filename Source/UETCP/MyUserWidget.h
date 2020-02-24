// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Button.h"
#include "EditableTextBox.h"
#include "TCPServer.h"
#include "TCPClientWrapper.h"
#include "MyUserWidget.generated.h"

UCLASS()
class UETCP_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	UFUNCTION()
	void StartClientEvent();
	UFUNCTION()
	void CloseClientEvent();
	UFUNCTION()
	void SendClientEvent();
	UFUNCTION()
	void StartServerEvent();
	UFUNCTION()
	void CloseServerEvent();
	UFUNCTION()
	void SendServerEvent();

public:
	void NativeOnInitialized() final;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) UButton* StartClientButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) UButton* CloseClientButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) UButton* SendButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) UButton* StartServerButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) UButton* CloseServerButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) UButton* SendServerButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) UEditableTextBox* TextBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) UEditableTextBox* TextBoxServer;

private:
	TUniquePtr<FTCPClientWrapper> mClient;
	TUniquePtr<FTCPServer> mServer;
};
