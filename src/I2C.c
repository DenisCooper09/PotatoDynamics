#include "I2C.h"

void I2C_Init(I2C_BusFrequency frequency)
{
    TWSR &= ~((1 << TWPS0) | (1 << TWPS1));
    TWBR = frequency;
}

void I2C_Start()
{
    TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTA);
    while (!(TWCR & (1 << TWINT)));
}

void I2C_Stop()
{
    TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);
}

void I2C_Transmit(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
}

uint8_t I2C_Receive8(I2C_Acknowledgement ack)
{
    TWCR = (1 << TWEN) | (1 << TWINT) | (ack << TWEA);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

int16_t I2C_Receive16(I2C_Acknowledgement ack)
{
    return (I2C_Receive8(I2C_ACK) << 8) | I2C_Receive8(ack);
}
