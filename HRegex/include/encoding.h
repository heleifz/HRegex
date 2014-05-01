#ifndef _HREG_ENCODING_
#define _HREG_ENCODING_

#include <cstdint>
#include "globals.h"

/*
	
	Support multiple encodings reading / writing

	TODO:
	
	多种编码 = UTF8(ascii), UTF16
	
	正则表达式本身是否支持多种编码？
	匹配的目标字符串要支持多种编码

*/

typedef uint8_t LengthType;

enum EncodingType
{
	ASCII,
	UTF8,
	UTF16
};

template <EncodingType T>
struct Character
{
};

template <>
struct Character<UTF8>
{

};

template <EncodingType T> 
class CharacterStream
{
};

template <ASCII>
class CharacterStream
{
	typedef char CharType;
	CharacterStream(const char *str)
		: data(str)
	{ 
	}
	CharType next()
	{
		return *data++;
	}
	CharType lookahead()
	{
		return *data;
	}
private:
	const char *data;
};


#endif