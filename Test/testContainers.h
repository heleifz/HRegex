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

void testRemove()
{
	SortedVectorSet<int> s;
	s.insert(3);
	s.insert(1);
	s.insert(2);
	ASSERT(s.remove(3));
	ASSERT(!s.contains(3));
	ASSERT(!s.remove(3));
	ASSERT(s.contains(2));
	ASSERT(s.remove(2));
	ASSERT(!s.remove(2));
	ASSERT_EQUAL(1, s.size());
}

void testLastPopback()
{
	SortedVectorSet<int> s;
	s.insert(3);
	s.insert(1);
	s.insert(2);
	SortedVectorSet<int> s2 = s;
	ASSERT_EQUAL(3, s.last());
	s.popBack();
	ASSERT_EQUAL(2, s.last());
	s.popBack();
	ASSERT_EQUAL(1, s.last());
	s.popBack();
	ASSERT_EQUAL(0, s.size());
	ASSERT_EQUAL(3, s2.size());
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

void testIntersection()
{
	SortedVectorSet<int> s1;
	SortedVectorSet<int> s2;
	SortedVectorSet<int> s3;
	s1.insert(1);
	s1.insert(4);
	s1.insert(8);
	s2.insert(2);
	s2.insert(4);
	s2.insert(3);
	auto result = s1 && s2;
	ASSERT_EQUAL(1, result.size());
	ASSERT(result.contains(4));
	result = s1 && s3;
	ASSERT_EQUAL(0, result.size());
	s3.insert(8);
	s3.insert(1);
	result = s1 && s3;
	ASSERT_EQUAL(2, result.size());
}

void testUnion()
{
	SortedVectorSet<int> s1;
	SortedVectorSet<int> s2;
	SortedVectorSet<int> s3;
	ASSERT_EQUAL(0, (s1 || s2).size());
	s1.insert(1);
	s1.insert(4);
	s1.insert(8);
	s2.insert(2);
	s2.insert(4);
	s2.insert(7);
	auto result = s1 || s2;
	ASSERT_EQUAL(5, result.size());
	ASSERT(result.contains(1));
	ASSERT(result.contains(4));
	ASSERT(result.contains(8));
	ASSERT(result.contains(2));
	ASSERT(result.contains(7));
	result = s1 || s3;
	ASSERT_EQUAL(result, s1);
	s3.insert(9);
	s3.insert(15);
	result = s1 || s3;
	ASSERT_EQUAL(5, result.size());
	ASSERT(result.contains(1));
	ASSERT(result.contains(4));
	ASSERT(result.contains(8));
	ASSERT(result.contains(9));
	ASSERT(result.contains(15));
}

void testSubstraction()
{
	SortedVectorSet<int> s1;
	SortedVectorSet<int> s2;
	SortedVectorSet<int> s3;
	ASSERT_EQUAL(0, (s1 - s2).size());
	s1.insert(1);
	s1.insert(4);
	s1.insert(8);
	s2.insert(2);
	s2.insert(4);
	s2.insert(9);
	auto result = s1 - s2;
	ASSERT_EQUAL(2, result.size());
	ASSERT(result.contains(1));
	ASSERT(result.contains(8));
	result = s1 - s3;
	ASSERT_EQUAL(result, s1);
	s3.insert(1);
	s3.insert(8);
	result = s1 - s3;
	ASSERT_EQUAL(1, result.size());
	ASSERT(result.contains(4));
}

void testSetNested()
{
	SortedVectorSet<SortedVectorSet<int>> ss;
	SortedVectorSet<int> s;
	s.insert(3);
	ss.insert(s);
	s.insert(4);
	ss.insert(s);
	s.insert(5);
	ss.insert(s);
	ASSERT(ss.contains(s));
	ASSERT(s.remove(5));
	ASSERT(ss.contains(s));
	ASSERT(s.remove(4));
	ASSERT(ss.contains(s));
	ASSERT(s.remove(3));
	ASSERT(!ss.contains(s));
	s.insert(4);
	ASSERT(!ss.contains(s));
	s.insert(5);
	ASSERT(!ss.contains(s));
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
	s += CUTE(testLastPopback);
	s += CUTE(testRemove);
	s += CUTE(testSetEqual);
	s += CUTE(testSetLessThan);
	s += CUTE(testIntersection);
	s += CUTE(testUnion);
	s += CUTE(testSubstraction);
	s += CUTE(testSetNested);
	s += CUTE(testCoW);
	cute::runner<cute::ostream_listener>()(s, "Containers Test");
}