#ifndef _HREG_SIMPLIFIER_
#define _HREG_SIMPLIFIER_

#include "automata.h"

class Simplifier
{
public:
	// convert NFA to DFA using subset construction algorithm
	static Automata NFAToDFA(const Automata& nfa)
	{
		Automata dfa;
		if (nfa.size() == 0)
		{
			return dfa;
		}
		std::map<SortedVectorSet<State>, State> setToState;
		SortedVectorSet<State> start;
		start.insert(nfa.getStart());
		start = nfa.epsilonClosure(start);
		State dfaStart = dfa.generateState();
		setToState[start] = dfaStart;
		dfa.setStart(dfaStart);
		std::stack<SortedVectorSet<State>> stk;
		stk.push(start);
		while (!stk.empty())
		{
			auto current = stk.top();
			auto currentState = setToState[current];
			stk.pop();
			auto transitions = nfa.getNoneEpsilonTransitions(current);
			for (auto t = transitions.begin(); t != transitions.end(); ++t)
			{
				auto next = nfa.move(current, *t);
				next = nfa.epsilonClosure(next);
				auto result = setToState.find(next);
				State dest;
				if (result != setToState.end())
				{
					dest = result->second;
				}
				else
				{
					dest = dfa.generateState();
					setToState[next] = dest;
					if (nfa.containsTerminate(next))
					{
						dfa.setTerminate(dest);
					}
					stk.push(next);
				}
				dfa.addTransition(currentState, dest, *t);
			}
		}
		return dfa;
	}

	// minimize DFA using Hopcroft's algorithm
	static Automata MinimizeDFA(const Automata& dfa)
	{
		return dfa;
	}
};

#endif