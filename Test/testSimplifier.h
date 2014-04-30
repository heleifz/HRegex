/************************************************************************/
/*  Test Simplifier
/************************************************************************/

#include "parser.h"
#include "simplifier.h"
#include "cute/cute.h"

void testNFAToDFA()
{
	// Automata for (a|b)*abb
	Automata nfa;
	for (int i = 0; i != 11; ++i)
	{
		nfa.generateState();
	}
	nfa.addTransition(0, 1, Transition::EPSILON);
	nfa.addTransition(0, 7, Transition::EPSILON);
	nfa.addTransition(1, 2, Transition::EPSILON);
	nfa.addTransition(1, 4, Transition::EPSILON);
	nfa.addTransition(2, 3, static_cast<HRegexByte>('a'));
	nfa.addTransition(4, 5, static_cast<HRegexByte>('b'));
	nfa.addTransition(3, 6, Transition::EPSILON);
	nfa.addTransition(5, 6, Transition::EPSILON);
	nfa.addTransition(6, 1, Transition::EPSILON);
	nfa.addTransition(6, 7, Transition::EPSILON);
	nfa.addTransition(7, 8, static_cast<HRegexByte>(static_cast<HRegexByte>('a')));
	nfa.addTransition(8, 9, static_cast<HRegexByte>('b'));
	nfa.addTransition(6, 7, Transition::EPSILON);
	nfa.addTransition(9, 10, static_cast<HRegexByte>('b'));
	nfa.setStart(0);
	nfa.setTerminate(10);

	Automata dfa = Simplifier::NFAToDFA(nfa);

	ASSERT_EQUAL(5, dfa.size());
	ASSERT(dfa.simulate("babb", 4));
	ASSERT(dfa.simulate("baaaaaabb", 9));
	ASSERT(dfa.simulate("aaabbbbababababb", 16));
	ASSERT(dfa.simulate("abb", 3));
	ASSERT(dfa.simulate("abbabb", 6));
	ASSERT(!dfa.simulate("abbacbb", 7));
	ASSERT(!dfa.simulate("babba", 5));
	ASSERT(!dfa.simulate("abbbbb", 6));
	ASSERT(!dfa.simulate("aaaaa", 5));
	ASSERT(!dfa.simulate("", 0));
	ASSERT(!dfa.simulate("b", 1));

	// TODO : more test?	

}

void testMinimizeDFA()
{
	// empty DFA
	Automata emptyDFA;
	ASSERT(Simplifier::MinimizeDFA(emptyDFA).size() == 0);

	// DFA for a*
	Automata star;
	star.generateState();
	star.generateState();
	star.addTransition(0, 1, static_cast<HRegexByte>('a'));
	star.addTransition(1, 1, static_cast<HRegexByte>('a'));
	star.setStart(0);
	star.setTerminate(0);
	star.setTerminate(1);
	star = Simplifier::MinimizeDFA(star);
	ASSERT_EQUAL(1, star.size());
	ASSERT_EQUAL(1, star.getNeighbours(0).size());
	ASSERT_EQUAL(Transition(static_cast<HRegexByte>('a')), star.getNeighbours(0)[0].getTransition());
	ASSERT(star.isStart(0));
	ASSERT(star.isTerminate(0));

	// DFA for (a|b)*abb	
	Automata dfa;
	for (auto i = 0; i != 5; ++i)
	{
		dfa.generateState();
	}
	dfa.addTransition(0, 1, static_cast<HRegexByte>('a'));
	dfa.addTransition(0, 2, static_cast<HRegexByte>('b'));
	dfa.addTransition(1, 1, static_cast<HRegexByte>('a'));
	dfa.addTransition(1, 3, static_cast<HRegexByte>('b'));
	dfa.addTransition(2, 1, static_cast<HRegexByte>('a'));
	dfa.addTransition(2, 2, static_cast<HRegexByte>('b'));
	dfa.addTransition(3, 1, static_cast<HRegexByte>('a'));
	dfa.addTransition(3, 4, static_cast<HRegexByte>('b'));
	dfa.addTransition(4, 1, static_cast<HRegexByte>('a'));
	dfa.addTransition(4, 2, static_cast<HRegexByte>('b'));
	dfa.setStart(0);
	dfa.setTerminate(4);
	dfa = Simplifier::MinimizeDFA(dfa);
	ASSERT_EQUAL(4, dfa.size());
	ASSERT(dfa.simulate("babb", 4));
	ASSERT(dfa.simulate("baaaaaabb", 9));
	ASSERT(dfa.simulate("aaabbbbababababb", 16));
	ASSERT(dfa.simulate("abb", 3));
	ASSERT(dfa.simulate("abbabb", 6));
	ASSERT(!dfa.simulate("abbacbb", 7));
	ASSERT(!dfa.simulate("babba", 5));
	ASSERT(!dfa.simulate("abbbbb", 6));
	ASSERT(!dfa.simulate("aaaaa", 5));
	ASSERT(!dfa.simulate("", 0));
	ASSERT(!dfa.simulate("b", 1));
	
	// DFA for a(b|c)*
	Automata dfa2;
	for (auto i = 0; i != 4; ++i)
	{
		dfa2.generateState();
	}
	dfa2.addTransition(0, 1, static_cast<HRegexByte>('a'));
	dfa2.addTransition(1, 2, static_cast<HRegexByte>('b'));
	dfa2.addTransition(1, 3, static_cast<HRegexByte>('c'));
	dfa2.addTransition(2, 2, static_cast<HRegexByte>('b'));
	dfa2.addTransition(2, 3, static_cast<HRegexByte>('c'));
	dfa2.addTransition(3, 3, static_cast<HRegexByte>('c'));
	dfa2.addTransition(3, 2, static_cast<HRegexByte>('b'));
	dfa2.setStart(0);
	dfa2.setTerminate(1);
	dfa2.setTerminate(2);
	dfa2.setTerminate(3);
	dfa2 = Simplifier::MinimizeDFA(dfa2);
	ASSERT_EQUAL(2, dfa2.size());
	ASSERT(dfa2.simulate("abbcc", 5));
	ASSERT(dfa2.simulate("abc", 3));
	ASSERT(dfa2.simulate("a", 1));
	ASSERT(dfa2.simulate("abbbb", 5));
	ASSERT(dfa2.simulate("acccc", 5));
	ASSERT(dfa2.simulate("accbc", 5));
	ASSERT(!dfa2.simulate("", 0));
	ASSERT(!dfa2.simulate("bc", 2));
	ASSERT(!dfa2.simulate("bcbb", 4));
}

// Test suits

void simplifierSuit()
{
	cute::suite s;
	s += CUTE(testNFAToDFA);
	s += CUTE(testMinimizeDFA);
	cute::runner<cute::ostream_listener>()(s, "Simplifier Test");
}