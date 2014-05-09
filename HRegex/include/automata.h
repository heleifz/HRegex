#ifndef _HREG_AUTOMATA_
#define _HREG_AUTOMATA_

/*

	有限自动机的抽象

	字符全部以 Unicode 保存

*/

#include "globals.h"
#include "encoding.h"
#include "containers.h"

struct Range
{
	UnicodeChar lower;
	UnicodeChar upper;
	bool operator==(const Range& other) const
	{
		return lower == lower && upper == upper;
	}
};

class RangeSet
{
public:
	void insert(const Range& r)
	{
		ranges.push_back(r);
	}
	bool contains(UnicodeChar ch) const
	{
		for (auto i = ranges.begin(); i != ranges.end(); ++i)
		{
			if (ch >= i->lower && ch <= i->upper)
			{
				return true;
			}
		}
		return false;
	}
	bool operator==(const RangeSet& other) const
	{
		return ranges == other.ranges;
	}
	std::vector<Range>::const_iterator begin() const
	{
		return ranges.begin();
	}
	std::vector<Range>::const_iterator end() const
	{
		return ranges.end();
	}
private:
	std::vector<Range> ranges;
};

class Transition
{
public:
	enum TransitionType
	{
		NORMAL,
		EPSILON,
		WILDCARD,
		RANGE
	};

	Transition(UnicodeChar m)
		: type(NORMAL)
	{ 
		data.match = m; 
	}
	Transition(const RangeSet& st)
		: type(RANGE), rangeSet(st)
	{
	}
	Transition(TransitionType tp)
		: type(tp)
	{
	}

	bool check(UnicodeChar input) const
	{
		switch (type)
		{
		case NORMAL:
			return data.match == input;
		case EPSILON:
			return true;
		case WILDCARD:
			return true;
		case RANGE:
			return rangeSet.contains(input);
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
		case Transition::RANGE:
			return rangeSet == other.rangeSet;
		case Transition::EPSILON:
		case Transition::WILDCARD:
			return true;
		default:
			return false;
		}
	}

	std::string toString() const
	{
		std::stringstream ss;
		switch (type)
		{
		case Transition::NORMAL:
			ss << "Normal" << "[" << data.match << "]";
			return ss.str();
		case Transition::EPSILON:
			return std::string("Episilon");
		case Transition::WILDCARD:
			return std::string("Wildcard");
		case Transition::RANGE:
			ss << "Range" << "[";
			for (auto i = rangeSet.begin(); i != rangeSet.end(); ++i)
			{
				ss << "(" << i->lower << "," << i->upper << ")";
			}
			ss << "]";
			return ss.str();
		default:
			return std::string("Illegal Transition");
		}
	}

private:
	TransitionType type;
	union u
	{
		UnicodeChar match;
	} data;
	RangeSet rangeSet;
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

	Automata() {}

	// NFA construction
	void clear()
	{
		adj.clear();
		start.clear();
		terminate.clear();
	}

	State generateState()
	{
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
		start.insert(s);
	}

	void setTerminate(State s)
	{
		if (s >= adj.size())
		{
			throw IllegalStateError();
		}
		terminate.insert(s);
	}

	const std::vector<Edge>& getNeighbours(State s) const
	{
		if (s > adj.size())
		{
			throw IllegalStateError();
		}
		return adj[s];
	}

	SortedVectorSet<State> getAllStates() const
	{
		SortedVectorSet<State> ret;
		for (auto i = 0; i != size(); ++i)
		{
			ret.insert(i);
		}
		return ret;
	}

	SortedVectorSet<State> getStart() const
	{
		return start;
	}

	SortedVectorSet<State> getTerminate() const
	{
		return terminate;
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
		return start.contains(s);
	}

	bool isTerminate(State s) const
	{
		if (s >= adj.size())
		{
			throw IllegalStateError();
		}
		return terminate.contains(s);
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

	SortedVectorSet<State> epsilonClosure(const SortedVectorSet<State>& states) const
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

	SortedVectorSet<State> move(const SortedVectorSet<State>& states, Transition t) const
	{
		SortedVectorSet<State> destinations;

		// !!! IGNORE EPSILON TRANSITIONS

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

	SortedVectorSet<State> move(const SortedVectorSet<State>& states, UnicodeChar input) const
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

	Automata reverseEdges() const
	{
		Automata ret;
		for (size_t i = 0; i != size(); ++i)
		{
			ret.generateState();
		}
		for (auto i = adj.begin(); i != adj.end(); ++i)
		{
			for (auto j = i->begin(); j != i->end(); ++j)
			{
				ret.addTransition(j->getTo(), j->getFrom(), j->getTransition());
			}
		}
		ret.start = terminate;
		ret.terminate = start;
		return ret;
	}

	template <EncodeType E>
	bool simulate(typename Encode<E>::PointerType str, size_t length) const
	{
		StreamReader<E> reader(str);
		SortedVectorSet<State> states = getStart();
		for (size_t i = 0; i < length; ++i)
		{
			states = move(epsilonClosure(states), reader.next());
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
			if (isStart(j))
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
	SortedVectorSet<State> start;
	SortedVectorSet<State> terminate;
	std::vector<std::vector<Edge>> adj;
};

#endif