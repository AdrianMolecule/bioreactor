#ifndef DISPLAY_INT_H
#define DISPLAY_INT_H

#include <WString.h>

class Display
{
public:
	virtual ~Display() {};

	virtual void init() = 0;
	virtual void print(const String& text) = 0;
};


#endif
