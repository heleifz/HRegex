/************************************************************************/
/*  Test DFA
/************************************************************************/

#include "dfa.h"
#include "cute/cute.h"

// Test suits

void dfaSuit()
{
	cute::suite s;
	cute::runner<cute::ostream_listener>()(s, "DFA Test");
}