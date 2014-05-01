#include "testAutomata.h"
#include "testParser.h"
#include "testContainers.h"
#include "testSimplifier.h"
#include "testEncoding.h"

int main()
{
	encodingSuit();
	containersSuit();
	automataSuit();
	parserSuit();
	simplifierSuit();

	Automata a;
	Parser<ASCII>("ss(s(ss?)?)?", a);
	a = Simplifier::NFAToDFA(a);
	a = Simplifier::MinimizeDFA(a);
	std::cout << a.simulate<ASCII>("s", 1) << "\n";
	std::cout << a.simulate<ASCII>("ss", 2) << "\n";
	std::cout << a.simulate<ASCII>("sss", 3) << "\n";
	std::cout << a.simulate<ASCII>("ssss", 4) << "\n";
	std::cout << a.simulate<ASCII>("sssss", 5) << "\n";
	std::cout << a.simulate<ASCII>("ssssss", 6) << "\n";
	std::cout << a.simulate<ASCII>("sssssss", 7) << "\n";

	system("pause");
}