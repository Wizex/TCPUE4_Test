// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MemoryArchive.h"
#include "ArrayReader.h"
#include "MessageString.generated.h"

USTRUCT()
struct UETCP_API FMessageString
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY() FString Str;

	friend FORCEINLINE FArchive& operator <<(FArchive& Ar, FMessageString& TheStruct)
	{
		Ar << TheStruct.Str;
		return Ar;
	}
};

