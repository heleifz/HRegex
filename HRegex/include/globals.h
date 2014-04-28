#ifndef _HREG_GLOBALS_
#define _HREG_GLOBALS_

#include <cstring>
#include <cstdlib>

#include <vector>
#include <stack>
#include <map>
#include <string>
#include <memory>
#include <algorithm>

class IllegalStateError {};
class ParseError {};

class NotCopyable
{
public:
	NotCopyable() {}
private:
	NotCopyable(const NotCopyable&) {}
	NotCopyable& operator=(const NotCopyable&) {}
};

#endif