#pragma once

#include "CoreMinimal.h"
#include "Data.generated.h"

USTRUCT()
struct UETCP_API FData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY() uint32 Id;
	UPROPERTY() TArray<uint8> SerializedData;

	friend FORCEINLINE FArchive& operator <<(FArchive& Ar, FData& TheStruct)
	{
		Ar << TheStruct.Id;
		Ar << TheStruct.SerializedData;
		return Ar;
	}
};




