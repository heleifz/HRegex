#include "automata.h"
#include "encoding.h"

struct Range
{
	UnicodeChar lower;
	UnicodeChar upper;
};

class ExpressionNode
{
};

typedef std::shared_ptr<ExpressionNode> NodePtr;

///////////
class CharNode : public ExpressionNode
{
public:
	CharNode(UnicodeChar c)
		: ch(c)
	{
	}
private:
	UnicodeChar ch;
};

class CharsetNode : public ExpressionNode
{
public:
	void addRange(UnicodeChar l, UnicodeChar u)
	{
	}
private:
	std::vector<Range> ranges;
};

class WildcardNode : public ExpressionNode
{
};
//////////////
class KleenNode : public ExpressionNode
{
public:
	KleenNode(NodePtr n)
		: child(n)
	{
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
private:
	NodePtr child;
};

class RepetitionNode : public ExpressionNode
{
public:
	RepetitionNode(int minN, int maxN, NodePtr n)
		: child(n), minNum(minN), maxNum(maxN)
	{
	}
private:
	int minNum;
	int maxNum;
	NodePtr child;
};

class OneOrMoreNode : public ExpressionNode
{
public:
	OneOrMoreNode(NodePtr n)
		: child(n)
	{
	}
private:
	NodePtr child;
};

class AlternateNode : public ExpressionNode
{
public:
	AlternateNode(NodePtr l, NodePtr r)
		: left(l), right(r)
	{
	}
private:
	NodePtr left;
	NodePtr right;
};

class ConcatenateNode : public ExpressionNode
{
public:
	ConcatenateNode(NodePtr l, NodePtr r)
		: left(l), right(r)
	{
	}
private:
	NodePtr left;
	NodePtr right;
};


template <EncodeType E>
class Parser
{
public:
	Parser(typename Encode<E>::PointerType input, Automata& nfa)
		: reader(input)
	{
		nfa.clear();
		parseRE();
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
		NodePtr ret = parseFactor();
		auto p = reader.peek();
		while (p != '\0' && p != '|' && p != ')')
		{
			ret = std::make_shared<ConcatenateNode>(ret, parseFactor());
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
				// construct equivalent AST
				// special cases : {count}
				if (minRepetition == maxRepetition)
				{
					ret = chainConcatenation(ret, minRepetition);
					if (ret == nullptr)
					{
						throw ParseError();
					}
				}
				// special case : {count,} {,}
				else if (maxRepetition == -1)
				{
					NodePtr n = chainConcatenation(ret, minRepetition);
					ret = std::make_shared<KleenNode>(ret);
					if (n != nullptr)
					{
						ret = std::make_shared<ConcatenateNode>(n, ret);
					}
				}
				// normal case : {count1, count2}
				else if (minRepetition < maxRepetition && minRepetition >= 0)
				{
					// convert to nested optional expression
					// s{2,5} == "ss(s(ss?)?)?
					// concatenation part
					NodePtr l = chainConcatenation(ret, minRepetition);
					int optionalPart = maxRepetition - minRepetition;
					NodePtr r = ret;
					for (int i = 0; i < optionalPart - 1; ++i)
					{
						r = std::make_shared<ConcatenateNode>(ret,
							std::make_shared<OptionalNode>(r));
					}
					r = std::make_shared<OptionalNode>(r);
					if (l == nullptr)
					{
						ret = r;
					}
					else
					{
						ret = std::make_shared<ConcatenateNode>(l, r);
					}
				}
				else
				{
					throw ParseError();
				}
				break;
			}
			p = reader.peek();
		}
		return ret;
	}

	NodePtr parsePrimitive()
	{
		NodePtr p = nullptr;
		// lookahead
		switch (reader.peek())
		{
		case '\\':
			reader.next();
			switch (reader.peek())
			{
			case 'd':
				return std::make_shared<CharsetNode>();
				break;
			case '{': case '}': case '|':
			case '(': case ')': case '.':
			case '+': case '*': case '?':
			case '\\': case 'n': case 't':
				return std::make_shared<CharNode>(reader.peek());
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
	
	NodePtr chainConcatenation(NodePtr n, int count)
	{
		if (count == 0)
		{
			return nullptr;
		}
		else if (count < 0)
		{
			throw ParseError();
		}
		NodePtr ret = n;
		for (int i = 0; i < count - 1; ++i)
		{
			ret = std::make_shared<ConcatenateNode>(ret, n);
		}
		return ret;
	}
	StreamReader<E> reader;
};

int main()
{
	Automata nfa;
	Parser<ASCII>("a+(ab*)*?|c{,5}", nfa);
}
