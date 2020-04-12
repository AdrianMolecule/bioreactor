#include "factory.h"
#include "ST7735.h"

std::unique_ptr<Display> CreateDisplay(Displays type)
{

	switch(type)
	{
		case Displays::ST7735:
			return std::unique_ptr<Display>( new ST7735() );
	}

	return nullptr;
}
