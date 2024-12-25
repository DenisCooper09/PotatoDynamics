#include <SPI.h>
#include <Arduino.h>

extern "C"
{
#include "I2C.h"
#include "MPU6050.h"
#include "BME280.h"
}

static MPU6050 s_MPU6050;
static BME280  s_BME280;

void setup()
{
    Serial.begin(115200);

    I2C_Init(I2C_400kHz);

    MPU6050_Config mpu6050_config;
    mpu6050_config.accelerometer_fsr = MPU6050_AccelerometerFSR_16g;
    mpu6050_config.gyroscope_fsr     = MPU6050_GyroscopeFSR_500;

    MPU6050_Init(&mpu6050_config);

    BME280_Config bme280_config;
    bme280_config.mode             = BME280_Mode_Normal;
    bme280_config.standby          = BME280_StandbyPeriod_62500us;
    bme280_config.filter           = BME280_FilterCoefficient_8;
    bme280_config.pressure_osrs    = BME280_Oversampling_x8;
    bme280_config.temperature_osrs = BME280_Oversampling_x8;
    bme280_config.humidity_osrs    = BME280_Oversampling_x8;

    BME280_Init(&bme280_config);
    BME280_ReadCalibrationData(&s_BME280);
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

    /*
    BME280_Read(&s_BME280);

    Serial.print("hum:");
    //Serial.println(BME280_CompensateTemperature(&s_BME280));
    //Serial.println(BME280_CompensatePressure(&s_BME280) / 100);
    BME280_CompensateTemperature(&s_BME280);
    Serial.println(BME280_CompensateHumidity(&s_BME280));
     */
}
