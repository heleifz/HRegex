#ifndef _HREG_NFA_
#define _HREG_NFA_

#include "globals.h"
#include "containers.h"

class Transition
{
public:
	enum TransitionType
	{
		NORMAL,
		EPSILON
	};

	Transition(char m) : type(NORMAL) { data.match = m; }
	Transition() : type(EPSILON) { }

	bool check(char input) const
	{
		switch (type)
		{
		case NORMAL:
			return data.match == input;
		case EPSILON:
			return true;
		}
		return false;
	}

	TransitionType getType() const
	{
		return type;
	}

	bool operator==(const Transition& other) const
	{
		if (other.type != type)
		{
			return false;
		}
		switch (type)
		{
		case Transition::NORMAL:
			return data.match == other.data.match;
			break;
		case Transition::EPSILON:
			return true;
			break;
		default:
			return false;
			break;
		}
	}

private:
	TransitionType type;
	union u
	{
		char match;
	} data;
};

typedef size_t State;

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

	NFA() : hasStart(false) {}

	// NFA construction
	void clear()
	{
		adj.clear();
		hasStart = false;
		terminateFlags.clear();
	}

	State generateState()
	{
		terminateFlags.push_back(false);
		adj.push_back(std::vector<NFAEdge>());
		return adj.size() - 1;
	}

	void addTransition(State from, State to, const Transition& t)
	{
		adj[from].push_back(NFAEdge(from, to, t));
	}

	void setStart(State s)
	{
		if (s >= adj.size())
		{
			throw IllegalStateError();
		}
		hasStart = true;
		start = s;
	}

	void setTerminate(State s)
	{
		if (s >= adj.size())
		{
			throw IllegalStateError();
		}
		terminateFlags[s] = true;
	}

	const std::vector<NFAEdge>& getNeighbours(State s) const
	{
		if (s > adj.size())
		{
			throw IllegalStateError();
		}
		return adj[s];
	}

	State getStart() const
	{
		if (!hasStart)
		{
			throw IllegalStateError();
		}
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
		return (hasStart && s == start);
	}

	bool isTerminate(State s) const
	{
		if (s >= adj.size())
		{
			throw IllegalStateError();
		}
		return terminateFlags[s];
	}

	bool containsTerminate(const SortedVectorSet<State>& s) const
	{
		return std::find_if(s.begin(), s.end(), [&](State st)
		{
			return isTerminate(st);
		}) != s.end();
	}

	std::vector<Transition> getAllTransitions(const SortedVectorSet<State>& states) const
	{
		std::vector<Transition> transitions;
		for (auto i = states.begin(); i != states.end(); ++i)
		{
			for (auto j = getNeighbours(*i).begin(); j != getNeighbours(*i).end(); ++j)
			{
				Transition t = j->getTransition();
				if (std::find(transitions.begin(), transitions.end(), t) == transitions.end())
				{
					transitions.push_back(j->getTransition());
				}
			}
		}
		return transitions;
	}

	void epsilonClosure(SortedVectorSet<State>& states) const
	{
		// use DFS to find eps-closure
		std::vector<bool> mark(size(), false);
		std::stack<State> stk;
		for (auto i = states.begin(); i != states.end(); ++i)
		{
			mark[*i] = true;
			stk.push(*i);
		}
		while (!stk.empty())
		{
			State s = stk.top();
			stk.pop();
			for (auto i = getNeighbours(s).begin(); i != getNeighbours(s).end(); ++i)
			{
				State foo = i->getTo();
				if (!mark[foo] && i->getTransition().getType() == Transition::EPSILON)
				{
					states.insert(foo);
					mark[foo] = true;
					stk.push(foo);
				}
			}
		}
		return;
	}

	SortedVectorSet<State> step(SortedVectorSet<State>& states, Transition t) const
	{
		SortedVectorSet<State> destinations;
		// ignore epsilon transition
		if (t.getType() != Transition::EPSILON)
		{
			for(auto i = states.begin(); i != states.end(); ++i)
			{
				for (auto j = getNeighbours(*i).begin(); j != getNeighbours(*i).end(); ++j)
				{
					if (j->getTransition() == t)
					{
						destinations.insert(j->getTo());
					}
				}
			};
		}
		return destinations;
	}

	bool match(const char *str, size_t length) const
	{
		SortedVectorSet<State> states;
		states.insert(getStart());
		for (size_t i = 0; i < length; ++i)
		{
			epsilonClosure(states);
			states = step(states, str[i]);
		}
		epsilonClosure(states);
		return containsTerminate(states);
	}

private:
	State start;
	bool hasStart;
	std::vector<bool> terminateFlags;
	std::vector<std::vector<NFAEdge>> adj;
};

#endif