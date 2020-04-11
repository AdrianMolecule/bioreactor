#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "interface.h"


class ST7735 final : public Display
{
public:
	void init() override;
	void print(const String& text) override;

private:
	void backLight(bool state);
	void setupExpander();
};

#endif
