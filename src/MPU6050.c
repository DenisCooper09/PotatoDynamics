#include "MPU6050.h"

void MPU6050_Init(MPU6050_AccelerometerFSR accelerometer_fsr, MPU6050_GyroscopeFSR gyroscope_fsr)
{
    I2C_Start();
    I2C_Transmit(MPU6050_ADDRESS_WRITE);
    I2C_Transmit(MPU6050_PWR_MGMT_1_REGISTER);
    I2C_Transmit(0x00); // Clear register to wake up slave
    I2C_Start();
    I2C_Transmit(MPU6050_ADDRESS_WRITE);
    I2C_Transmit(MPU6050_GYRO_CONFIG_REGISTER); // Write from "GYRO_CONFIG" until "ACCEL_CONFIG" register
    I2C_Transmit(gyroscope_fsr);
    I2C_Transmit(accelerometer_fsr);
    I2C_Stop();
}

void MPU6050_Read(MPU6050 *mpu6050)
{
    I2C_Start();
    I2C_Transmit(MPU6050_ADDRESS_WRITE);
    I2C_Transmit(MPU6050_ACCEL_XOUT_H_REGISTER);

    I2C_Start();
    I2C_Transmit(MPU6050_ADDRESS_READ);

    mpu6050->accelerometer.x = I2C_Receive16(I2C_ACK);
    mpu6050->accelerometer.y = I2C_Receive16(I2C_ACK);
    mpu6050->accelerometer.z = I2C_Receive16(I2C_ACK);

    // Ignore temperature LSB and MSB registers.
    I2C_Receive8(I2C_ACK);
    I2C_Receive8(I2C_ACK);

    mpu6050->gyroscope.x = I2C_Receive16(I2C_ACK);
    mpu6050->gyroscope.y = I2C_Receive16(I2C_ACK);
    mpu6050->gyroscope.z = I2C_Receive16(I2C_NAK);

    I2C_Stop();
}