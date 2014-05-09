#ifndef _HREG_PARSER_
#define _HREG_PARSER_

#include "automata.h"
#include "encoding.h"

/*	
	** Thompson Construction Algorithm **

	TODO : 
	construct AST explicity
	then rewrite the tree
	then convert the tree to NFA

	{2, 5} = Parser<ASCII>("ss(s(ss?)?)?", a);
	{2, } = ss+
	{0} = empty

	character class : 现在的弄法是错的，考虑到negate操作，整个char class应该是个整体
	如何支持字符类
	如果是查表的话，行和列要如何设计

	// regular expression syntax : 

	re : term { "|" term }
	term : factor { factor }
	factor : primitive (repetition|"*"|"+"|"?")*
	repetition : {digit?,digit?}
	primitive : "\" meta | alpha | (re) | charclass
	meta : "n" "{" "|" "}" "(" ")" "t" "+" "?"
	charclass "[" "^"? (alpha | alpha "-" alpha)+ "]"

	// parse function return start and end state 
	// of the constructed subgraph

	void parseXXXX(Node& start, Node& end)

*/
//
//template <EncodeType E>
//class Parser : public NotCopyable
//{
//public:
//	Parser(typename Encode<E>::PointerType input, Automata& automata)
//		: reader(input), nfa(automata)
//	{
//		nfa.clear();
//		// empty string
//		if (reader.peek() == '\0')
//		{
//			return;
//		}
//		State s;
//		State e;
//		parseRE(s, e);
//		if (reader.peek() != '\0')
//		{
//			throw ParseError();
//		}
//		nfa.setStart(s);
//		nfa.setTerminate(e);
//	}
//private:
//	void parseRE(State& start, State& end)
//	{
//		State s1;
//		State s2;
//		parseTerm(s1, s2);
//		if (reader.peek() == '|')
//		{
//			start = nfa.generateState();
//			end = nfa.generateState();
//			nfa.addTransition(start, s1, Transition::EPSILON);
//			nfa.addTransition(s2, end, Transition::EPSILON);
//			while (reader.peek() == '|')
//			{
//				reader.next();
//				parseTerm(s1, s2);
//				nfa.addTransition(start, s1, Transition::EPSILON);
//				nfa.addTransition(s2, end, Transition::EPSILON);
//			}
//		}
//		else
//		{
//			start = s1;
//			end = s2;
//		}
//	}
//	void parseTerm(State& start, State& end)
//	{
//		parseFactor(start, end);
//		State current = end;
//		auto p = reader.peek();
//		while (p != '\0' && p != '|' && p != ')')
//		{
//			State s1;
//			State s2;
//			parseFactor(s1, s2);
//			nfa.addTransition(current, s1, Transition::EPSILON);
//			current = s2;
//			p = reader.peek();
//		}
//		end = current;
//	}
//	void parseFactor(State& start, State& end)
//	{
//		State s1; State s2;
//		State s3; State s4;
//		parsePrimitive(s1, s2);
//		auto p = reader.peek();
//		while (p == '?' || p == '*' || p == '+' || p == '{')
//		{
//			int minRepetition;
//			int maxRepetition;
//			switch (p)
//			{
//			case '?':
//				nfa.addTransition(s1, s2, Transition::EPSILON);
//				reader.next();
//				break;
//			case '*':
//				s3 = nfa.generateState();
//				s4 = nfa.generateState();
//				nfa.addTransition(s3, s1, Transition::EPSILON);
//				nfa.addTransition(s3, s4, Transition::EPSILON);
//				nfa.addTransition(s2, s4, Transition::EPSILON);
//				nfa.addTransition(s2, s1, Transition::EPSILON);
//				s1 = s3;
//				s2 = s4;
//				reader.next();
//				break;
//			case '+':
//				s4 = nfa.generateState();
//				nfa.addTransition(s2, s4, Transition::EPSILON);
//				nfa.addTransition(s4, s1, Transition::EPSILON);
//				s2 = s4;
//				reader.next();
//				break;
//			case '{':
//				parseRepetition(minRepetition, maxRepetition);
//				// construct equivalent nfa
//				// special cases : {count}
//				if (minRepetition == maxRepetition && minRepetition > 0)
//				{
//
//				}
//				// special case : {count,}
//				else if (minRepetition > 0 && maxRepetition == -1)
//				{
//
//				}
//				// special case : {,}
//				else if (minRepetition == 0 && maxRepetition == -1)
//				{
//
//				}
//				// normal case : {count1, count2}
//				else if (minRepetition < maxRepetition && minRepetition >= 0)
//				{
//
//				}
//				else
//				{
//					throw ParseError();
//				}
//				break;
//			}
//			p = reader.peek();
//		}
//		start = s1;
//		end = s2;
//	}
//	// assume that the reader is at {...}
//	//                              ^
//	void parseRepetition(int& minNum, int& maxNum)
//	{
//		if (reader.peek() != '{')
//		{
//			throw ParseError();
//		}
//		reader.next();
//		minNum = 0;
//		// parse minNum
//		while (reader.peek() != ',' && reader.peek() != '}')
//		{
//			if (reader.peek() >= '0' && reader.peek() <= '9')
//			{
//				minNum = minNum * 10 + (reader.next() - '0');
//			}
//			else
//			{
//				throw ParseError();
//			}
//		}
//		if (reader.peek() == '}')
//		{
//			reader.next();
//			maxNum = minNum;
//			return;
//		}
//		// consume the comma
//		reader.next();
//		// parse maxNum
//		if (reader.peek() == '}')
//		{
//			maxNum = -1;
//			reader.next();
//			return;
//		}
//		maxNum = 0;
//		while (reader.peek() != '}')
//		{
//			if (reader.peek() >= '0' && reader.peek() <= '9')
//			{
//				maxNum = maxNum * 10 + (reader.next() - '0');
//			}
//			else
//			{
//				throw ParseError();
//			}
//		}
//		// consome the right curly braces
//		reader.next();
//		return;
//	}
//	void parsePrimitive(State& start, State& end)
//	{
//		RangeSet s;
//		// lookahead
//		switch (reader.peek())
//		{
//		case '\\':
//			reader.next();
//			start = nfa.generateState();
//			end = nfa.generateState();
//			switch (reader.peek())
//			{
//			case 'n':
//				nfa.addTransition(start, end, '\n');
//				break;
//			case 't':
//				nfa.addTransition(start, end, '\t');
//				break;
//			case 'd':
//				s.insert({ '0', '9' });
//				nfa.addTransition(start, end, Transition(s));
//				break;
//			case '{': case '}': case '|':
//			case '(': case ')': case '.':
//			case '+': case '*': case '?':
//			case '\\':
//				nfa.addTransition(start, end, reader.peek());
//				break;
//			default:
//				throw ParseError();
//			}
//			reader.next();
//			break;
//		case '(':
//			reader.next();
//			parseRE(start, end);
//			if (reader.peek() != ')')
//			{
//				throw ParseError();
//			}
//			reader.next();
//			break;
//		case '\0': case '*': case '|':
//			throw ParseError();
//			break;
//		case '.':
//			start = nfa.generateState();
//			end = nfa.generateState();
//			nfa.addTransition(start, end, Transition::WILDCARD);
//			reader.next();
//			break;
//		default:
//			start = nfa.generateState();
//			end = nfa.generateState();
//			nfa.addTransition(start, end, reader.next());
//			break;
//		}
//	}
//
//	StreamReader<E> reader;
//	Automata& nfa;
//};


// 只在生成NFA时候需要walk，所以没有用visitor pattern

class ExpressionNode
{
public:
	virtual void convertToNFA(Automata& nfa, State& s, State& e) const = 0;
};
typedef std::shared_ptr<ExpressionNode> NodePtr;

///////////
class CharNode : public ExpressionNode
{
public:
	CharNode(UnicodeChar c)
		: ch(c)
	{}
	void convertToNFA(Automata& nfa, State& s, State& e) const
	{
		s = nfa.generateState();
		e = nfa.generateState();
		nfa.addTransition(s, e, ch);
	}
private:
	UnicodeChar ch;
};

class CharsetNode : public ExpressionNode
{
public:
	CharsetNode(const RangeSet& st)
		: rangeSet(st)
	{
	}
	void convertToNFA(Automata& nfa, State& s, State& e) const
	{
		s = nfa.generateState();
		e = nfa.generateState();
		nfa.addTransition(s, e, rangeSet);
	}
private:
	RangeSet rangeSet;
};

class WildcardNode : public ExpressionNode
{
public:
	void convertToNFA(Automata& nfa, State& s, State& e) const
	{
		s = nfa.generateState();
		e = nfa.generateState();
		nfa.addTransition(s, e, Transition::WILDCARD);
	}
};
//////////////
class KleenNode : public ExpressionNode
{
public:
	KleenNode(NodePtr n)
		: child(n)
	{
	}
	void convertToNFA(Automata& nfa, State& s, State& e) const
	{
		State childS;
		State childE;
		child->convertToNFA(nfa, childS, childE);
		s = nfa.generateState();
		e = nfa.generateState();
		nfa.addTransition(s, childS, Transition::EPSILON);
		nfa.addTransition(childE, e, Transition::EPSILON);
		nfa.addTransition(s, e, Transition::EPSILON);
		nfa.addTransition(childE, childS, Transition::EPSILON);
	}
private:
	NodePtr child;
};

class OptionalNode : public ExpressionNode
{
public:
	OptionalNode(NodePtr n)
		: child(n)
	{
	}
	void convertToNFA(Automata& nfa, State& s, State& e) const
	{
		child->convertToNFA(nfa, s, e);
		nfa.addTransition(s, e, Transition::EPSILON);
	}
private:
	NodePtr child;
};

class OneOrMoreNode : public ExpressionNode
{
public:
	OneOrMoreNode(NodePtr n)
		: child(n)
	{
	}
	void convertToNFA(Automata& nfa, State& s, State& e) const
	{
		e = nfa.generateState();
		State childE;
		child->convertToNFA(nfa, s, childE);
		nfa.addTransition(childE, e, Transition::EPSILON);
		nfa.addTransition(e, s, Transition::EPSILON);
	}
private:
	NodePtr child;
};


class RepetitionNode : public ExpressionNode
{
public:
	RepetitionNode(NodePtr n, int minCnt, int maxCnt)
		: child(n), minCount(minCnt), maxCount(maxCnt)
	{
	}
	void convertToNFA(Automata& nfa, State& s, State& e) const
	{
		// special case : {count,} {,}
		if (maxCount == -1)
		{
			State tmpS;
			State tmpE;
			bool result = chainConcatenation(nfa, minCount, false, tmpS, tmpE);
			State kleenS;
			State kleenE;
			KleenNode(child).convertToNFA(nfa, kleenS, kleenE);
			if (result)
			{
				s = tmpS;
				nfa.addTransition(tmpE, kleenS, Transition::EPSILON);
				e = kleenE;
			}
			else
			{
				s = kleenS;
				e = kleenE;
			}
		}
		// other case : {count}, {count1, count2}, {,count2}
		else if (minCount <= maxCount && minCount >= 0)
		{
			State firstS; State firstE;
			State secondS; State secondE;
			bool resultFirst = chainConcatenation(nfa, minCount, false, firstS, firstE);
			bool resultSecond = chainConcatenation(nfa, maxCount - minCount, true, secondS, secondE);
			if (!resultFirst && !resultSecond)
			{
				s = nfa.generateState();
				e = nfa.generateState();
				nfa.addTransition(s, e, Transition::EPSILON);
				return;
			}
			if (resultFirst)
			{
				s = firstS;
			}
			else
			{
				s = secondS;
			}
			if (resultSecond)
			{
				e = secondE;
			}
			else
			{
				e = firstE;
			}
			if (resultFirst && resultSecond)
			{
				nfa.addTransition(firstE, secondS, Transition::EPSILON);
			}
		}
		else
		{
			throw ParseError();
		}
	}
private:
	bool chainConcatenation(Automata& nfa, int count, bool addEps, State& s, State& e) const
	{
		if (count == 0)
		{
			return false;
		}
		s = nfa.generateState();
		e = nfa.generateState();
		State current = s;
		for (int i = 0; i < count; ++i)
		{
			State childS;
			State childE;
			child->convertToNFA(nfa, childS, childE);
			nfa.addTransition(current, childS, Transition::EPSILON);
			if (addEps)
			{
				nfa.addTransition(current, e, Transition::EPSILON);
			}
			current = childE;
		}
		nfa.addTransition(current, e, Transition::EPSILON);
		return true;
	}
	NodePtr child;
	int minCount;
	int maxCount;
};


class AlternateNode : public ExpressionNode
{
public:
	AlternateNode(NodePtr l, NodePtr r)
		: left(l), right(r)
	{
	}
	void convertToNFA(Automata& nfa, State& s, State& e) const
	{
		State leftS; State leftE;
		State rightS; State rightE;
		left->convertToNFA(nfa, leftS, leftE);
		right->convertToNFA(nfa, rightS, rightE);
		s = nfa.generateState();
		e = nfa.generateState();
		nfa.addTransition(s, leftS, Transition::EPSILON);
		nfa.addTransition(s, rightS, Transition::EPSILON);
		nfa.addTransition(leftE, e, Transition::EPSILON);
		nfa.addTransition(rightE, e, Transition::EPSILON);
	}
private:
	NodePtr left;
	NodePtr right;
};

class ConcatenateNode : public ExpressionNode
{
public:
	void addSibling(NodePtr sibling)
	{
		siblings.push_back(sibling);
	}
	void convertToNFA(Automata& nfa, State& s, State& e) const
	{
		s = nfa.generateState();
		e = nfa.generateState();
		State current = s;
		for (auto i = siblings.begin(); i != siblings.end(); ++i)
		{
			State siblingS;
			State siblingE;
			(*i)->convertToNFA(nfa, siblingS, siblingE);
			nfa.addTransition(current, siblingS, Transition::EPSILON);
			current = siblingE;
		}
		nfa.addTransition(current, e, Transition::EPSILON);
	}
private:
	std::vector<NodePtr> siblings;
};

template <EncodeType E>
class Parser
{
public:
	Parser(typename Encode<E>::PointerType input, Automata& nfa)
		: reader(input)
	{
		nfa.clear();
		if (reader.peek() == 0)
		{
			return;
		}
		auto ast = parseRE();
		if (reader.peek() != 0)
		{
			throw ParseError();
		}
		State s;
		State e;
		ast->convertToNFA(nfa, s, e);
		nfa.setStart(s);
		nfa.setTerminate(e);
	}
private:
	NodePtr parseRE()
	{
		NodePtr ret = parseTerm();
		if (reader.peek() == '|')
		{
			while (reader.peek() == '|')
			{
				reader.next();
				ret = std::make_shared<AlternateNode>(ret, parseTerm());
			}
		}
		return ret;
	}

	NodePtr parseTerm()
	{
		auto ret = std::make_shared<ConcatenateNode>();
		ret->addSibling(parseFactor());
		auto p = reader.peek();
		while (p != '\0' && p != '|' && p != ')')
		{
			ret->addSibling(parseFactor());
			p = reader.peek();
		}
		return ret;
	}

	NodePtr parseFactor()
	{
		NodePtr ret = parsePrimitive();
		auto p = reader.peek();
		while (p == '?' || p == '*' || p == '+' || p == '{')
		{
			int minRepetition;
			int maxRepetition;
			switch (p)
			{
			case '?':
				ret = std::make_shared<OptionalNode>(ret);
				reader.next();
				break;
			case '*':
				ret = std::make_shared<KleenNode>(ret);
				reader.next();
				break;
			case '+':
				ret = std::make_shared<OneOrMoreNode>(ret);
				reader.next();
				break;
			case '{':
				// !NOTE : max must greater than zero, min must less or equal to max
				parseRepetition(minRepetition, maxRepetition);
				ret = std::make_shared<RepetitionNode>(ret, minRepetition, maxRepetition);

				break;
			}
			p = reader.peek();
		}
		return ret;
	}

	NodePtr parsePrimitive()
	{
		NodePtr p = nullptr;
		RangeSet st;
		// lookahead
		switch (reader.peek())
		{
		case '\\':
			reader.next();
			switch (reader.peek())
			{
			case 'd':
				st.insert({ '0', '9' });
				reader.next();
				return std::make_shared<CharsetNode>(st);
				break;
			case '{': case '}': case '|':
			case '(': case ')': case '.':
			case '+': case '*': case '?':
			case '\\': case 'n': case 't':
				return std::make_shared<CharNode>(reader.next());
				break;
			default:
				throw ParseError();
			}
			reader.next();
			break;
		case '(':
			reader.next();
			p = parseRE();
			if (reader.peek() != ')')
			{
				throw ParseError();
			}
			reader.next();
			return p;
		case '\0': case '*': case '|':
			throw ParseError();
			break;
		case '.':
			reader.next();
			return std::make_shared<WildcardNode>();
			break;
		default:
			return std::make_shared<CharNode>(reader.next());
		}
	}

	// assume that the reader is at {...}
	//                              ^
	void parseRepetition(int& minNum, int& maxNum)
	{
		if (reader.peek() != '{')
		{
			throw ParseError();
		}
		reader.next();
		minNum = 0;
		// parse minNum
		while (reader.peek() != ',' && reader.peek() != '}')
		{
			if (reader.peek() >= '0' && reader.peek() <= '9')
			{
				minNum = minNum * 10 + (reader.next() - '0');
			}
			else
			{
				throw ParseError();
			}
		}
		if (reader.peek() == '}')
		{
			reader.next();
			maxNum = minNum;
			return;
		}
		// consume the comma
		reader.next();
		// parse maxNum
		if (reader.peek() == '}')
		{
			maxNum = -1;
			reader.next();
			return;
		}
		maxNum = 0;
		while (reader.peek() != '}')
		{
			if (reader.peek() >= '0' && reader.peek() <= '9')
			{
				maxNum = maxNum * 10 + (reader.next() - '0');
			}
			else
			{
				throw ParseError();
			}
		}
		// consome the right curly braces
		reader.next();
		return;
	}
	StreamReader<E> reader;
};


#endif