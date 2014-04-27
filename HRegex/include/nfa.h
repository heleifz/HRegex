#ifndef _HREG_NFA_
#define _HREG_NFA_

#include "globals.h"


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
		start.clear();
		terminateFlags.clear();
	}

	State generateState()
	{
		terminateFlags.push_back(false);
		adj.push_back(std::vector<std::shared_ptr<NFAEdge>>());
		return adj.size() - 1;
	}

	void addTransition(std::shared_ptr<NFAEdge> edge)
	{
		adj[edge->getFrom()].push_back(edge);
	}

	void setStart(State s)
	{
		if (s >= adj.size())
		{
			throw IllegalStateError();
		}
		start.insert(s);
	}

	void setTerminate(State s)
	{
		if (s >= adj.size())
		{
			throw IllegalStateError();
		}
		terminateFlags[s] = true;
	}

	const std::vector<std::shared_ptr<NFAEdge>>& getNeighbours(State s) const
	{
		if (s > adj.size())
		{
			throw IllegalStateError();
		}
		return adj[s];
	}

	const std::set<State>& getStart() const
	{
		return start;
	}

	size_t size() const
	{
		return adj.size();
	}

	bool isStart(State s) const
	{
		if (s >= adj.size())
		{
			throw IllegalStateError();
		}
		return start.find(s) != start.end();
	}

	bool isTerminate(State s) const
	{
		if (s >= adj.size())
		{
			throw IllegalStateError();
		}
		return terminateFlags[s];
	}

	bool containsTerminate(const std::set<State>& s) const
	{
		return std::find_if(s.begin(), s.end(), [&](State st)
		{
			return isTerminate(st);
		}) != s.end();
	}

	std::shared_ptr<NFAEdge> getTransition(State s, Transition t) const
	{
		if (s >= adj.size())
		{
			throw IllegalStateError();
		}
		auto result = std::find_if(adj[s].begin(), adj[s].end(),
			[t](const std::shared_ptr<NFAEdge>& e)
			{
				return e->getTransition() == t;
			});

		if (result == adj[s].end())
		{
			return nullptr;
		}
		else
		{
			return (*result);
		}
	}

private:
	std::set<State> start;
	std::vector<bool> terminateFlags;
	std::vector<std::vector<std::shared_ptr<NFAEdge>>> adj;
};

#endif