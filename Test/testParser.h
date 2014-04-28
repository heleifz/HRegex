/************************************************************************/
/*  Test Regex Parser 
/************************************************************************/

#include "parser.h"
#include "cute/cute.h"

void testConcatenate()
{
	NFA nfa;
	Parser("abc", nfa);
	ASSERT(nfa.match("abc", 3));
	ASSERT(!nfa.match("babc", 4));
	ASSERT(!nfa.match("cba", 3));
	ASSERT(!nfa.match("ab", 2));
	//ASSERT(walker.match("abc", 4));
}

void testKleen()
{
	NFA nfa;
	Parser("a*b", nfa);
	ASSERT(nfa.match("ab", 2));
	ASSERT(nfa.match("aaaab", 5));
	ASSERT(nfa.match("aaaaaaab", 8));
	ASSERT(nfa.match("b", 1));
	ASSERT(!nfa.match("bb", 2));
	ASSERT(!nfa.match("aaa", 3));
}

void testAlternate()
{
	NFA nfa;
	Parser("a|bbc", nfa);
	ASSERT(nfa.match("bbc", 3));
	ASSERT(nfa.match("a", 1));
	ASSERT(!nfa.match("bc", 2));
	ASSERT(!nfa.match("bb", 2));
}

void testRegexTogether()
{
	NFA nfa;
	Parser("(a|b)*abb", nfa);
	ASSERT(nfa.match("babb", 4));
	ASSERT(nfa.match("baaaaaabb", 9));
	ASSERT(nfa.match("aaabbbbababababb", 16));
	ASSERT(nfa.match("abb", 3));
	ASSERT(nfa.match("abbabb", 6));
	ASSERT(!nfa.match("abbacbb", 7));
	ASSERT(!nfa.match("babba", 5));
	ASSERT(!nfa.match("abbbbb", 6));
	ASSERT(!nfa.match("aaaaa", 5));
	ASSERT(!nfa.match("", 0));
	ASSERT(!nfa.match("b", 1));
	Parser("(1|2|3333)*a*cb*", nfa);
	ASSERT(nfa.match("c", 1));
	ASSERT(nfa.match("1212c", 5));
	ASSERT(nfa.match("aaaaaacbbbbbbb", 14));
	ASSERT(nfa.match("22aaaaaacbbbbbbb", 16));
	ASSERT(nfa.match("3333aaaaaacbbbbbbb", 18));
	ASSERT(!nfa.match("23aaaaaacbbbbbbb", 16));
	ASSERT(!nfa.match("333aaaaaacbbbbbbb", 17));
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
