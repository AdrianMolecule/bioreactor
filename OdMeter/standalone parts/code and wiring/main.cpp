// Reference implementation for Optical Density Meter using an Arduino nano
//Wiring info at the end of this file
// works for arduino nano
// work-dir index head
// main code for the Optical density meter
// current project hosted in Visual Source Code
#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <BH1750.h>  //NEEDS TO BE AFTER WIRE
#include <main.h>  

int BH17_add = 0x23;  // I2C address of BH1750 https://www.mouser.com/datasheet/2/348/bh1750fvi-e-186247.pdf  Small measurement variation (+/- 20%)
uint32_t buffer = 0;
boolean oldCalibration = true;
const uint32_t lowLightTreshold = 20;
unsigned long warmupTime = 15 * 60000;  // in milliseconds for some reasons it does not like 15*60*1000
// commands
int arg = 0;

const boolean DEBUG_MODE=true;
const int BUFER_WRITING_EEPROM_ADDRESS = 0;
// see electronics at the end
/* Usage Instructions
 cover the instrument to measure less than 20 lux does a reset
 wait from prompt
 insert cuvette with buffer to read blank value and wait for next prompt
 insert sample.
 You can run it without a display using a serial cable and a terminal program like coolTerm.
 You can even type commands terminated by new line like b and a new value for the buffer value.
 */

BH1750 lightSensor(0x23);

void setup() {
    Serial.begin(115200);
    Serial.print("In setup ");
    Wire.begin();
    if (lightSensor.begin(BH1750::CONTINUOUS_LOW_RES_MODE)) {
        Serial.println(F("BH1750 Advanced begin"));
    } else {
        Serial.println(F("Error initialising BH1750"));
    }
    Serial.println("Finished waking up");
    EEPROM.get(BUFER_WRITING_EEPROM_ADDRESS, buffer);
    delay(500);
    Serial.println("Finished setup");
}
//
void loop() {
    uint32_t sampleLux = readLight(100);
    if(DEBUG_MODE) {
      Serial.print("lux:");
      Serial.println(sampleLux);
      return;
    }
    if (sampleLux < lowLightTreshold) {
        buffer = 0;
        calibrate(sampleLux);
    }
    Serial.print("OD:");
    Serial.print(-log10((double)sampleLux / buffer));
    Serial.print("   Sampl:");
    Serial.print(sampleLux);
    if (oldCalibration) {
        Serial.print("  Buffer (old):");
    } else {
        Serial.print("  Buffer:");
    }
    Serial.print(buffer);
    Serial.print("  sample/buffer:");
    Serial.print((double)sampleLux / buffer);
    if (warmupTime > millis()) {
        unsigned long timeLeft = warmupTime - millis();
        Serial.print(" WARMING UP for another:");
        // Serial.print((int) (((double) timeLeft) / 60000.));
        Serial.print((int)(timeLeft / 60000.));
        Serial.print(" minutes, ");
        Serial.print((int)(((timeLeft) % 60000) / 1000));
        Serial.println(" seconds");
    }
    Serial.println(" after:" + getFormatedTimeSinceStart());
    checkForCommand();
    delay(250);
}
//
void checkForCommand() {
    if (Serial.available()) {                           // if there is data coming
        String command = Serial.readStringUntil('\n');  // read string until newline character
        Serial.println(command);
        int index = command.indexOf(" ");
        if (index > 0) {
            arg = (command.substring(index + 1)).toInt();
            //			Serial.print("arg:");
            //			Serial.println(arg);
            command = (command.substring(0, index));
            //			Serial.print("short command:");
            //			Serial.println(command);
        }
        int str_len = command.length() + 1;
        char tok_array[str_len];
        command.toCharArray(tok_array, str_len);
        executeCommand(tok_array);
    }
}
//
uint32_t readLight(int times) {
    uint32_t l = 0;
    uint32_t sampleLux = 0;
    //Serial.println("readLight Called: ");
    for (int i = 0; i < times; i++) {
        while (!lightSensor.measurementReady())
            ;
        l += lightSensor.readLightLevel();
        //Serial.println(lightSensor.readLightLevel());
    }
    sampleLux = l / (float)times;
    //Serial.println("readLight Finished after reading: ");
    //Serial.println(sampleLux);
    return sampleLux;
}
//
void calibrate(uint32_t sampleLux) {
    Serial.print("\nCalibration Started after reading only: ");
    Serial.print(sampleLux);
    Serial.println(" lux");
    uint32_t tempLux;
    do {
        tempLux = readLight(30);
    } while (tempLux < lowLightTreshold);
    Serial.println("Light Detected... You have ONLY 5 seconds so Insert Buffer Now");
    delay(5000);  // wait to make sure buffer tube was fully inserted
    Serial.println("Reading Buffer Value\n");
    do {
        tempLux = readLight(10000);
    } while (tempLux <= 0);
    delay(100);
    if (tempLux != buffer) {
        EEPROM.put(BUFER_WRITING_EEPROM_ADDRESS, tempLux);
        buffer = tempLux;  // old buffer was12256
    }
    oldCalibration = false;
    Serial.print("Done Reading Buffer. Buffer: ");  // prepare text
    Serial.println(buffer);
    uint32_t readValue;
    EEPROM.get(BUFER_WRITING_EEPROM_ADDRESS, readValue);
    Serial.print("Buffer value saved in EEPROM is:");
    Serial.println(readValue);
    Serial.println("Calibration Done... Insert Sample\n");  // prepare text
    delay(1000);
}
//
const int DEFAULT_BRIDGE_FREQUENCY_OF_CHANGING_PWM = 1000;
const int RESOLUTION = 10;  // Resolution 8 does not seem to work, 10, 12, 15

const int MAX_DUTY_CYCLE = (int)(pow(2, RESOLUTION) - 1);
const uint8_t PWM_CHANNEL = 0;
int power = 5;  // 5 percent
int calculatedDuty = (int)power * MAX_DUTY_CYCLE / 100;

String getFormatedTimeSinceStart() {
    unsigned long time = (unsigned long)((millis() / 1000));  // finds the time since last print in secs
    String result = "";
    int hours = (unsigned long)(time / 3600);
    if (hours > 0) {
        result = +hours;
        result += ("h ");
    }
    result += ((unsigned long)(time % 3600) / 60);
    result += ("m ");
    result += (time % 60);
    result += ("s");
    return result;
}
//
void setbuffer() {
    Serial.println(" set buffer");
    buffer = arg;
}
//
void ledon() {
    Serial.print(" LED ON ");
    digitalWrite(2, HIGH);  // turn on LED
}
//
void ledoff() {
    Serial.print(" LED OFF ");
    digitalWrite(2, LOW);  // turn off LED
}

//
struct stringcase {
    const char *caseName;
    void (*func)(void);
};
//
stringcase cases[] = {{"b", setbuffer}, {"ledon", ledon}, {"ledoff", ledoff}};

void executeCommand(char *token) {
    Serial.print("execute command ");
    Serial.println(String(token));
    boolean found = false;
    for (stringcase *pCase = cases; pCase != cases + sizeof(cases) / sizeof(cases[0]); pCase++) {
        if (0 == strcasecmp(pCase->caseName, token)) {
            (*pCase->func)();
            found = true;
            break;
        }
    }
    if (!found) {
        Serial.println("command not found!!!");
    }
    delay(1000);
}

/* electronics
 This is the reference implementation
 light sensor BH1750 https://www.mouser.com/datasheet/2/348/bh1750fvi-e-186247.pdf  Small measurement variation (+/- 20%)
 OLED LCD
 light source LED 1 watt orange in series with 5 Ohm resistor (.5 watt) powered from USB
 Ground =Black to BH1750FVI Sensor GND to OLED GND to Nano GND
 Nano SDA= Yellow to Sensor SDA to OLED SDA to Nano SDA/A4
 Nano SCK =Orange to Sensor SCK to OLED SCK to Nano SCK/A5
 First Red to Nano +5 to sensor CC to OLED VDD
 second RED from Nano Vin to 5 Ohm resistor .5 watts or 2 10 ohm in parallel at .25 watts

 Measured voltage on resistor=1.23 volts  and on the LED 2.23 so the current= 1.23v/5 ohm=250 mA and power .25A*2.23=.6 Watts
 should get about 10 000 lux reading with just air

 The sensor has to be pushed downwards in the slot to align with the window.
 */