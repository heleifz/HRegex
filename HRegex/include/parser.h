#ifndef _HREG_PARSER_
#define _HREG_PARSER_

#include "automata.h"
#include "encoding.h"

/*	
	** Thompson Construction Algorithm **

	TODO : 
	{2, 5} = Parser<ASCII>("ss(s(ss?)?)?", a);
	{2, } = ss+
	{0} = empty

	character class : 现在的弄法是错的，考虑到negate操作，整个char class应该是个整体
	如何支持字符类
	如果是查表的话，行和列要如何设计

	// regular expression syntax : 

	re : term { "|" term }
	term : factor { factor }
	factor : primitive (*|+|?)
	primitive : "\" meta | alpha | (re) | charclass
	meta : "n" "{" "|" "}" "(" ")" "t" "+" "?"
	charclass "[" "^"? (alpha | alpha "-" alpha)+ "]"

	// parse function return start and end state 
	// of the constructed subgraph

	void parseXXXX(Node& start, Node& end)

*/

template <EncodeType E>
class Parser : public NotCopyable
{
public:
	Parser(typename Encode<E>::PointerType input, Automata& automata)
		: reader(input), nfa(automata)
	{
		nfa.clear();
		// empty string
		if (reader.peek() == '\0')
		{
			return;
		}
		State s;
		State e;
		parseRE(s, e);
		if (reader.peek() != '\0')
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
		if (reader.peek() == '|')
		{
			start = nfa.generateState();
			end = nfa.generateState();
			nfa.addTransition(start, s1, Transition::EPSILON);
			nfa.addTransition(s2, end, Transition::EPSILON);
			while (reader.peek() == '|')
			{
				reader.next();
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
		auto p = reader.peek();
		while (p != '\0' && p != '|' && p != ')')
		{
			State s1;
			State s2;
			parseFactor(s1, s2);
			nfa.addTransition(current, s1, Transition::EPSILON);
			current = s2;
			p = reader.peek();
		}
		end = current;
	}
	void parseFactor(State& start, State& end)
	{
		State s1;
		State s2;
		parsePrimitive(s1, s2);
		switch (reader.peek())
		{
		case '?':
			nfa.addTransition(s1, s2, Transition::EPSILON);
			start = s1;
			end = s2;
			reader.next();
			break;
		case '*':
			start = nfa.generateState();
			end = nfa.generateState();
			nfa.addTransition(start, s1, Transition::EPSILON);
			nfa.addTransition(start, end, Transition::EPSILON);
			nfa.addTransition(s2, end, Transition::EPSILON);
			nfa.addTransition(s2, s1, Transition::EPSILON);
			reader.next();
			break;
		case '+':
			start = s1;
			end = nfa.generateState();
			nfa.addTransition(s2, end, Transition::EPSILON);
			nfa.addTransition(end, s1, Transition::EPSILON);
			reader.next();
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
		switch (reader.peek())
		{
		case '\\':
			reader.next();
			start = nfa.generateState();
			end = nfa.generateState();
			switch (reader.peek())
			{
			case 'n':
				nfa.addTransition(start, end, '\n');
				break;
			case 't':
				nfa.addTransition(start, end, '\t');
				break;
			case 'd':
				nfa.addTransition(start, end, Transition('0', '9'));
				break;
			case '{': case '}': case '|':
			case '(': case ')': case '.':
			case '+': case '*': case '?':
			case '\\':
				nfa.addTransition(start, end, reader.peek());
				break;
			default:
				throw ParseError();
			}
			reader.next();
			break;
		case '(':
			reader.next();
			parseRE(start, end);
			if (reader.peek() != ')')
			{
				throw ParseError();
			}
			reader.next();
			break;
		case '\0': case '*': case '|':
			throw ParseError();
			break;
		case '.':
			start = nfa.generateState();
			end = nfa.generateState();
			nfa.addTransition(start, end, Transition::WILDCARD);
			reader.next();
			break;
		default:
			start = nfa.generateState();
			end = nfa.generateState();
			nfa.addTransition(start, end, reader.next());
			break;
		}
	}

	StreamReader<E> reader;
	Automata& nfa;
};

#endif