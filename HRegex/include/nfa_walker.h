#ifndef _HREG_NFA_WALKER_
#define _HREG_NFA_WALKER_

#include "nfa.h"

class NFAWalker
{
public:
	NFAWalker(NFA& n) : nfa(n) {}
	
	void epsilonClosure(std::set<State>& states)
	{
		// use DFS to find eps-closure
		std::vector<bool> mark(nfa.size(), false);
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
			for (auto i = nfa.getNeighbours(s).begin(); i != nfa.getNeighbours(s).end(); ++i)
			{
				State foo = (*i)->getTo();
				if (!mark[foo] && (*i)->getTransition() == EPSTRANS)
				{
					states.insert(foo);
					mark[foo] = true;
					stk.push(foo);
				}
			}
		}
		return;
	}

	std::set<State> step(std::set<State>& states, Transition t)
	{
		std::set<State> destinations;
		if (t != EPSTRANS)
		{
			std::for_each(states.begin(), states.end(), [&](State s)
			{
				auto e = nfa.getTransition(s, t);
				if (e != nullptr)
				{
					destinations.insert(e->getTo());
				}
			});
		}
		return destinations;
	}

	bool match(const char *str, size_t length)
	{
		std::set<State> states = nfa.getStart();
		for (size_t i = 0; i < length; ++i)
		{
			epsilonClosure(states);
			states = step(states, str[i]);
		}
		epsilonClosure(states);
		return nfa.containsTerminate(states);
	}

private:
	const NFA& nfa;
};

#endif