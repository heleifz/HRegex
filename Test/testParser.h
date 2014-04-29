/************************************************************************/
/*  Test Regex Parser 
/************************************************************************/

#include "parser.h"
#include "cute/cute.h"

void testEmptyInput()
{
	Automata nfa;
	Parser("", nfa);
	ASSERT_EQUAL(0, nfa.size());
}

void testIllegalInput()
{
	Automata nfa;
	ASSERT_THROWS(Parser("(aa", nfa), ParseError);
	ASSERT_THROWS(Parser("aa)", nfa), ParseError);
	ASSERT_THROWS(Parser("(aa))", nfa), ParseError);
	ASSERT_THROWS(Parser("(((aa))", nfa), ParseError);
	ASSERT_THROWS(Parser("ab|", nfa), ParseError);
	ASSERT_THROWS(Parser("ab||", nfa), ParseError);
	ASSERT_THROWS(Parser("|ab", nfa), ParseError);
	ASSERT_THROWS(Parser("||ab", nfa), ParseError);
	ASSERT_THROWS(Parser("*ab", nfa), ParseError);
}

void testConcatenate()
{
	Automata nfa;
	Parser("abc", nfa);
	ASSERT(nfa.simulate("abc", 3));
	ASSERT(!nfa.simulate("babc", 4));
	ASSERT(!nfa.simulate("cba", 3));
	ASSERT(!nfa.simulate("ab", 2));
	//ASSERT(walker.match("abc", 4));
}

void testKleen()
{
	Automata nfa;
	Parser("a*b", nfa);
	ASSERT(nfa.simulate("ab", 2));
	ASSERT(nfa.simulate("aaaab", 5));
	ASSERT(nfa.simulate("aaaaaaab", 8));
	ASSERT(nfa.simulate("b", 1));
	ASSERT(!nfa.simulate("bb", 2));
	ASSERT(!nfa.simulate("aaa", 3));
}

void testAlternate()
{
	Automata nfa;
	Parser("a|bbc", nfa);
	ASSERT(nfa.simulate("bbc", 3));
	ASSERT(nfa.simulate("a", 1));
	ASSERT(!nfa.simulate("bc", 2));
	ASSERT(!nfa.simulate("bb", 2));
}

void testOptional()
{
	Automata nfa;
	Parser("a?bb?c", nfa);
	ASSERT(nfa.simulate("bc", 2));
	ASSERT(nfa.simulate("abbc", 4));
	ASSERT(nfa.simulate("abc", 3));
	ASSERT(nfa.simulate("bbc", 3));
	ASSERT(!nfa.simulate("aabbb", 5));
	ASSERT(!nfa.simulate("abcc", 4));
	ASSERT(!nfa.simulate("ac", 2));
	ASSERT(!nfa.simulate("a", 1));
	ASSERT(!nfa.simulate("c", 1));
	ASSERT(!nfa.simulate("b", 1));
}

void testOneOrMore()
{
	Automata nfa;
	Parser("a+b+c+", nfa);
	ASSERT(nfa.simulate("abc", 3));
	ASSERT(nfa.simulate("abbc", 4));
	ASSERT(nfa.simulate("abccc", 5));
	ASSERT(nfa.simulate("aabbcc", 6));
	ASSERT(nfa.simulate("aaabccc", 7));
	ASSERT(nfa.simulate("abbbbbbbccc", 11));
	ASSERT(!nfa.simulate("aabbb", 5));
	ASSERT(!nfa.simulate("bbcc", 4));
	ASSERT(!nfa.simulate("acb", 3));
	ASSERT(!nfa.simulate("ac", 2));
	ASSERT(!nfa.simulate("ab", 2));
	ASSERT(!nfa.simulate("bc", 2));
	ASSERT(!nfa.simulate("a", 1));
	ASSERT(!nfa.simulate("c", 1));
	ASSERT(!nfa.simulate("b", 1));
}

void testWildcard()
{
	Automata nfa;
	Parser(".*abc.*", nfa);
	ASSERT(nfa.simulate("abc", 3));
	ASSERT(nfa.simulate("aaasdfabcadsfasdf", 17));
	ASSERT(nfa.simulate("abcaa", 5));
	ASSERT(nfa.simulate("bbabc", 5));
	ASSERT(!nfa.simulate("bbc", 3));
	ASSERT(!nfa.simulate("a", 1));
	ASSERT(!nfa.simulate("abaac", 5));
	ASSERT(!nfa.simulate("cba", 3));
}

void testDigit()
{
	Automata nfa;
	Parser("\\d\\d\\d", nfa);
	ASSERT(nfa.simulate("123", 3));
	ASSERT(nfa.simulate("456", 3));
	ASSERT(nfa.simulate("999", 3));
	ASSERT(!nfa.simulate("9999", 4));
	ASSERT(!nfa.simulate("12", 2));
	ASSERT(!nfa.simulate("1", 1));
	ASSERT(!nfa.simulate("asd", 3));
	ASSERT(!nfa.simulate("a12", 3));
}

void testEscape()
{
	Automata nfa;
	Parser("\\+\\.\\)", nfa);
	ASSERT(nfa.simulate("+.)", 3));
	Parser("\\*\\(\\?", nfa);
	ASSERT(nfa.simulate("*(?", 3));
	Parser("\\\\", nfa);
	ASSERT(nfa.simulate("\\", 1));
}

void testRegexTogether()
{
	Automata nfa;
	Parser("(a|b)*abb", nfa);
	ASSERT(nfa.simulate("babb", 4));
	ASSERT(nfa.simulate("baaaaaabb", 9));
	ASSERT(nfa.simulate("aaabbbbababababb", 16));
	ASSERT(nfa.simulate("abb", 3));
	ASSERT(nfa.simulate("abbabb", 6));
	ASSERT(!nfa.simulate("abbacbb", 7));
	ASSERT(!nfa.simulate("babba", 5));
	ASSERT(!nfa.simulate("abbbbb", 6));
	ASSERT(!nfa.simulate("aaaaa", 5));
	ASSERT(!nfa.simulate("", 0));
	ASSERT(!nfa.simulate("b", 1));
	Parser("(1|2|3333)*a*cb*", nfa);
	ASSERT(nfa.simulate("c", 1));
	ASSERT(nfa.simulate("1212c", 5));
	ASSERT(nfa.simulate("aaaaaacbbbbbbb", 14));
	ASSERT(nfa.simulate("22aaaaaacbbbbbbb", 16));
	ASSERT(nfa.simulate("3333aaaaaacbbbbbbb", 18));
	ASSERT(!nfa.simulate("23aaaaaacbbbbbbb", 16));
	ASSERT(!nfa.simulate("333aaaaaacbbbbbbb", 17));
	Parser("\\d\\d\\d\\.\\d\\d\\d\\.\\d\\d\\d", nfa);
	ASSERT(nfa.simulate("111.111.111", 11));
	ASSERT(!nfa.simulate("1a1.11a.111", 11));
	Parser("\\d+", nfa);
	ASSERT(nfa.simulate("123", 3));
	ASSERT(nfa.simulate("122293", 6));
	ASSERT(!nfa.simulate("1222a93", 7));
}

void parserSuit()
{
	cute::suite s;
	s += CUTE(testEmptyInput);
	s += CUTE(testIllegalInput);
	s += CUTE(testConcatenate);
	s += CUTE(testKleen);
	s += CUTE(testAlternate);
	s += CUTE(testOptional);
	s += CUTE(testOneOrMore);
	s += CUTE(testWildcard);
	s += CUTE(testDigit);
	s += CUTE(testEscape);
	s += CUTE(testRegexTogether);
	cute::runner<cute::ostream_listener>()(s, "Regex Parser Test");
}
