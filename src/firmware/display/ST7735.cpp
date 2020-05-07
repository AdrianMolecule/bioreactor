#include "ST7735.h"

#include <Adafruit_GFX.h>

#include <Wire.h>
#include <jm_PCF8574.h>

jm_PCF8574 expanderTwo(0x20);
#define lcdLed    4

void ST7735::backLight (bool state)
{

	uint16_t _pinModeMask = 0xffff;
	uint8_t _val = !state & 1;
	  if(_val) _pinModeMask |= _val << lcdLed;
	  else _pinModeMask &= ~(1 << lcdLed);

	  _pinModeMask = (_pinModeMask & 0xff00) | _pinModeMask;


	  expanderTwo.write(_pinModeMask);
}

void ST7735::setupExpander()
{
	  Wire.begin(21, 22);
	  Wire.setClock(100000L);

	  expanderTwo.begin();
	  expanderTwo.write(0xffff);

	  pinMode(32, INPUT_PULLUP);
	  expanderTwo.read();
}

void ST7735::init()
{
	tft.reset(new Adafruit_ST7735(-1, 12, 13, 14, -1));	//cs dc mosi sck rst
	setupExpander();

	tft->initR(INITR_BLACKTAB);
	backLight (true);
	tft->initR(INITR_BLACKTAB);
	tft->setRotation(3);

	tft->fillScreen(ST77XX_BLACK);
}

void ST7735::print(const String& text)
{
	tft->fillScreen(ST77XX_BLACK);
	tft->setTextWrap(true);

	tft->setTextSize(2);
	tft->setCursor(10, 10);
	tft->setTextColor(ST77XX_WHITE);
	tft->print(text);

}
