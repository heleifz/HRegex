#ifndef _HREG_ENCODING_
#define _HREG_ENCODING_

#include <cstdint>
#include "globals.h"

/*
	
	Support multiple encodings reading / writing

	TODO:
	
	���ֱ��� = UTF8(ascii), UTF16
	
	������ʽ�����Ƿ�֧�ֶ��ֱ��룿
	ƥ���Ŀ���ַ���Ҫ֧�ֶ��ֱ���

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