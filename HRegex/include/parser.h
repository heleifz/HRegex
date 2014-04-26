#include "nfa.h"

/*	
	** Thompson Construction Algorithm **

	// regular expression syntax : 

	re : term { "|" term }
	term : factor { factor }
	factor : "\" meta | alpha | (re)
	meta : "n" "{" "|" "}" "(" ")" "t"

	// parse function return start and end state 
	// of the constructed subgraph
	void parseXXXX(Node& start, Node& end)

*/

class Parser
{
public:
	Parser(const char *re, NFA& automata);
private:
	void parseRE(State& start, State& end);
	void parseTerm(State& start, State& end);
	void parseFactor(State& start, State& end);

	const char *re;
	NFA& nfa;
};