#ifndef _HREG_PARSER_
#define _HREG_PARSER_

#include "automata.h"

/*	
	** Thompson Construction Algorithm **

	TODO : 
	charactor class (suffix tree)
	如何支持字符类
	如果是查表的话，行和列要如何设计

	// regular expression syntax : 

	re : term { "|" term }
	term : factor { factor }
	factor : primitive (*|+|?)
	primitive : "\" meta | alpha | (re) | charclass
	meta : "n" "{" "|" "}" "(" ")" "t" "+" "?"
	charclass "[" -- TODO -- "]"

	// parse function return start and end state 
	// of the constructed subgraph

	void parseXXXX(Node& start, Node& end)

*/

class Parser : public NotCopyable
{
public:
	Parser(const char *input, Automata& automata)
		: re(input), nfa(automata)
	{
		nfa.clear();
		// empty string
		if (*re == '\0')
		{
			return;
		}
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
			nfa.addTransition(start, s1, Transition::EPSILON);
			nfa.addTransition(s2, end, Transition::EPSILON);
			while (*re == '|')
			{
				re++;
				parseTerm(s1, s2);
				nfa.addTransition(start, s1, Transition::EPSILON);
				nfa.addTransition(s2, end, Transition::EPSILON);
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
			nfa.addTransition(current, s1, Transition::EPSILON);
			current = s2;
		}
		end = current;
	}
	void parseFactor(State& start, State& end)
	{
		State s1;
		State s2;
		parsePrimitive(s1, s2);
		switch (*re)
		{
		case '?':
			nfa.addTransition(s1, s2, Transition::EPSILON);
			start = s1;
			end = s2;
			re++;
			break;
		case '*':
			start = nfa.generateState();
			end = nfa.generateState();
			nfa.addTransition(start, s1, Transition::EPSILON);
			nfa.addTransition(start, end, Transition::EPSILON);
			nfa.addTransition(s2, end, Transition::EPSILON);
			nfa.addTransition(s2, s1, Transition::EPSILON);
			re++;
			break;
		case '+':
			start = s1;
			end = nfa.generateState();
			nfa.addTransition(s2, end, Transition::EPSILON);
			nfa.addTransition(end, s1, Transition::EPSILON);
			re++;
			break;
		default:
			start = s1;
			end = s2;
			break;
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
				nfa.addTransition(start, end, static_cast<HRegexByte>('\n'));
				break;
			case 't':
				nfa.addTransition(start, end, static_cast<HRegexByte>('\t'));
				break;
			case 'd':
				nfa.addTransition(start, end, Transition(
					static_cast<HRegexByte>('0'),
					static_cast<HRegexByte>('9')));
				break;
			case '{': case '}': case '|':
			case '(': case ')': case '.':
			case '+': case '*': case '?':
			case '\\':
				nfa.addTransition(start, end, *re);
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
		case '\0': case '*': case '|':
			throw ParseError();
			break;
		case '.':
			start = nfa.generateState();
			end = nfa.generateState();
			nfa.addTransition(start, end, Transition::WILDCARD);
			re++;
			break;
		default:
			start = nfa.generateState();
			end = nfa.generateState();
			nfa.addTransition(start, end, *re);
			re++;
			break;
		}
	}

	const char *re;
	Automata& nfa;
};

#endif