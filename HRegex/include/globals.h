#ifndef _HREG_GLOBALS_
#define _HREG_GLOBALS_

#include <vector>
#include <stack>
#include <map>
#include <sstream>
#include <memory>
#include <algorithm>

class IllegalStateError {};
class ParseError {};
class EmptyContainerError {};

typedef unsigned char HRegexByte;

class NotCopyable
{
public:
	NotCopyable() {}
private:
	NotCopyable(const NotCopyable&) {}
	NotCopyable& operator=(const NotCopyable&) {}
};

#endif