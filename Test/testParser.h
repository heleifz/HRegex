/************************************************************************/
/*  Test Regex Parser 
/************************************************************************/

#include "parser.h"
#include "nfa_walker.h"
#include "cute/cute.h"

void testConcatenate()
{
	NFA nfa;
	Parser p1("abc", nfa);
	NFAWalker walker1(nfa);
	ASSERT(walker1.match("abc", 3));
	ASSERT(!walker1.match("babc", 4));
	ASSERT(!walker1.match("cba", 3));
	ASSERT(!walker1.match("ab", 2));
	//ASSERT(walker.match("abc", 4));
}

void testKleen()
{
	NFA nfa;
	Parser p1("a*b", nfa);
	NFAWalker walker1(nfa);
	ASSERT(walker1.match("ab", 2));
	ASSERT(walker1.match("aaaab", 5));
	ASSERT(walker1.match("aaaaaaab", 8));
	ASSERT(walker1.match("b", 1));
	ASSERT(!walker1.match("bb", 2));
	ASSERT(!walker1.match("aaa", 3));
}

void testAlternate()
{
	NFA nfa;
	Parser p1("a|bbc", nfa);
	NFAWalker walker1(nfa);
	ASSERT(walker1.match("bbc", 3));
	ASSERT(walker1.match("a", 1));
	ASSERT(!walker1.match("bc", 2));
	ASSERT(!walker1.match("bb", 2));
}

void testRegexTogether()
{
	NFA nfa;
	Parser("(a|b)*abb", nfa);
	NFAWalker walker(nfa);
	ASSERT(walker.match("babb", 4));
	ASSERT(walker.match("baaaaaabb", 9));
	ASSERT(walker.match("aaabbbbababababb", 16));
	ASSERT(walker.match("abb", 3));
	ASSERT(walker.match("abbabb", 6));
	ASSERT(!walker.match("abbacbb", 7));
	ASSERT(!walker.match("babba", 5));
	ASSERT(!walker.match("abbbbb", 6));
	ASSERT(!walker.match("aaaaa", 5));
	ASSERT(!walker.match("", 0));
	ASSERT(!walker.match("b", 1));
	Parser("(1|2|3333)*a*cb*", nfa);
	ASSERT(walker.match("c", 1));
	ASSERT(walker.match("1212c", 5));
	ASSERT(walker.match("aaaaaacbbbbbbb", 14));
	ASSERT(walker.match("22aaaaaacbbbbbbb", 16));
	ASSERT(walker.match("3333aaaaaacbbbbbbb", 18));
	ASSERT(!walker.match("23aaaaaacbbbbbbb", 16));
	ASSERT(!walker.match("333aaaaaacbbbbbbb", 17));
}

void parserSuit()
{
	cute::suite s;
	s += CUTE(testConcatenate);
	s += CUTE(testKleen);
	s += CUTE(testAlternate);
	s += CUTE(testRegexTogether);
	cute::runner<cute::ostream_listener>()(s, "Regex Parser Test");
}
