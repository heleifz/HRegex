#include "globals.h"

#define EPSTRANS -1

class NFAEdge
{
public:
	NFAEdge(State f, State t, Transition tran)
		: from(f), to(t), transition(tran)
	{ 
	}
	State getFrom() const
	{
		return from;
	}
	State getTo() const
	{
		return to;
	}
	Transition getTransition() const
	{
		return transition;
	}
private:
	State from;
	State to;
	Transition transition;
};

class NFA
{
public:

	NFA() {}

	// NFA construction
	void clear()
	{
		adj.clear();
		starts.clear();
		terminates.clear();
	}

	State generateState()
	{
		adj.push_back(std::vector<std::shared_ptr<NFAEdge>>());
		return adj.size() - 1;
	}

	void addTransition(std::shared_ptr<NFAEdge> edge)
	{
		adj[edge->getFrom()].push_back(edge);
	}

	const std::vector<std::shared_ptr<NFAEdge>>& getNeighbour(State s)
	{
		if (s > adj.size())
		{
			throw IllegalStateError();
		}
		return adj[s];
	}

	void setStart(State s)
	{
		if (s >= adj.size())
		{
			throw IllegalStateError();
		}
		starts.insert(s);
	}

	void setTerminate(State s)
	{
		if (s >= adj.size())
		{
			throw IllegalStateError();
		}
		terminates.insert(s);
	}

	bool isStart(State s) const
	{
		return (starts.find(s) != starts.end());
	}

	bool isTerminate(State s) const
	{
		return (terminates.find(s) != terminates.end());
	}

	// ! 
	bool containsTerminate(const std::set<State>& s) const
	{
		return std::find_if(s.begin(), s.end(), [&](State st) {
			return isTerminate(st);
		}) != s.end();
	}

	//std::set<State> epsilonClosure(const std::set<State>& states);
	//std::set<State> step(const std::set<State>& states);

private:

	std::set<State> starts;
	std::set<State> terminates;
	std::vector<std::vector<std::shared_ptr<NFAEdge>>> adj;
};