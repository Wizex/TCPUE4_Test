// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetController.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.h"

void AWidgetController::BeginPlay()
{
	Super::BeginPlay();

	if (WidgetClass) {

		UsrWidget = CreateWidget<UMyUserWidget>(this, WidgetClass);

		if (UsrWidget)
		{
			UsrWidget->AddToViewport();
		}

		bShowMouseCursor = true;
	}
}






