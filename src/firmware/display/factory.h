#ifndef DISPLAY_FACTORY_H
#define DISPLAY_FACTORY_H

#include <memory>
#include "interface.h"

enum class Displays
{
	ST7735
};

std::unique_ptr<Display> CreateDisplay(Displays instance);

#endif
