#include <SPI.h>
#include <Arduino.h>

extern "C"
{
#include "I2C.h"
#include "MPU6050.h"
}

static MPU6050 s_MPU6050;

void setup()
{
    Serial.begin(115200);

    I2C_Init(I2C_400kHz);

    MPU6050_Init(MPU6050_AccelerometerFSR_8g, MPU6050_GyroscopeFSR_500);
}

void loop()
{
    MPU6050_Read(&s_MPU6050);

    Serial.print("X:");
    Serial.print(s_MPU6050.accelerometer.x);
    Serial.print(",");
    Serial.print("Y:");
    Serial.print(s_MPU6050.accelerometer.y);
    Serial.print(",");
    Serial.print("Z:");
    Serial.println(s_MPU6050.accelerometer.z);
}
