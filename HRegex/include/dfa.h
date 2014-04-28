#ifndef _HREG_DFA_
#define _HREG_DFA_

#include "nfa.h"

class DFA
{

};

class NFAToDFAConverter : public NotCopyable
{
	NFAToDFAConverter(const NFA& n, DFA& d)
		: nfa(n)
	{
	}
private:
	std::map<SortedVectorSet<State>, State> setToState;
	const NFA& nfa;
};

#endif