/************************************************************************/
/*  Test Encoding
/************************************************************************/

#include "cute/cute.h"
#include "encoding.h"

void testASCII()
{
	StreamReader<ASCII> empty("");
	ASSERT_EQUAL('\0', empty.next());

	StreamReader<ASCII> reader("hello");
	ASSERT_EQUAL('h', reader.peek());
	ASSERT_EQUAL('h', reader.next());
	ASSERT_EQUAL('e', reader.peek());
	ASSERT_EQUAL('e', reader.next());
	ASSERT_EQUAL('l', reader.peek());
	ASSERT_EQUAL('l', reader.next());
	ASSERT_EQUAL('l', reader.peek());
	ASSERT_EQUAL('l', reader.next());
	ASSERT_EQUAL('o', reader.peek());
	ASSERT_EQUAL('o', reader.next());
	ASSERT_EQUAL('\0', reader.peek());
	ASSERT_EQUAL('\0', reader.next());
}

void testUTF8()
{
	// utf8 for "八*百ab万"
	const char d1[] = {
		0xe5u, 0x85u, 0xabu, '*', 0xe7u, 0x99u, 0xbeu,
		'a', 'b', 0xe4u, 0xb8u, 0x87u, 0x00u
	};
	StreamReader<UTF8> reader(d1);
	ASSERT_EQUAL(0x516bu, reader.peek());
	ASSERT_EQUAL(0x516bu, reader.next());
	ASSERT_EQUAL('*', reader.peek());
	ASSERT_EQUAL('*', reader.next());
	ASSERT_EQUAL(0x767eu, reader.peek());
	ASSERT_EQUAL(0x767eu, reader.next());
	ASSERT_EQUAL('a', reader.peek());
	ASSERT_EQUAL('a', reader.next());
	ASSERT_EQUAL('b', reader.peek());
	ASSERT_EQUAL('b', reader.next());
	ASSERT_EQUAL(0x4e07u, reader.peek());
	ASSERT_EQUAL(0x4e07u, reader.next());
	ASSERT_EQUAL(0, reader.peek());
	ASSERT_EQUAL(0, reader.next());
}

void testUTF16()
{
#ifdef _MSC_VER
	StreamReader<UTF16> empty(L"");
	ASSERT_EQUAL(0x0000u, empty.next());

	const wchar_t *data = L"八百万";
	StreamReader<UTF16> reader(data);
	ASSERT_EQUAL(0x516bu, reader.peek());
	ASSERT_EQUAL(0x516bu, reader.next());
	ASSERT_EQUAL(0x767eu, reader.peek());
	ASSERT_EQUAL(0x767eu, reader.next());
	ASSERT_EQUAL(0x4e07u, reader.peek());
	ASSERT_EQUAL(0x4e07u, reader.next());
	ASSERT_EQUAL(0x0000u, reader.peek());
	ASSERT_EQUAL(0x0000u, reader.next());

	const wchar_t *data1 = L"八abc百*d万";
	StreamReader<UTF16> reader1(data1);
	ASSERT_EQUAL(0x516bu, reader1.peek());
	ASSERT_EQUAL(0x516bu, reader1.next());
	ASSERT_EQUAL('a', reader1.peek());
	ASSERT_EQUAL('a', reader1.next());
	ASSERT_EQUAL('b', reader1.peek());
	ASSERT_EQUAL('b', reader1.next());
	ASSERT_EQUAL('c', reader1.peek());
	ASSERT_EQUAL('c', reader1.next());
	ASSERT_EQUAL(0x767eu, reader1.peek());
	ASSERT_EQUAL(0x767eu, reader1.next());
	ASSERT_EQUAL('*', reader1.peek());
	ASSERT_EQUAL('*', reader1.next());
	ASSERT_EQUAL('d', reader1.peek());
	ASSERT_EQUAL('d', reader1.next());
	ASSERT_EQUAL(0x4e07u, reader1.peek());
	ASSERT_EQUAL(0x4e07u, reader1.next());
	ASSERT_EQUAL(0x0000u, reader1.peek());
	ASSERT_EQUAL(0x0000u, reader1.next());
#else
	std::cout << " === SKIP THIS TEST === "
#endif
}

void testIllegalOperation()
{
	ASSERT_THROWS(StreamReader<ASCII>(0), NullPointerError);
	ASSERT_THROWS(StreamReader<UTF16>(0), NullPointerError);
	ASSERT_THROWS(StreamReader<UTF8>(0), NullPointerError);

	StreamReader<ASCII> reader("hello");
	StreamReader<UTF8> reader2("world");
	StreamReader<UTF16> reader3(L"man");
	for (int i = 0; i != 10; ++i)
	{
		reader.next();
		reader2.next();
		reader3.next();
	}
	ASSERT_EQUAL(0, reader.next());
	ASSERT_EQUAL(0, reader2.next());
	ASSERT_EQUAL(0, reader3.next());
}

// Test suits

void encodingSuit()
{
	cute::suite s;
	s += CUTE(testASCII);
	s += CUTE(testUTF8);
	s += CUTE(testUTF16);
	s += CUTE(testIllegalOperation);
	cute::runner<cute::ostream_listener>()(s, "Encoding Test");
}