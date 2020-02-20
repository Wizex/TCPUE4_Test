// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Button.h"
#include "WidgetController.generated.h"

UCLASS()
class UETCP_API AWidgetController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UMyUserWidget> WidgetClass;

	UMyUserWidget* UsrWidget;

	virtual void BeginPlay() override;
};
