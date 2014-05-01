/************************************************************************/
/*  Test Automata                                                            */
/************************************************************************/

// TODO : test input move

#include "automata.h"
#include "cute/cute.h"

void testTransitionEqualAndType()
{
	Transition t1(Transition::EPSILON);
	ASSERT_EQUAL(Transition::EPSILON, t1.getType());
	Transition t2(static_cast<HRegexByte>('c'));
	ASSERT_EQUAL(Transition::NORMAL, t2.getType());
	ASSERT(!(t1 == t2));
	Transition t3(static_cast<HRegexByte>('c'));
	Transition t4(Transition::EPSILON);
	ASSERT(t3 == t2);
	ASSERT(t1 == t4);
}

void testTransitionCheck()
{
	Transition t1(Transition::EPSILON);
	Transition t2(static_cast<HRegexByte>('c'));
	ASSERT(t1.check(static_cast<HRegexByte>('a')));
	ASSERT(t1.check(static_cast<HRegexByte>('b')));
	ASSERT(t1.check(static_cast<HRegexByte>('z')));
	ASSERT(!t2.check(static_cast<HRegexByte>('z')));
	ASSERT(!t2.check(static_cast<HRegexByte>('b')));
	ASSERT(!t2.check(static_cast<HRegexByte>('z')));
	ASSERT(t2.check(static_cast<HRegexByte>('c')));
}

void testSize()
{
	Automata nfa;
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
	Automata nfa;
	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();
	ASSERT_EQUAL(0, s1);
	ASSERT_EQUAL(1, s2);
	ASSERT_EQUAL(2, s3);
	nfa.addTransition(s1, s2, static_cast<HRegexByte>('B'));
	nfa.addTransition(s1, s3, static_cast<HRegexByte>('C'));
	ASSERT_EQUAL(nfa.getNeighbours(s1)[0].getTo(), s2);
	ASSERT_EQUAL(nfa.getNeighbours(s1)[1].getTo(), s3);
	ASSERT(nfa.getNeighbours(s3).empty());
	ASSERT(nfa.getNeighbours(s2).empty());
}

void testGetAllStates()
{	
	Automata nfa;
	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();
	State s4 = nfa.generateState();
	SortedVectorSet<State> s;
	s.insert(0);
	s.insert(1);
	s.insert(2);
	s.insert(3);
	ASSERT_EQUAL(s, nfa.getAllStates());
}

void testStartTerminate()
{
	Automata nfa;

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
	Automata nfa;

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

void testGetNoneEpsilonTransitions()
{
	Automata nfa;
	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();
	State s4 = nfa.generateState();

	nfa.addTransition(s1, s2, static_cast<HRegexByte>('B'));
	nfa.addTransition(s1, s3, static_cast<HRegexByte>('C'));
	nfa.addTransition(s1, s4, Transition::EPSILON);

	SortedVectorSet<State> states;
	auto result = nfa.getNoneEpsilonTransitions(states);
	ASSERT_EQUAL(0, result.size());
	states.insert(s1);
	result = nfa.getNoneEpsilonTransitions(states);
	ASSERT_EQUAL(2, result.size());
	ASSERT(std::find(result.begin(), result.end(), static_cast<HRegexByte>('B')) != result.end());
	ASSERT(std::find(result.begin(), result.end(), static_cast<HRegexByte>('C')) != result.end());
	nfa.addTransition(s1, s2, static_cast<HRegexByte>('B'));
	result = nfa.getNoneEpsilonTransitions(states);
	ASSERT_EQUAL(2, result.size());
}

void testEpsilonClosure()
{
	Automata nfa;
	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();
	State s4 = nfa.generateState();
	State s5 = nfa.generateState();
	State s6 = nfa.generateState();

	nfa.addTransition(s1, s2, Transition::EPSILON);
	nfa.addTransition(s2, s1, Transition::EPSILON);
	nfa.addTransition(s1, s3, Transition::EPSILON);
	nfa.addTransition(s2, s4, Transition::EPSILON);
	nfa.addTransition(s1, s5, static_cast<HRegexByte>('K'));
	nfa.addTransition(s5, s6, Transition::EPSILON);

	SortedVectorSet<State> states;
	states.insert(s1);
	states = nfa.epsilonClosure(states);
	ASSERT(states.contains(s1));
	ASSERT(states.contains(s2));
	ASSERT(states.contains(s3));
	ASSERT(states.contains(s4));
	ASSERT(!states.contains(s5));
	ASSERT(!states.contains(s6));
}

void testMove()
{
	Automata nfa;
	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();
	State s4 = nfa.generateState();
	State s5 = nfa.generateState();
	State s6 = nfa.generateState();

	nfa.addTransition(s1, s2, Transition::EPSILON);
	nfa.addTransition(s2, s1, Transition::EPSILON);
	nfa.addTransition(s1, s3, Transition::EPSILON);
	nfa.addTransition(s2, s4, Transition::EPSILON);
	nfa.addTransition(s1, s5, static_cast<HRegexByte>('K'));
	nfa.addTransition(s5, s6, Transition::EPSILON);

	SortedVectorSet<State> states;
	states.insert(s1);
	states = nfa.epsilonClosure(states);
	auto result = nfa.move(states, Transition::EPSILON);
	ASSERT_EQUAL(0, result.size());
	result = nfa.move(states, static_cast<HRegexByte>('K'));
	ASSERT_EQUAL(1, result.size());
	ASSERT(result.contains(s5));
}

void testReverseEdges()
{
	Automata nfa;
	ASSERT_EQUAL(0, nfa.reverseEdges().size());
	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();
	State s4 = nfa.generateState();
	nfa.addTransition(s1, s2, static_cast<HRegexByte>('B'));
	nfa.addTransition(s1, s3, static_cast<HRegexByte>('C'));
	nfa.addTransition(s1, s4, Transition::EPSILON);
	Automata r = nfa.reverseEdges();
	ASSERT_EQUAL(4, r.size());
	ASSERT_EQUAL(r.getNeighbours(2)[0].getTo(), 0);
	ASSERT_EQUAL(r.getNeighbours(2)[0].getTransition(), Transition(static_cast<HRegexByte>('C')));
	ASSERT_EQUAL(r.getNeighbours(1)[0].getTo(), 0);
	ASSERT_EQUAL(r.getNeighbours(1)[0].getTransition(), Transition(static_cast<HRegexByte>('B')));
	ASSERT_EQUAL(r.getNeighbours(3)[0].getTo(), 0);
	ASSERT_EQUAL(r.getNeighbours(3)[0].getTransition(), Transition(Transition::EPSILON));
	ASSERT(r.getNeighbours(0).empty());
	ASSERT_EQUAL(nfa.getTerminate(), r.getStart());
	ASSERT_EQUAL(nfa.getStart(), r.getTerminate());
}

void testSimulate()
{
	Automata nfa;
	for (int i = 0; i != 11; ++i)
	{
		nfa.generateState();
	}

	// Automata for (a|b)*abb

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
	nfa.addTransition(7, 8, static_cast<HRegexByte>('a'));
	nfa.addTransition(8, 9, static_cast<HRegexByte>('b'));
	nfa.addTransition(6, 7, Transition::EPSILON);
	nfa.addTransition(9, 10, static_cast<HRegexByte>('b'));
	nfa.setStart(0);
	nfa.setTerminate(10);

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
}

// Test suits

void automataSuit()
{
	cute::suite s;
	s += CUTE(testTransitionEqualAndType);
	s += CUTE(testTransitionCheck);
	s += CUTE(testSize);
	s += CUTE(testGetAllStates);
	s += CUTE(testAddTransition);
	s += CUTE(testStartTerminate);
	s += CUTE(testTerminateSet);
	s += CUTE(testGetNoneEpsilonTransitions);
	s += CUTE(testEpsilonClosure);
	s += CUTE(testSimulate);
	s += CUTE(testMove);
	s += CUTE(testReverseEdges);
	cute::runner<cute::ostream_listener>()(s, "Automata Test");
}