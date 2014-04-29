/************************************************************************/
/*  Test Simplifier
/************************************************************************/

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
	nfa.addTransition(0, 1, Transition());
	nfa.addTransition(0, 7, Transition());
	nfa.addTransition(1, 2, Transition());
	nfa.addTransition(1, 4, Transition());
	nfa.addTransition(2, 3, 'a');
	nfa.addTransition(4, 5, 'b');
	nfa.addTransition(3, 6, Transition());
	nfa.addTransition(5, 6, Transition());
	nfa.addTransition(6, 1, Transition());
	nfa.addTransition(6, 7, Transition());
	nfa.addTransition(7, 8, 'a');
	nfa.addTransition(8, 9, 'b');
	nfa.addTransition(6, 7, Transition());
	nfa.addTransition(9, 10, 'b');
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

// Test suits

void simplifierSuit()
{
	cute::suite s;
	s += CUTE(testNFAToDFA);
	cute::runner<cute::ostream_listener>()(s, "Simplifier Test");
}