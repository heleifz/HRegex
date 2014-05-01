#include "include/parser.h"
#include "include/simplifier.h"
#include <iostream>

int main()
{
	Automata a;
	Parser<UTF8>("八+百万", a);
	a = Simplifier::NFAToDFA(a);
	a = Simplifier::MinimizeDFA(a);
	std::cout << a.simulate<UTF8>("八百万", 3) << "\n";
	std::cout << a.simulate<UTF8>("百万", 2) << "\n";
	std::cout << a.simulate<UTF8>("八八八八百万", 6) << "\n";

}