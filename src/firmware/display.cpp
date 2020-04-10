#include <Adafruit_GFX.h>
#include "Adafruit_ST7735.h"
#include "display.h"

#include <Wire.h>
#include "pcf8574_esp.h"

Adafruit_ST7735 tft = Adafruit_ST7735(-1, 12, 13, 14, -1); //cs dc mosi sck rst

PCF857x expanderTwo(0x20, &Wire);
#define lcdLed    4

void backLight (bool state) {
  expanderTwo.write(lcdLed, !state); // lcd back
}

volatile bool PCFInterruptFlagTwo = false;
void ICACHE_RAM_ATTR PCFInterruptTwo() {
  PCFInterruptFlagTwo = true;
}

void setupExpander()
{
	  Wire.begin(21, 22);
	  Wire.setClock(100000L);

	  expanderTwo.begin();
	  pinMode(32, INPUT_PULLUP);
	  expanderTwo.resetInterruptPin();
	  attachInterrupt(digitalPinToInterrupt(32), PCFInterruptTwo, FALLING);

}

/* Initialize the display. */
void display_init(void) {
	setupExpander();

  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab


  backLight (true);
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(3);
  //delay(500);

   tft.fillScreen(ST77XX_BLACK);
}

/* Quick way to print a line of text. */
void display_text(const char *text, uint8_t x, uint8_t y, uint8_t size) {

	tft.fillScreen(ST77XX_BLACK);
  //  tft.setTextWrap(true);

  tft.setTextSize(size);
  tft.setCursor(x, y);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(text);

}
