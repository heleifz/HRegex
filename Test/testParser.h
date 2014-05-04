/************************************************************************/
/*  Test Regex Parser
/************************************************************************/

#include "parser.h"
#include "cute/cute.h"

void testEmptyInput()
{
	Automata nfa;
	Parser<ASCII>("", nfa);
	ASSERT_EQUAL(0, nfa.size());
}

void testIllegalInput()
{
	Automata nfa;
	ASSERT_THROWS(Parser<ASCII>("(aa", nfa), ParseError);
	ASSERT_THROWS(Parser<ASCII>("aa)", nfa), ParseError);
	ASSERT_THROWS(Parser<ASCII>("(aa))", nfa), ParseError);
	ASSERT_THROWS(Parser<ASCII>("(((aa))", nfa), ParseError);
	ASSERT_THROWS(Parser<ASCII>("ab|", nfa), ParseError);
	ASSERT_THROWS(Parser<ASCII>("ab||", nfa), ParseError);
	ASSERT_THROWS(Parser<ASCII>("|ab", nfa), ParseError);
	ASSERT_THROWS(Parser<ASCII>("||ab", nfa), ParseError);
	ASSERT_THROWS(Parser<ASCII>("*ab", nfa), ParseError);
}

void testConcatenate()
{
	Automata nfa;
	Parser<ASCII>("abc", nfa);
	ASSERT(nfa.simulate<ASCII>("abc", 3));
	ASSERT(!nfa.simulate<ASCII>("babc", 4));
	ASSERT(!nfa.simulate<ASCII>("cba", 3));
	ASSERT(!nfa.simulate<ASCII>("ab", 2));
	//ASSERT(walker.match("abc", 4));
}

void testKleen()
{
	Automata nfa;
	Parser<ASCII>("a*b", nfa);
	ASSERT(nfa.simulate<ASCII>("ab", 2));
	ASSERT(nfa.simulate<ASCII>("aaaab", 5));
	ASSERT(nfa.simulate<ASCII>("aaaaaaab", 8));
	ASSERT(nfa.simulate<ASCII>("b", 1));
	ASSERT(!nfa.simulate<ASCII>("bb", 2));
	ASSERT(!nfa.simulate<ASCII>("aaa", 3));
	Parser<ASCII>("a****", nfa);
	ASSERT(nfa.simulate<ASCII>("", 0));
	ASSERT(nfa.simulate<ASCII>("a", 1));
	ASSERT(nfa.simulate<ASCII>("aa", 2));
	ASSERT(nfa.simulate<ASCII>("aaa", 3));
	ASSERT(!nfa.simulate<ASCII>("aaab", 4));
}

void testAlternate()
{
	Automata nfa;
	Parser<ASCII>("a|bbc", nfa);
	ASSERT(nfa.simulate<ASCII>("bbc", 3));
	ASSERT(nfa.simulate<ASCII>("a", 1));
	ASSERT(!nfa.simulate<ASCII>("bc", 2));
	ASSERT(!nfa.simulate<ASCII>("bb", 2));
}

void testOptional()
{
	Automata nfa;
	Parser<ASCII>("a?bb?c", nfa);
	ASSERT(nfa.simulate<ASCII>("bc", 2));
	ASSERT(nfa.simulate<ASCII>("abbc", 4));
	ASSERT(nfa.simulate<ASCII>("abc", 3));
	ASSERT(nfa.simulate<ASCII>("bbc", 3));
	ASSERT(!nfa.simulate<ASCII>("aabbb", 5));
	ASSERT(!nfa.simulate<ASCII>("abcc", 4));
	ASSERT(!nfa.simulate<ASCII>("ac", 2));
	ASSERT(!nfa.simulate<ASCII>("a", 1));
	ASSERT(!nfa.simulate<ASCII>("c", 1));
	ASSERT(!nfa.simulate<ASCII>("b", 1));
	Parser<ASCII>("a?b?", nfa);
	ASSERT(nfa.simulate<ASCII>("", 0));
	ASSERT(nfa.simulate<ASCII>("a", 1));
	ASSERT(nfa.simulate<ASCII>("b", 1));
	ASSERT(nfa.simulate<ASCII>("ab", 2));
	Parser<ASCII>("ab????", nfa);
	ASSERT(nfa.simulate<ASCII>("ab", 2));
	ASSERT(nfa.simulate<ASCII>("a", 1));
	ASSERT(!nfa.simulate<ASCII>("b", 1));
	ASSERT(!nfa.simulate<ASCII>("abb", 3));
}

void testOneOrMore()
{
	Automata nfa;
	Parser<ASCII>("a+b+c+", nfa);
	ASSERT(nfa.simulate<ASCII>("abc", 3));
	ASSERT(nfa.simulate<ASCII>("abbc", 4));
	ASSERT(nfa.simulate<ASCII>("abccc", 5));
	ASSERT(nfa.simulate<ASCII>("aabbcc", 6));
	ASSERT(nfa.simulate<ASCII>("aaabccc", 7));
	ASSERT(nfa.simulate<ASCII>("abbbbbbbccc", 11));
	ASSERT(!nfa.simulate<ASCII>("aabbb", 5));
	ASSERT(!nfa.simulate<ASCII>("bbcc", 4));
	ASSERT(!nfa.simulate<ASCII>("acb", 3));
	ASSERT(!nfa.simulate<ASCII>("ac", 2));
	ASSERT(!nfa.simulate<ASCII>("ab", 2));
	ASSERT(!nfa.simulate<ASCII>("bc", 2));
	ASSERT(!nfa.simulate<ASCII>("a", 1));
	ASSERT(!nfa.simulate<ASCII>("c", 1));
	ASSERT(!nfa.simulate<ASCII>("b", 1));
	Parser<ASCII>("ab+++", nfa);
	ASSERT(nfa.simulate<ASCII>("ab", 2));
	ASSERT(nfa.simulate<ASCII>("abb", 3));
	ASSERT(nfa.simulate<ASCII>("abbb", 4));
	ASSERT(!nfa.simulate<ASCII>("a", 1));
	ASSERT(!nfa.simulate<ASCII>("b", 1));
}

#include "simplifier.h"
void testWildcard()
{
	Automata nfa;
	Parser<ASCII>(".*abc.*", nfa);
	ASSERT(nfa.simulate<ASCII>("abc", 3));
	ASSERT(nfa.simulate<ASCII>("aaasdfabcadsfasdf", 17));
	ASSERT(nfa.simulate<ASCII>("abcaa", 5));
	ASSERT(nfa.simulate<ASCII>("bbabc", 5));
	ASSERT(!nfa.simulate<ASCII>("bbc", 3));
	ASSERT(!nfa.simulate<ASCII>("a", 1));
	ASSERT(!nfa.simulate<ASCII>("abaac", 5));
	ASSERT(!nfa.simulate<ASCII>("cba", 3));

}

void testDigit()
{
	Automata nfa;
	Parser<ASCII>("\\d\\d\\d", nfa);
	ASSERT(nfa.simulate<ASCII>("123", 3));
	ASSERT(nfa.simulate<ASCII>("456", 3));
	ASSERT(nfa.simulate<ASCII>("999", 3));
	ASSERT(!nfa.simulate<ASCII>("9999", 4));
	ASSERT(!nfa.simulate<ASCII>("12", 2));
	ASSERT(!nfa.simulate<ASCII>("1", 1));
	ASSERT(!nfa.simulate<ASCII>("asd", 3));
	ASSERT(!nfa.simulate<ASCII>("a12", 3));
}

void testEscape()
{
	Automata nfa;
	Parser<ASCII>("\\+\\.\\)", nfa);
	ASSERT(nfa.simulate<ASCII>("+.)", 3));
	Parser<ASCII>("\\*\\(\\?", nfa);
	ASSERT(nfa.simulate<ASCII>("*(?", 3));
	Parser<ASCII>("\\\\", nfa);
	ASSERT(nfa.simulate<ASCII>("\\", 1));
}

void testRegexTogether()
{
	Automata nfa;
	Parser<ASCII>("(a|b)*abb", nfa);
	ASSERT(nfa.simulate<ASCII>("babb", 4));
	ASSERT(nfa.simulate<ASCII>("baaaaaabb", 9));
	ASSERT(nfa.simulate<ASCII>("aaabbbbababababb", 16));
	ASSERT(nfa.simulate<ASCII>("abb", 3));
	ASSERT(nfa.simulate<ASCII>("abbabb", 6));
	ASSERT(!nfa.simulate<ASCII>("abbacbb", 7));
	ASSERT(!nfa.simulate<ASCII>("babba", 5));
	ASSERT(!nfa.simulate<ASCII>("abbbbb", 6));
	ASSERT(!nfa.simulate<ASCII>("aaaaa", 5));
	ASSERT(!nfa.simulate<ASCII>("", 0));
	ASSERT(!nfa.simulate<ASCII>("b", 1));
	Parser<ASCII>("(1|2|3333)*a*cb*", nfa);
	ASSERT(nfa.simulate<ASCII>("c", 1));
	ASSERT(nfa.simulate<ASCII>("1212c", 5));
	ASSERT(nfa.simulate<ASCII>("aaaaaacbbbbbbb", 14));
	ASSERT(nfa.simulate<ASCII>("22aaaaaacbbbbbbb", 16));
	ASSERT(nfa.simulate<ASCII>("3333aaaaaacbbbbbbb", 18));
	ASSERT(!nfa.simulate<ASCII>("23aaaaaacbbbbbbb", 16));
	ASSERT(!nfa.simulate<ASCII>("333aaaaaacbbbbbbb", 17));
	Parser<ASCII>("\\d\\d\\d\\.\\d\\d\\d\\.\\d\\d\\d", nfa);
	ASSERT(nfa.simulate<ASCII>("111.111.111", 11));
	ASSERT(!nfa.simulate<ASCII>("1a1.11a.111", 11));
	Parser<ASCII>("\\d+", nfa);
	ASSERT(nfa.simulate<ASCII>("123", 3));
	ASSERT(nfa.simulate<ASCII>("122293", 6));
	ASSERT(!nfa.simulate<ASCII>("1222a93", 7));
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
