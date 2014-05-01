#ifndef _HREG_ENCODING_
#define _HREG_ENCODING_

#include <cstdint>
#include "globals.h"

/*
	
	Convert encoded byte stream to unicode codepoint

	!NOTE
	仅仅作最基本的输入检查，包括空指针，以及过度的读取（读完字符后，总是返回0）
	但是，不对UTF8和UTF16的格式做任何检查，也就是说，对于错误的编码字符串
	仍然可能越界

*/

typedef uint32_t UnicodeChar;

enum EncodeType
{
	ASCII,
	UTF8,
	UTF16
};

template <EncodeType E>
class Encode;

template <EncodeType E>
class StreamReader;

template <>
struct Encode<ASCII>
{
	typedef const char *PointerType;
};

template <>
struct Encode<UTF8>
{
	typedef const char *PointerType;
};

template <>
struct Encode<UTF16>
{
#ifdef _MSC_VER
	typedef const wchar_t *PointerType;
#else
	typedef const char16_t *PointerType;
#endif
};

template <>
class StreamReader<ASCII>
{
	typedef Encode<ASCII>::PointerType PointerType;
public:
	StreamReader(PointerType input) : data(input)
	{
		if (data == nullptr)
		{
			throw NullPointerError();
		}
		next();
	}
	UnicodeChar next()
	{
		UnicodeChar old = nextChar;
		nextChar = *data;
		if (nextChar != 0)
		{
			data++;
		}
		return old;
	}
	UnicodeChar peek() const
	{
		return nextChar;
	}
private:
	PointerType data;
	UnicodeChar nextChar;
};

template <>
class StreamReader<UTF8>
{
	typedef Encode<UTF8>::PointerType PointerType;
public:
	StreamReader(PointerType input) : data(input)
	{
		if (data == nullptr)
		{
			throw NullPointerError();
		}
		next();
	}
	UnicodeChar next()
	{
		UnicodeChar old = nextChar;
		nextChar = static_cast<uint32_t>(*data);
		size_t len = byteLength();
		switch (len) {
		case 1:
			break;
		case 2:
			data++;
			nextChar = ((nextChar << 6) & 0x7ff) +
				(static_cast<unsigned char>(*data) & 0x3f);
			break;
		case 3:
			++data;
			nextChar = ((nextChar << 12) & 0xffff) +
				((static_cast<unsigned char>(*data) << 6) & 0xfff);
			++data;
			nextChar += static_cast<unsigned char>(*data) & 0x3f;
			break;
		case 4:
			++data;
			nextChar = ((nextChar << 18) & 0x1fffff) +
				((static_cast<unsigned char>(*data) << 12) & 0x3ffff);
			++data;
			nextChar += (static_cast<unsigned char>(*data) << 6) & 0xfff;
			++data;
			nextChar += static_cast<unsigned char>(*data) & 0x3f;
			break;
		}
		if (nextChar != 0)
		{
			data++;
		}
		return old;
	}
	UnicodeChar peek() const
	{
		return nextChar;
	}
private:
	size_t byteLength() const
	{
		if (static_cast<unsigned char>(*data) < 0x80)
		{
			return 1;
		}
		else if ((static_cast<unsigned char>(*data) >> 5) == 0x6)
		{
			return 2;
		}
		else if ((static_cast<unsigned char>(*data) >> 4) == 0xe)
		{
			return 3;
		}
		else if ((static_cast<unsigned char>(*data) >> 3) == 0x1e)
		{
			return 4;
		}
		else
		{
			return 0;
		}
	}
	UnicodeChar nextChar;
	PointerType data;
};

template <>
class StreamReader<UTF16>
{
	typedef Encode<UTF16>::PointerType PointerType;
public:
	StreamReader(PointerType input) : data(input)
	{
		if (data == nullptr)
		{
			throw NullPointerError();
		}
		next();
	}
	UnicodeChar next()
	{
		UnicodeChar old = nextChar;
		nextChar = *data++;
		if (nextChar >= 0xd800u && nextChar <= 0xdbffu)
		{
			uint32_t trail = *data++;
			nextChar = (nextChar << 10) + trail + (0xd800u - (0x10000 >> 10));
		}
		if (nextChar == 0)
		{
			data--;
		}
		return old;
	}
	UnicodeChar peek() const
	{
		return nextChar;
	}
private:
	PointerType data;
	UnicodeChar nextChar;
};

#endif