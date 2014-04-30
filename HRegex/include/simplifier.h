#ifndef _HREG_SIMPLIFIER_
#define _HREG_SIMPLIFIER_

#include "automata.h"

class Simplifier
{
public:
	// convert NFA to DFA using subset construction algorithm
	// see http://en.wikipedia.org/wiki/Powerset_construction
	static Automata NFAToDFA(const Automata& nfa)
	{
		Automata dfa;
		if (nfa.size() == 0)
		{
			return dfa;
		}
		std::map<SortedVectorSet<State>, State> setToState;
		SortedVectorSet<State> start = nfa.getStart();
		start = nfa.epsilonClosure(start);
		State dfaStart = dfa.generateState();
		setToState[start] = dfaStart;
		dfa.setStart(dfaStart);
		if (nfa.containsTerminate(start))
		{
			dfa.setTerminate(dfaStart);
		}
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
	// see http://en.wikipedia.org/wiki/DFA_minimization
	// ! NOTE : A straight-forward implementation, it's not efficient for now
	static Automata MinimizeDFA(const Automata& dfa)
	{
		Automata reversed = dfa.reverseEdges();
		SortedVectorSet<SortedVectorSet<State>> workList;
		std::vector<SortedVectorSet<State>> groups;
		// initialize worklist and group set
		auto terminates = dfa.getTerminate();
		auto rest = dfa.getAllStates() - terminates;
		groups.push_back(terminates);
		groups.push_back(rest);
		workList.insert(terminates);

		while (!workList.isEmpty())
		{
			auto current = workList.last();
			workList.popBack();
			auto inEdges = reversed.getNoneEpsilonTransitions(current);
			for (auto i = inEdges.begin(); i != inEdges.end(); ++i)
			{
				auto projection = reversed.move(current, *i);
				// new groups to be added in next iteration
				// (cannot be directly inserted into groups vector, because
				//  that will invalidate the vector iterator)
				std::vector<SortedVectorSet<State>> added;
				for (auto j = groups.begin(); j != groups.end(); )
				{
					auto intersection = projection && (*j);
					auto restStates = (*j) - intersection;
					if ((!intersection.isEmpty()) &&
						(!restStates.isEmpty()))
					{
						added.push_back(intersection);
						added.push_back(restStates);
						if (workList.contains(*j))
						{
							workList.remove(*j);
							workList.insert(intersection);
							workList.insert(restStates);
						}
						else if (intersection.size() <= restStates.size())
						{
							workList.insert(intersection);
						}
						else
						{
							workList.insert(restStates);
						}
						j = groups.erase(j);
					}
					else
					{
						j++;
					}
				}
				// update group
				groups.insert(groups.end(), added.begin(), added.end());
			}
		}
		// convert groups to new DFA
		Automata minimized;
		// remove empty group
		groups.erase(std::remove_if(groups.begin(), groups.end(),
		[&](const SortedVectorSet<State>& s) {
			return s.isEmpty();
		}), groups.end());
		if (groups.empty())
		{
			return minimized;
		}
		// map each state to group index
		std::vector<size_t> groupMap(dfa.size());
		for (size_t i = 0; i != groups.size(); ++i)
		{
			minimized.generateState();
		}
		for (size_t i = 0; i != groups.size(); ++i)
		{
			for (auto j = groups[i].begin(); j != groups[i].end(); ++j)
			{
				groupMap[*j] = i;
				if (dfa.isTerminate(*j))
				{
					minimized.setTerminate(i);
				}
				if (dfa.isStart(*j))
				{
					minimized.setStart(i);
				}
			}
		}
		for (State i = 0; i != groups.size(); ++i)
		{
			auto edges = dfa.getNeighbours(groups[i].last());
			for (auto j = edges.begin(); j != edges.end(); ++j)
			{
				minimized.addTransition(i, groupMap[j->getTo()], j->getTransition());
			}
		}
		return minimized;
	}
};

#endif