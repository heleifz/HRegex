#ifndef _HREG_AUTOMATA_
#define _HREG_AUTOMATA_

#include "globals.h"
#include "containers.h"

class Transition
{
public:
	enum TransitionType
	{
		NORMAL,
		EPSILON,
		WILDCARD,
		DIGIT
	};

	Transition(char m) : type(NORMAL) { data.match = m; }
	Transition(TransitionType tp) : type(tp) { }
	Transition() : type(EPSILON) { }

	bool check(char input) const
	{
		switch (type)
		{
		case NORMAL:
			return data.match == input;
		case EPSILON:
			return true;
		case WILDCARD:
			return true;
		case DIGIT:
			return (input >= '0' && input <= '9');
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
		case Transition::WILDCARD:
		case Transition::DIGIT:
			return true;
			break;
		default:
			return false;
			break;
		}
	}

	std::string toString() const
	{
		switch (type)
		{
		case Transition::NORMAL:
			return std::string("Normal") + "[" + data.match + "]";
			break;
		case Transition::EPSILON:
			return std::string("Episilon");
			break;
		case Transition::WILDCARD:
			return std::string("Wildcard");
			break;
		case Transition::DIGIT:
			return std::string("Digit");
			break;
		default:
			return std::string("Illegal Transition");
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

class Edge
{
public:
	Edge(State f, State t, Transition tran)
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
	std::string toString() const
	{
		std::stringstream ss;
		ss << "From " << from << " To " << to << " : " << transition.toString();
		return ss.str();
	}
private:
	State from;
	State to;
	Transition transition;
};

class Automata
{
public:

	Automata() : hasStart(false) {}

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
		adj.push_back(std::vector<Edge>());
		return adj.size() - 1;
	}

	void addTransition(State from, State to, const Transition& t)
	{
		adj[from].push_back(Edge(from, to, t));
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

	const std::vector<Edge>& getNeighbours(State s) const
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

	std::vector<Transition> getNoneEpsilonTransitions(const SortedVectorSet<State>& states) const
	{
		std::vector<Transition> transitions;
		for (auto i = states.begin(); i != states.end(); ++i)
		{
			for (auto j = getNeighbours(*i).begin(); j != getNeighbours(*i).end(); ++j)
			{
				Transition t = j->getTransition();
				if (t.getType() == Transition::EPSILON)
				{
					continue;
				}
				if (std::find(transitions.begin(), transitions.end(), t) == transitions.end())
				{
					transitions.push_back(j->getTransition());
				}
			}
		}
		return transitions;
	}

	SortedVectorSet<State> epsilonClosure(SortedVectorSet<State>& states) const
	{
		// use DFS to find eps-closure
		std::vector<bool> mark(size(), false);
		std::stack<State> stk;
		SortedVectorSet<State> epsilon = states;
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
					epsilon.insert(foo);
					mark[foo] = true;
					stk.push(foo);
				}
			}
		}
		return epsilon;
	}

	SortedVectorSet<State> move(SortedVectorSet<State>& states, Transition t) const
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

	SortedVectorSet<State> move(SortedVectorSet<State>& states, char input) const
	{
		SortedVectorSet<State> destinations;
		for (auto i = states.begin(); i != states.end(); ++i)
		{
			for (auto j = getNeighbours(*i).begin(); j != getNeighbours(*i).end(); ++j)
			{
				Transition t = j->getTransition();
				if (t.getType() != Transition::EPSILON &&
					t.check(input))
				{
					destinations.insert(j->getTo());
				}
			}
		};
		return destinations;
	}

	bool simulate(const char *str, size_t length) const
	{
		SortedVectorSet<State> states;
		states.insert(getStart());
		for (size_t i = 0; i < length; ++i)
		{
			states = move(epsilonClosure(states), str[i]);
		}
		return containsTerminate(epsilonClosure(states));
	}

	std::string toString() const
	{
		std::stringstream ss;
		ss << "The automata has " << size() << " state(s):\n";
		int j = 0;
		for (auto i = adj.begin(); i != adj.end(); i++, j++)
		{
			ss << "State " << j;
			if (isTerminate(j))
			{
				ss << "(terminate)";
			}
			else if (isStart(j))
			{
				ss << "(start)";
			}
			ss << "\n============\n";
			for (auto k = (*i).begin(); k != (*i).end(); ++k)
			{
				ss << k->toString() << "\n";
			}
			ss << "============\n";
		}
		return ss.str();
	}

private:
	State start;
	bool hasStart;
	std::vector<bool> terminateFlags;
	std::vector<std::vector<Edge>> adj;
};

#endif