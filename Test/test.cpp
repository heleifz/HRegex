#include "testNFA.h"
#include "testParser.h"
#include "testContainers.h"
#include "testDFA.h"

int main()
{
	containersSuit();
	nfaSuit();
	parserSuit();
	dfaSuit();
	system("pause");
}