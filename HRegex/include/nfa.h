#include "globals.h"

class NFAEdge
{
public:
	NFAEdge(State f, State t, Transition tran);
	State getFrom();
	State getTo();
	State getTheOther(State n);
	Transition getTransition();
private:
	State from;
	State to;
	Transition transition;
};

class NFA
{
public:
	NFA(size_t sz);
	// NFA construction
	void clear();
	State generateState();
	void addTransition(std::shared_ptr<NFAEdge> edge);

	std::set<State> epsilonClosure(const std::set<State>& states);
	std::set<State> step(const std::set<State>& states);

private:
	std::vector<std::vector<std::shared_ptr<NFAEdge>>> adj;
};