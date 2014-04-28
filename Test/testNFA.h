/************************************************************************/
/*  Test NFA                                                            */
/************************************************************************/

#include "nfa.h"
#include "cute/cute.h"

void testTransitionEqualAndType()
{
	Transition t1;
	ASSERT_EQUAL(Transition::EPSILON, t1.getType());
	Transition t2('c');
	ASSERT_EQUAL(Transition::NORMAL, t2.getType());
	ASSERT(!(t1 == t2));
	Transition t3('c');
	Transition t4;
	ASSERT(t3 == t2);
	ASSERT(t1 == t4);
}

void testTransitionCheck()
{
	Transition t1;
	Transition t2('c');
	ASSERT(t1.check('a'));
	ASSERT(t1.check('b'));
	ASSERT(t1.check('z'));
	ASSERT(!t2.check('z'));
	ASSERT(!t2.check('b'));
	ASSERT(!t2.check('z'));
	ASSERT(t2.check('c'));
}

void testSize()
{
	NFA nfa;
	ASSERT_EQUAL(0, nfa.size());
	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();
	ASSERT_EQUAL(3, nfa.size());
	nfa.clear();
	ASSERT_EQUAL(0, nfa.size());
}

void testAddTransition()
{
	NFA nfa;
	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();

	ASSERT_EQUAL(0, s1);
	ASSERT_EQUAL(1, s2);
	ASSERT_EQUAL(2, s3);
	nfa.addTransition(s1, s2, 'B');
	nfa.addTransition(s1, s3, 'C');
	ASSERT_EQUAL(nfa.getNeighbours(s1)[0].getTo(), s2);
	ASSERT_EQUAL(nfa.getNeighbours(s1)[1].getTo(), s3);
	ASSERT(nfa.getNeighbours(s3).empty());
	ASSERT(nfa.getNeighbours(s2).empty());
}

void testStartTerminate()
{
	NFA nfa;

	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();
	State s4 = nfa.generateState();

	nfa.setStart(s1);
	nfa.setTerminate(s3);

	ASSERT(nfa.isStart(s1));
	ASSERT(nfa.isTerminate(s3));
	ASSERT(!nfa.isTerminate(s2));
	ASSERT(!nfa.isTerminate(s4));
	ASSERT(!nfa.isStart(s2));
	ASSERT(!nfa.isStart(s4));
}

void testTerminateSet()
{
	NFA nfa;

	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();
	State s4 = nfa.generateState();

	nfa.setStart(s1);
	nfa.setTerminate(s3);
	nfa.setTerminate(s4);

	SortedVectorSet<State> s;
	ASSERT(!nfa.containsTerminate(s));
	s.insert(s1);
	s.insert(s2);
	ASSERT(!nfa.containsTerminate(s));
	s.insert(s3);
	ASSERT(nfa.containsTerminate(s));
	s.clear();
	s.insert(s4);
	ASSERT(nfa.containsTerminate(s));
}

void testGetAllTransitions()
{
	NFA nfa;
	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();

	nfa.addTransition(s1, s2, 'B');
	nfa.addTransition(s1, s3, 'C');

	SortedVectorSet<State> states;
	auto result = nfa.getAllTransitions(states);
	ASSERT_EQUAL(0, result.size());
	states.insert(s1);
	result = nfa.getAllTransitions(states);
	ASSERT_EQUAL(2, result.size());
	ASSERT(std::find(result.begin(), result.end(), 'B') != result.end());
	ASSERT(std::find(result.begin(), result.end(), 'C') != result.end());
	nfa.addTransition(s1, s2, 'B');
	result = nfa.getAllTransitions(states);
	ASSERT_EQUAL(2, result.size());
}

void testEpsilonClosure()
{
	NFA nfa;
	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();
	State s4 = nfa.generateState();
	State s5 = nfa.generateState();
	State s6 = nfa.generateState();

	nfa.addTransition(s1, s2, Transition());
	nfa.addTransition(s2, s1, Transition());
	nfa.addTransition(s1, s3, Transition());
	nfa.addTransition(s2, s4, Transition());
	nfa.addTransition(s1, s5, 'K');
	nfa.addTransition(s5, s6, Transition());

	SortedVectorSet<State> states;
	states.insert(s1);
	nfa.epsilonClosure(states);
	ASSERT(states.contains(s1));
	ASSERT(states.contains(s2));
	ASSERT(states.contains(s3));
	ASSERT(states.contains(s4));
	ASSERT(!states.contains(s5));
	ASSERT(!states.contains(s6));
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

	nfa.addTransition(s1, s2, Transition());
	nfa.addTransition(s2, s1, Transition());
	nfa.addTransition(s1, s3, Transition());
	nfa.addTransition(s2, s4, Transition());
	nfa.addTransition(s1, s5, 'K');
	nfa.addTransition(s5, s6, Transition());

	SortedVectorSet<State> states;
	states.insert(s1);
	nfa.epsilonClosure(states);
	auto result = nfa.step(states, Transition());
	ASSERT_EQUAL(0, result.size());
	result = nfa.step(states, 'K');
	ASSERT_EQUAL(1, result.size());
	ASSERT(result.contains(s5));
}

void testMatch()
{
	NFA nfa;
	for (int i = 0; i != 11; ++i)
	{
		nfa.generateState();
	}

	// NFA for (a|b)*abb

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
}


// Test suits

void nfaSuit()
{
	cute::suite s;
	s += CUTE(testTransitionEqualAndType);
	s += CUTE(testTransitionCheck);
	s += CUTE(testSize);
	s += CUTE(testAddTransition);
	s += CUTE(testStartTerminate);
	s += CUTE(testTerminateSet);
	s += CUTE(testGetAllTransitions);
	s += CUTE(testEpsilonClosure);
	s += CUTE(testMatch);
	s += CUTE(testStep);
	cute::runner<cute::ostream_listener>()(s, "NFA Test");
}