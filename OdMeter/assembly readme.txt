The cuvette end has to be cut
Two silicone corks have to be poured

Adjust the current:
	Adjust the voltage pot to maybe -5 volts rom the pot near Input
	Short the output on a 10 AMp scale and adjust the current pot (near output) to about 200 mA until the max light with water cuette about 15-20000

Cut the long cable with 6 wires
Solder the cable to sensor and LED. Solder the other ends to the 2 pin connector and a 4 pin connector for the sensor. 
Wrap the cable around the 3D printed outer case. Add heat shrink parts.
With 2 more wires Solder the const current output to the connector. With 2 extra red black wire solder from constant current input board to another 2 pin connector DONE that will go to the main board VCC. Add heat shrink parts.
So the constant current board needs to be with 2 connectors coming out
mount the costant current using BOM 2x 9 mm 2 mm diam screws





	print the cell flow inner part in black
	print the cell low ODMeter outer housing
	push the sensor and LED inthe slots on the inner part making sure the sensor is aligned with the LED. Sometimes you eed to sand the edges of the sensor board to fit perfectly in the plastic slot. Wipe the actual sensor in case it has fingerprints or or got dirty in the handling
Connect the redBlue LED connector to vcc and Ground on the main board
Connect the I2c connector to I2c on the main board

test with TestOdMeter from VSC. Q is the TestODMeter in GIT?
make sure lank gives at least 13 000 lux measurement value
