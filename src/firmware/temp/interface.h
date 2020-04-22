#ifndef TEMP_INT_H
#define TEMP_INT_H

class TempSensor
{
public:
	virtual ~TempSensor() {};

	virtual void init(unsigned dataPin) = 0;
	virtual float readCelcius() const = 0;
};


#endif
