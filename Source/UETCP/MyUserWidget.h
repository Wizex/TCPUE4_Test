// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Button.h"
#include "EditableTextBox.h"
#include "TCPServer.h"
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
	void SentClientEvent();
	UFUNCTION()
	void StartServerEvent();
	UFUNCTION()
	void CloseServerEvent();

public:
	void NativeOnInitialized() final;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) UButton* StartClientButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) UButton* CloseClientButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) UButton* SentButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) UButton* StartServerButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) UButton* CloseServerButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) UEditableTextBox* TextBox;

private:
	TUniquePtr<FTCPClient> mClient;
	TUniquePtr<FTCPServer> mServer;
	
};
