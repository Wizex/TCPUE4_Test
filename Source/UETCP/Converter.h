#pragma once

#include "Data.h"

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
};