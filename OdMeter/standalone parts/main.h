#ifndef MAIN_H_
#define MAIN_H_

void executeCommand(char *token);
String getFormatedTimeSinceStart();
void checkForCommand();
void calibrate(uint32_t sampleLux);
uint32_t readLight(int times); 


#endif /* MAIN_H_ */
#pragma once