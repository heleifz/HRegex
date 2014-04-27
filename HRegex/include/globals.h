#ifndef _HREG_GLOBALS_
#define _HREG_GLOBALS_

#include <cstring>

#include <vector>
#include <memory>
#include <set>
#include <string>
#include <algorithm>
#include <stack>

#define EPSTRANS (-1)

typedef char Transition;
typedef size_t State;

class IllegalStateError {};
class ParseError {};

#endif