#include "testAutomata.h"
#include "testParser.h"
#include "testContainers.h"
#include "testSimplifier.h"

int main()
{
	containersSuit();
	automataSuit();
	parserSuit();
	simplifierSuit();
	system("pause");
}