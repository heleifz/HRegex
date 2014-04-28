#ifndef _HREG_PARSER_
#define _HREG_PARSER_

#include "nfa.h"

/*	
	** Thompson Construction Algorithm **

	TODO : 
	+,?
	charactor class
	refactor

	// regular expression syntax : 

	re : term { "|" term }
	term : factor { factor }
	factor : primitive (*|+|?)
	primitive : "\" meta | alpha | (re)
	meta : "n" "{" "|" "}" "(" ")" "t" "+" "?"

	// parse function return start and end state 
	// of the constructed subgraph

	void parseXXXX(Node& start, Node& end)

*/

class Parser
{
public:
	Parser(const char *input, NFA& automata)
		: re(input), nfa(automata)
	{
		nfa.clear();
		State s;
		State e;
		parseRE(s, e);
		if (*re != '\0')
		{
			throw ParseError();
		}
		nfa.setStart(s);
		nfa.setTerminate(e);
	}
private:
	void parseRE(State& start, State& end)
	{
		State s1;
		State s2;
		parseTerm(s1, s2);
		if (*re == '|')
		{
			start = nfa.generateState();
			end = nfa.generateState();
			nfa.addTransition(std::make_shared<NFAEdge>(start, s1, EPSTRANS));
			nfa.addTransition(std::make_shared<NFAEdge>(s2, end, EPSTRANS));
			while (*re == '|')
			{
				re++;
				parseTerm(s1, s2);
				nfa.addTransition(std::make_shared<NFAEdge>(start, s1, EPSTRANS));
				nfa.addTransition(std::make_shared<NFAEdge>(s2, end, EPSTRANS));
			}
		}
		else
		{
			start = s1;
			end = s2;
		}

	}
	void parseTerm(State& start, State& end)
	{
		parseFactor(start, end);
		State current = end;
		while (*re != '\0' && *re != '|' && *re != ')')
		{
			State s1;
			State s2;
			parseFactor(s1, s2);
			nfa.addTransition(std::make_shared<NFAEdge>(current, s1, EPSTRANS));
			current = s2;
		}
		end = current;
	}
	void parseFactor(State& start, State& end)
	{
		State s1;
		State s2;
		parsePrimitive(s1, s2);
		if (*re == '*')
		{
			start = nfa.generateState();
			end = nfa.generateState();
			nfa.addTransition(std::make_shared<NFAEdge>(start, s1, EPSTRANS));
			nfa.addTransition(std::make_shared<NFAEdge>(start, end, EPSTRANS));
			nfa.addTransition(std::make_shared<NFAEdge>(s2, end, EPSTRANS));
			nfa.addTransition(std::make_shared<NFAEdge>(s2, s1, EPSTRANS));
			re++;
		}
		else
		{
			start = s1;
			end = s2;
		}
	}
	void parsePrimitive(State& start, State& end)
	{
		// lookahead
		switch (*re)
		{
		case '\\':
			re++;
			start = nfa.generateState();
			end = nfa.generateState();
			switch (*re)
			{
			case 'n':
				nfa.addTransition(std::make_shared<NFAEdge>(start, end, '\n'));
				break;
			case 't':
				nfa.addTransition(std::make_shared<NFAEdge>(start, end, '\t'));
				break;
			case '{': case '}': case '|':
			case '(': case ')':
				nfa.addTransition(std::make_shared<NFAEdge>(start, end, *re));
				break;
			default:
				throw ParseError();
			}
			re++;
			break;
		case '(':
			re++;
			parseRE(start, end);
			if (*re != ')')
			{
				throw ParseError();
			}
			re++;
			break;
		default:
			start = nfa.generateState();
			end = nfa.generateState();
			nfa.addTransition(std::make_shared<NFAEdge>(start, end, *re));
			re++;
			break;
		}
	}

	const char *re;
	NFA& nfa;
};

#endif