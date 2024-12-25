#ifndef POTATO_DYNAMICS_BME280_H
#define POTATO_DYNAMICS_BME280_H

#include <util/delay.h>

#include "I2C.h"

#define BME280_ADDRESS_WRITE 0xEC
#define BME280_ADDRESS_READ 0xED

#define BME280_RESET_REGISTER 0xE0
#define BME280_CTRL_HUM_REGISTER 0xF2
#define BME280_CTRL_MEAS_REGISTER 0xF4

#define BME280_CALIB00_REGISTER 0x88
#define BME280_CALIB26_REGISTER 0xE1

#define BME280_PRESS_MSB_REGISTER 0xF7

typedef enum
{
    BME280_Mode_Sleep  = 0,
    BME280_Mode_Forced = 1,
    BME280_Mode_Normal = 3,
} BME280_Mode;

typedef enum
{
    BME280_Oversampling_Skipped = 0,
    BME280_Oversampling_x1      = 1,
    BME280_Oversampling_x2      = 2,
    BME280_Oversampling_x4      = 3,
    BME280_Oversampling_x8      = 4,
    BME280_Oversampling_x16     = 5
} BME280_Oversampling;

typedef enum
{
    BME280_FilterCoefficient_Off = 0,
    BME280_FilterCoefficient_2   = 1,
    BME280_FilterCoefficient_4   = 2,
    BME280_FilterCoefficient_8   = 3,
    BME280_FilterCoefficient_16  = 4
} BME280_FilterCoefficient;

typedef enum
{
    BME280_StandbyPeriod_500us   = 0,
    BME280_StandbyPeriod_62500us = 1,
    BME280_StandbyPeriod_125ms   = 2,
    BME280_StandbyPeriod_250ms   = 3,
    BME280_StandbyPeriod_500ms   = 4,
    BME280_StandbyPeriod_1s      = 5,
    BME280_StandbyPeriod_10ms    = 6,
    BME280_StandbyPeriod_20ms    = 7
} BME280_StandbyPeriod;

typedef struct
{
    BME280_Mode              mode;
    BME280_StandbyPeriod     standby;
    BME280_FilterCoefficient filter;
    BME280_Oversampling      pressure_osrs, temperature_osrs, humidity_osrs;
} BME280_Config;

typedef struct
{
    int32_t pressure;
    int32_t temperature;
    int32_t humidity;

    union
    {
        struct
        {
            uint16_t dig_T1;
            int16_t  dig_T2;
            int16_t  dig_T3;

            uint16_t dig_P1;
            int16_t  dig_P2;
            int16_t  dig_P3;
            int16_t  dig_P4;
            int16_t  dig_P5;
            int16_t  dig_P6;
            int16_t  dig_P7;
            int16_t  dig_P8;
            int16_t  dig_P9;

            uint8_t dig_H1;
            int16_t dig_H2;
            uint8_t dig_H3;
            int16_t dig_H4;
            int16_t dig_H5;
            int8_t  dig_H6;

            int32_t t_fine;
        };

        uint8_t data[33];

    } calibration;

} BME280;

void BME280_Init(BME280_Config *config);
void BME280_ReadCalibrationData(BME280 *bme280);
void BME280_Read(BME280 *bme280);

double BME280_CompensateTemperature(BME280 *bme280);
double BME280_CompensatePressure(BME280 *bme280);
double BME280_CompensateHumidity(BME280 *bme280);

double BME280_Altitude(double pressure);

#endif // POTATO_DYNAMICS_BME280_H
