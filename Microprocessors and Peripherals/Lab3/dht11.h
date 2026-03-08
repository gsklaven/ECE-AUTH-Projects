#include "gpio.h"
#include "delay.h"
#include <stdint.h>
#include "platform.h"
#include <stdio.h>
#include <stdbool.h>

void startSignal();
uint8_t dataTransmission(); 
float* getData();
bool checksum(uint8_t* data);
