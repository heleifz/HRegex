#include "cute/cute.h"

#include "nfa.h"

void testAddTransition()
{
	NFA nfa;
	State s1 = nfa.generateState();
	State s2 = nfa.generateState();
	State s3 = nfa.generateState();

	ASSERT_EQUAL(0, s1);
	ASSERT_EQUAL(1, s2);
	ASSERT_EQUAL(2, s3);

	nfa.addTransition(std::make_shared<NFAEdge>(s1, s2, 'B'));
	nfa.addTransition(std::make_shared<NFAEdge>(s1, s3, 'C'));
	
	ASSERT_EQUAL(nfa.getNeighbour(s1)[0]->getTo(), s2);
	ASSERT_EQUAL(nfa.getNeighbour(s1)[1]->getTo(), s3);
	ASSERT(nfa.getNeighbour(s3).empty());
	ASSERT(nfa.getNeighbour(s2).empty());
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

	std::set<State> s;
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

void nfaSuit()
{
	cute::suite s;
	s += CUTE(testAddTransition);
	s += CUTE(testStartTerminate);
	s += CUTE(testTerminateSet);
	cute::runner<cute::ostream_listener>()(s, "NFA Test");
}

int main()
{
	nfaSuit();
	system("pause");
}