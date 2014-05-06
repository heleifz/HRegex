/*

	试验新功能

 */

#include "cute/cute.h"
#include "encoding.h"
#include "globals.h"

template <EncodeType E>
void shuntingYard(StreamReader<E>& reader)
{
	std::stack<UnicodeChar> ops;
	while (reader.peek() != 0)
	{
		UnicodeChar ch = reader.next();
		switch (ch)
		{
		case '|':
			while (!ops.empty() && (ops.top() == '*' || ops.top() == 256))
			{
				std::cout << ops.top() << std::endl;
				ops.pop();
			}
			ops.push(ch);
			break;
		case '*':
			ops.push(ch);
			if (reader.peek() != ')' && reader.peek() != '(' &&
				reader.peek() != '*' && reader.peek() != '|')
			{
				while (!ops.empty() && (ops.top() == '*' || ops.top() == 256))
				{
					std::cout << ops.top() << std::endl;
					ops.pop();
				}
				ops.push(256);
			}
			break;
		case '(':
			ops.push(ch);
			break;
		case ')':
			while (!ops.empty() && ops.top() != '(')
			{
				std::cout << ops.top() << std::endl;
				ops.pop();
			}
			// pop the left parenthis
			ops.pop();
			if (reader.peek() != ')' && reader.peek() != '(' &&
				reader.peek() != '*' && reader.peek() != '|')
			{
				while (!ops.empty() && (ops.top() == '*' || ops.top() == 256))
				{
					std::cout << ops.top() << std::endl;
					ops.pop();
				}
				ops.push(256);
			}
			break;
		default:
			std::cout << (char)ch << std::endl;
			
			if (reader.peek() != ')' && reader.peek() != '(' &&
				reader.peek() != '*' && reader.peek() != '|')
			{
				while (!ops.empty() && (ops.top() == '*' || ops.top() == 256))
				{
					std::cout << ops.top() << std::endl;
					ops.pop();
				}
				ops.push(256);
			}
			break;
		}
	}
	while (!ops.empty())
	{
		std::cout << ops.top() << std::endl;
		ops.pop();
	}
}

int main()
{
	StreamReader<ASCII> r("(h|el)|w*l|l*o");
	shuntingYard(r);
	system("pause");
	return 0;
}
