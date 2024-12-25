#ifndef POTATO_DYNAMICS_I2C_H
#define POTATO_DYNAMICS_I2C_H

#include <util/twi.h>

typedef enum
{
    I2C_100kHz = 72,
    I2C_400kHz = 12
} I2C_BusFrequency;

typedef enum
{
    I2C_ACK = 1,
    I2C_NAK = 0
} I2C_Acknowledgement;

void I2C_Init(I2C_BusFrequency frequency);

void I2C_Start();
void I2C_Stop();

void I2C_Transmit(uint8_t data);
uint8_t I2C_Receive(I2C_Acknowledgement ack);

#endif // POTATO_DYNAMICS_I2C_H
