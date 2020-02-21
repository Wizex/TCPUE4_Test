#pragma once

#include "Data.h"
#include "ArrayReader.h"

class UETCP_API FConverter
{
public:
	template<typename T>
	static FData Convert(T& Data, const uint32 Id)
	{
		FArrayWriter Writer;

		Writer << Data;

		FData Struct;

		Struct.Id = Id;
		Struct.SerializedData = Writer;
		
		return Struct;
	}

	static FData Convert(TArray<uint8> ByteArray)
	{
		FArrayReader Reader;

		Reader.Append(ByteArray);
		
		FData DataStruct;

		Reader << DataStruct;

		return DataStruct;
	}
};
