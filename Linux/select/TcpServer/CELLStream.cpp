#include "CELLStream.h"
#include <cstring>


int8_t CELLStream::readInt8(int8_t n )
{
	Read<int8_t>(n);
	return n;
}

int16_t CELLStream::readInt16(int16_t n)
{
	Read<int16_t>(n);
	return n;
}

int32_t CELLStream::readInt32(int32_t n)
{
	Read<int32_t>(n);
	return n;
}

float CELLStream::readFloat(float n)
{
	Read<float>(n);
	return n;
}

double CELLStream::readDouble(double n)
{
	Read<double>(n);
	return n;
}


bool CELLStream::writeInt8(int8_t n)
{
	return Write(n);
}

bool CELLStream::writeInt16(int16_t n)
{
	return Write(n);
}

bool CELLStream::writeInt32(int32_t n)
{
	return Write(n);
}

bool CELLStream::writeFloat(float n)
{
	return Write(n);
}

bool CELLStream::writeDouble(double n)
{
	return Write(n);
}
