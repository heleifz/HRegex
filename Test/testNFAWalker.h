/************************************************************************/
/*  Test NFA Matcher                                                    */
/************************************************************************/

#include "nfa_walker.h"
#include "cute/cute.h"

void testEpsilonClosure()
{
	NFA nfa;
	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();
	State s4 = nfa.generateState();
	State s5 = nfa.generateState();
	State s6 = nfa.generateState();

	nfa.addTransition(std::make_shared<NFAEdge>(s1, s2, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(s2, s1, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(s1, s3, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(s2, s4, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(s1, s5, 'K'));
	nfa.addTransition(std::make_shared<NFAEdge>(s5, s6, EPSTRANS));

	std::set<State> states;
	states.insert(s1);
	NFAWalker walker(nfa);
	walker.epsilonClosure(states);
	ASSERT(states.find(s1) != states.end());
	ASSERT(states.find(s2) != states.end());
	ASSERT(states.find(s3) != states.end());
	ASSERT(states.find(s4) != states.end());
	ASSERT(states.find(s5) == states.end());
	ASSERT(states.find(s6) == states.end());
}

void testStep()
{
	NFA nfa;
	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();
	State s4 = nfa.generateState();
	State s5 = nfa.generateState();
	State s6 = nfa.generateState();

	nfa.addTransition(std::make_shared<NFAEdge>(s1, s2, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(s2, s1, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(s1, s3, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(s2, s4, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(s1, s5, 'K'));
	nfa.addTransition(std::make_shared<NFAEdge>(s5, s6, EPSTRANS));

	std::set<State> states;
	states.insert(s1);
	NFAWalker walker(nfa);
	walker.epsilonClosure(states);
	auto result = walker.step(states, EPSTRANS);
	ASSERT_EQUAL(0, result.size());
	result = walker.step(states, 'K');
	ASSERT_EQUAL(1, result.size());
	ASSERT(result.find(s5) != result.end());
}

void testMatch()
{
	NFA nfa;
	for (int i = 0; i != 11; ++i)
	{
		nfa.generateState();
	}

	// NFA for (a|b)*abb

	nfa.addTransition(std::make_shared<NFAEdge>(0, 1, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(0, 7, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(1, 2, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(1, 4, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(2, 3, 'a'));
	nfa.addTransition(std::make_shared<NFAEdge>(4, 5, 'b'));
	nfa.addTransition(std::make_shared<NFAEdge>(3, 6, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(5, 6, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(6, 1, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(6, 7, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(7, 8, 'a'));
	nfa.addTransition(std::make_shared<NFAEdge>(8, 9, 'b'));
	nfa.addTransition(std::make_shared<NFAEdge>(6, 7, EPSTRANS));
	nfa.addTransition(std::make_shared<NFAEdge>(9, 10, 'b'));
	nfa.setStart(0);
	nfa.setTerminate(10);
	
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
}

void nfaWalkerSuit()
{
	cute::suite s;
	s += CUTE(testEpsilonClosure);
	s += CUTE(testStep);
	s += CUTE(testMatch);
	cute::runner<cute::ostream_listener>()(s, "NFA Matcher Test");
}
