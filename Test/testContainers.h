/************************************************************************/
/*  Test Containers                                                          */
/************************************************************************/

#include "containers.h"
#include "cute/cute.h"

void testSetInsertContains()
{
	SortedVectorSet<int> s;
	s.insert(3);
	ASSERT(s.contains(3));
	ASSERT(!s.contains(2));
	ASSERT(!s.contains(1));
	s.insert(1);
	s.insert(2);
	ASSERT(s.contains(3));
	ASSERT(s.contains(2));
	ASSERT(s.contains(1));
}

void testSetSize()
{
	SortedVectorSet<int> s;
	ASSERT_EQUAL(0, s.size());
	s.insert(3);
	s.insert(2);
	s.insert(5);
	ASSERT_EQUAL(3, s.size());
	s.insert(5);
	s.insert(2);
	ASSERT_EQUAL(3, s.size());
	s.clear();
	ASSERT_EQUAL(0, s.size());
}

void testSetEqual()
{
	SortedVectorSet<int> s1;
	SortedVectorSet<int> s2;
	s1.insert(3);
	s1.insert(2);
	s1.insert(5);
	s2.insert(3);
	s2.insert(2);
	ASSERT(s1 != s2);
	s2.insert(5);
	ASSERT(s1 == s2);
	s2.insert(3);
	ASSERT(s1 == s2);
	s1.clear();
	ASSERT(s1 != s2);
}

void testSetLessThan()
{
	SortedVectorSet<int> s1;
	SortedVectorSet<int> s2;
	ASSERT(!(s2 < s1));
	ASSERT(!(s1 < s2));
	s1.insert(3);
	s1.insert(2);
	s1.insert(5);
	s2.insert(3);
	s2.insert(2);
	ASSERT(s2 < s1);
	s2.insert(5);
	ASSERT(!(s2 < s1));
	ASSERT(!(s1 < s2));
	s2.insert(6);
	ASSERT(s1 < s2);
}

void testCoW()
{
	SortedVectorSet<int> s1;
	s1.insert(3);
	SortedVectorSet<int> s2 = s1;
	ASSERT(s1 == s2);
	s2.insert(4);
	ASSERT(s1 != s2);
	ASSERT_EQUAL(1, s1.size());
	ASSERT_EQUAL(2, s2.size());
}

// Test suits

void containersSuit()
{
	cute::suite s;
	s += CUTE(testSetSize);
	s += CUTE(testSetInsertContains);
	s += CUTE(testSetEqual);
	s += CUTE(testSetLessThan);
	s += CUTE(testCoW);
	cute::runner<cute::ostream_listener>()(s, "Containers Test");
}