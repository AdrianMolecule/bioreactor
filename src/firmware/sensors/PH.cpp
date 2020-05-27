/*
 * file PH.cpp * @ https://github.com/GreenPonik/PH_BY_GREENPONIK
 *
 * Arduino library for Gravity: Analog pH Sensor / Meter Kit V2, SKU: SEN0161-V2
 *
 * Based on the @ https://github.com/DFRobot/DFRobot_PH
 * Copyright   [DFRobot](http://www.dfrobot.com), 2018
 * Copyright   GNU Lesser General Public License
 *
 * ##################################################
 * ##################################################
 * ########## Fork on github by GreenPonik ##########
 * ############# ONLY ESP COMPATIBLE ################
 * ##################################################
 * ##################################################
 *
 * version  V1.0
 * date  2019-05
 */

#include "PH.h"
#include "EEPROM.h"

#define ESPADC 4096.0   //the esp Analog Digital Convertion value
#define ESPVOLTAGE 3300 //the esp voltage supply value
#define PH_3_VOLTAGE 2010

using namespace sensor;

PH::PH(adc1_channel_t ph_pin)
{
    _temperature = 25.0;
    _phValue = 7.0;
    _acidVoltage = 3000; //2032.44;   //buffer solution 4.0 at 25C
    _neutralVoltage = 2105; //2350; //1500.0; //buffer solution 7.0 at 25C
    _voltage = 1500.0;
    _ph_pin = ph_pin;
}

PH::~PH()
{
}

void PH::begin()
{
    //check if calibration values (neutral and acid) are stored in eeprom
    _neutralVoltage = EEPROM.readFloat(PHVALUEADDR); //load the neutral (pH = 7.0)voltage of the pH board from the EEPROM
    if (_neutralVoltage == float() || isnan(_neutralVoltage))
    {
        _neutralVoltage = 1500.0; // new EEPROM, write typical voltage
        EEPROM.writeFloat(PHVALUEADDR, _neutralVoltage);
        EEPROM.commit();
    }

    _acidVoltage = EEPROM.readFloat(PHVALUEADDR + sizeof(float)); //load the acid (pH = 4.0) voltage of the pH board from the EEPROM
    if (_acidVoltage == float() || isnan(_acidVoltage))
    {
        _acidVoltage = 2032.44; // new EEPROM, write typical voltage
        EEPROM.writeFloat(PHVALUEADDR + sizeof(float), _acidVoltage);
        EEPROM.commit();
    }
}

float PH::readPH()
{
	_voltage = adc1_get_raw(_ph_pin) / ESPADC * ESPVOLTAGE; // read the voltage

    //Serial.print("_neutraVoltage:");
    //Serial.print(_neutralVoltage);
    //Serial.print(", _acidVoltage:");
    //Serial.print(_acidVoltage);
    float slope = (7.0 - 4.0) / ((_neutralVoltage - 1500.0) / 3.0 - (_acidVoltage - 1500.0) / 3.0); // two point: (_neutralVoltage,7.0),(_acidVoltage,4.0)
    float intercept = 7.0 - slope * (_neutralVoltage - 1500.0) / 3.0;
    //Serial.print(", slope:");
    //Serial.print(slope);
    //Serial.print(", intercept:");
    //Serial.println(intercept);
    _phValue = slope * (_voltage - 1500.0) / 3.0 + intercept; //y = k*x + b
    //Serial.print("[readPH]... phValue ");
    //Serial.println(_phValue);
    return _phValue;
}

void PH::calibration(float voltage, float temperature, char *cmd)
{
    _voltage = voltage;
    _temperature = temperature;
    strupr(cmd);
    phCalibration(cmdParse(cmd)); // if received Serial CMD from the serial monitor, enter into the calibration mode
}

void PH::calibration(float voltage, float temperature)
{
    _voltage = voltage;
    _temperature = temperature;
    if (cmdSerialDataAvailable() > 0)
    {
        phCalibration(cmdParse()); // if received Serial CMD from the serial monitor, enter into the calibration mode
    }
}

boolean PH::cmdSerialDataAvailable()
{
    char cmdReceivedChar;
    static unsigned long cmdReceivedTimeOut = millis();
    while (Serial.available() > 0)
    {
        if (millis() - cmdReceivedTimeOut > 500U)
        {
            _cmdReceivedBufferIndex = 0;
            memset(_cmdReceivedBuffer, 0, (ReceivedBufferLength));
        }
        cmdReceivedTimeOut = millis();
        cmdReceivedChar = Serial.read();
        if (cmdReceivedChar == '\n' || _cmdReceivedBufferIndex == ReceivedBufferLength - 1)
        {
            _cmdReceivedBufferIndex = 0;
            strupr(_cmdReceivedBuffer);
            return true;
        }
        else
        {
            _cmdReceivedBuffer[_cmdReceivedBufferIndex] = cmdReceivedChar;
            _cmdReceivedBufferIndex++;
        }
    }
    return false;
}

byte PH::cmdParse(const char *cmd)
{
    byte modeIndex = 0;
    if (strstr(cmd, "ENTERPH") != NULL)
    {
        modeIndex = 1;
    }
    else if (strstr(cmd, "EXITPH") != NULL)
    {
        modeIndex = 3;
    }
    else if (strstr(cmd, "CALPH") != NULL)
    {
        modeIndex = 2;
    }
    return modeIndex;
}

byte PH::cmdParse()
{
    byte modeIndex = 0;
    if (strstr(_cmdReceivedBuffer, "ENTERPH") != NULL)
    {
        modeIndex = 1;
    }
    else if (strstr(_cmdReceivedBuffer, "EXITPH") != NULL)
    {
        modeIndex = 3;
    }
    else if (strstr(_cmdReceivedBuffer, "CALPH") != NULL)
    {
        modeIndex = 2;
    }
    return modeIndex;
}

void PH::phCalibration(byte mode)
{
    static boolean phCalibrationFinish = 0;
    static boolean enterCalibrationFlag = 0;
    switch (mode)
    {
    case 0:
        if (enterCalibrationFlag)
        {
            Serial.println(F(">>>Command Error<<<"));
        }
        break;

    case 1:
        enterCalibrationFlag = 1;
        phCalibrationFinish = 0;
        Serial.println();
        Serial.println(F(">>>Enter PH Calibration Mode<<<"));
        Serial.println(F(">>>Please put the probe into the 4.0 or 7.0 standard buffer solution<<<"));
        Serial.println();
        break;

    case 2:
        if (enterCalibrationFlag)
        {
            if ((_voltage > PH_8_VOLTAGE) && (_voltage < PH_6_VOLTAGE))
            { // buffer solution:7.0
                Serial.println();
                Serial.print(F(">>>Buffer Solution:7.0"));
                _neutralVoltage = _voltage;
                Serial.println(F(",Send EXITPH to Save and Exit<<<"));
                Serial.println();
                phCalibrationFinish = 1;
            }
            else if ((_voltage > PH_5_VOLTAGE) && (_voltage < PH_3_VOLTAGE))
            { //buffer solution:4.0
                Serial.println();
                Serial.print(F(">>>Buffer Solution:4.0"));
                _acidVoltage = _voltage;
                Serial.println(F(",Send EXITPH to Save and Exit<<<"));
                Serial.println();
                phCalibrationFinish = 1;
            }
            else
            {
                Serial.println();
                Serial.print(F(">>>Buffer Solution Error Try Again<<<"));
                Serial.println(); // not buffer solution or faulty operation
                phCalibrationFinish = 0;
            }
        }
        break;

    case 3: //store calibration value in eeprom
        if (enterCalibrationFlag)
        {
            Serial.println();
            if (phCalibrationFinish)
            {
                if ((_voltage > PH_8_VOLTAGE) && (_voltage < PH_5_VOLTAGE))
                {
                    EEPROM.writeFloat(PHVALUEADDR, _neutralVoltage);
                    EEPROM.commit();
                }
                else if ((_voltage > PH_5_VOLTAGE) && (_voltage < PH_3_VOLTAGE))
                {
                    EEPROM.writeFloat(PHVALUEADDR + sizeof(float), _acidVoltage);
                    EEPROM.commit();
                }
                Serial.print(F(">>>Calibration Successful"));
            }
            else
            {
                Serial.print(F(">>>Calibration Failed"));
            }
            Serial.println(F(",Exit PH Calibration Mode<<<"));
            Serial.println();
            phCalibrationFinish = 0;
            enterCalibrationFlag = 0;
        }
        break;
    }
}
