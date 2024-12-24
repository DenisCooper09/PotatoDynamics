#include <SPI.h>
#include <Arduino.h>

extern "C"
{
#include "I2C.h"
}

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    Serial.println("Hello, World");
}
