#ifndef POTATO_DYNAMICS_MPU6050_H
#define POTATO_DYNAMICS_MPU6050_H

#include "I2C.h"

#define MPU6050_ADDRESS_WRITE 0xD0
#define MPU6050_ADDRESS_READ 0xD1

#define MPU6050_PWR_MGMT_1_REGISTER 0x6B
#define MPU6050_GYRO_CONFIG_REGISTER 0x1B
#define MPU6050_ACCEL_XOUT_H_REGISTER 0x3B

typedef struct
{
    int16_t x, y, z;
} MPU6050_Vector3;

typedef enum
{
    MPU6050_AccelerometerFSR_2g  = 0x00, // +-19.6133
    MPU6050_AccelerometerFSR_4g  = 0x08, // +-39.2266
    MPU6050_AccelerometerFSR_8g  = 0x10, // +-78.4532
    MPU6050_AccelerometerFSR_16g = 0x18  // +-156.9064
} MPU6050_AccelerometerFSR;

typedef enum
{
    MPU6050_GyroscopeFSR_250  = 0x00,
    MPU6050_GyroscopeFSR_500  = 0x08,
    MPU6050_GyroscopeFSR_1000 = 0x10,
    MPU6050_GyroscopeFSR_2000 = 0x18
} MPU6050_GyroscopeFSR;

typedef struct
{
    MPU6050_Vector3 accelerometer;
    MPU6050_Vector3 gyroscope;
    int16_t temperature;
} MPU6050;

void MPU6050_Init(MPU6050_AccelerometerFSR accelerometer_fsr, MPU6050_GyroscopeFSR gyroscope_fsr);
void MPU6050_Read(MPU6050 *mpu6050);

#endif // POTATO_DYNAMICS_MPU6050_H
