#include "BME280.h"

void BME280_Init(BME280_Config *config)
{
    I2C_Start();
    I2C_Transmit(BME280_ADDRESS_WRITE);
    I2C_Transmit(BME280_RESET_REGISTER);
    I2C_Transmit(0xB6); // Reset slave
    I2C_Stop();

    // Wait for the slave to reset
    _delay_ms(5);

    I2C_Start();
    I2C_Transmit(BME280_ADDRESS_WRITE);
    I2C_Transmit(BME280_CTRL_HUM_REGISTER);
    I2C_Transmit(config->humidity_osrs);
    I2C_Start();
    I2C_Transmit(BME280_ADDRESS_WRITE);
    I2C_Transmit(BME280_CTRL_MEAS_REGISTER);
    I2C_Transmit((config->temperature_osrs << 5) | (config->pressure_osrs << 2) | config->mode); //ctrl_meas register
    I2C_Transmit((config->standby << 5) | (config->filter << 2)); // config register
    I2C_Stop();
}

void BME280_ReadCalibrationData(BME280 *bme280)
{
    I2C_Start();
    I2C_Transmit(BME280_ADDRESS_WRITE);
    I2C_Transmit(BME280_CALIB00_REGISTER);
    I2C_Start();
    I2C_Transmit(BME280_ADDRESS_READ);

    bme280->calibration.dig_T1 = (uint16_t) I2C_Receive(I2C_ACK) | ((uint16_t) I2C_Receive(I2C_ACK) << 8);
    bme280->calibration.dig_T2 = (int16_t) I2C_Receive(I2C_ACK) | ((int16_t) I2C_Receive(I2C_ACK) << 8);
    bme280->calibration.dig_T3 = (int16_t) I2C_Receive(I2C_ACK) | ((int16_t) I2C_Receive(I2C_ACK) << 8);
    bme280->calibration.dig_P1 = (uint16_t) I2C_Receive(I2C_ACK) | ((uint16_t) I2C_Receive(I2C_ACK) << 8);
    bme280->calibration.dig_P2 = (int16_t) I2C_Receive(I2C_ACK) | ((int16_t) I2C_Receive(I2C_ACK) << 8);
    bme280->calibration.dig_P3 = (int16_t) I2C_Receive(I2C_ACK) | ((int16_t) I2C_Receive(I2C_ACK) << 8);
    bme280->calibration.dig_P4 = (int16_t) I2C_Receive(I2C_ACK) | ((int16_t) I2C_Receive(I2C_ACK) << 8);
    bme280->calibration.dig_P5 = (int16_t) I2C_Receive(I2C_ACK) | ((int16_t) I2C_Receive(I2C_ACK) << 8);
    bme280->calibration.dig_P6 = (int16_t) I2C_Receive(I2C_ACK) | ((int16_t) I2C_Receive(I2C_ACK) << 8);
    bme280->calibration.dig_P7 = (int16_t) I2C_Receive(I2C_ACK) | ((int16_t) I2C_Receive(I2C_ACK) << 8);
    bme280->calibration.dig_P8 = (int16_t) I2C_Receive(I2C_ACK) | ((int16_t) I2C_Receive(I2C_ACK) << 8);
    bme280->calibration.dig_P9 = (int16_t) I2C_Receive(I2C_ACK) | ((int16_t) I2C_Receive(I2C_ACK) << 8);
    I2C_Receive(I2C_ACK);
    bme280->calibration.dig_H1 = I2C_Receive(I2C_NAK);

    I2C_Start();
    I2C_Transmit(BME280_ADDRESS_WRITE);
    I2C_Transmit(BME280_CALIB26_REGISTER);
    I2C_Start();
    I2C_Transmit(BME280_ADDRESS_READ);

    bme280->calibration.dig_H2 = (int16_t) I2C_Receive(I2C_ACK) | ((int16_t) I2C_Receive(I2C_ACK) << 8);
    bme280->calibration.dig_H3 = I2C_Receive(I2C_ACK);
    bme280->calibration.dig_H4 = (int16_t) ((int8_t) I2C_Receive(I2C_ACK) << 4) | (int16_t) (I2C_Receive(I2C_ACK) & 0x0F);
    bme280->calibration.dig_H5 = (int16_t) ((int8_t) I2C_Receive(I2C_ACK) >> 4) | ((int8_t) I2C_Receive(I2C_ACK) << 4);
    bme280->calibration.dig_H6 = (int8_t) I2C_Receive(I2C_NAK);

    I2C_Stop();
}

void BME280_Read(BME280 *bme280)
{
    I2C_Start();
    I2C_Transmit(BME280_ADDRESS_WRITE);
    I2C_Transmit(BME280_PRESS_MSB_REGISTER);
    I2C_Start();
    I2C_Transmit(BME280_ADDRESS_READ);

    // press_msb, press_lsb, press_xlsb
    bme280->pressure =
            ((int32_t) I2C_Receive(I2C_ACK) << 12) |
            ((int32_t) I2C_Receive(I2C_ACK) << 4) |
            ((int32_t) I2C_Receive(I2C_ACK) >> 4);

    // temp_msb, temp_lsb, temp_xlsb
    bme280->temperature =
            ((int32_t) I2C_Receive(I2C_ACK) << 12) |
            ((int32_t) I2C_Receive(I2C_ACK) << 4) |
            ((int32_t) I2C_Receive(I2C_ACK) >> 4);

    // hum_msb, hum_lsb
    bme280->humidity = ((int32_t) I2C_Receive(I2C_ACK) << 8) | (int32_t) I2C_Receive(I2C_NAK);

    I2C_Stop();
}

double BME280_CompensateTemperature(BME280 *bme280)
{
    double var1;
    double var2;
    double temperature;
    double temperature_min = -40;
    double temperature_max = 85;

    var1 = (((double) bme280->temperature) / 16384.0 - ((double) bme280->calibration.dig_T1) / 1024.0);
    var1 = var1 * ((double) bme280->calibration.dig_T2);
    var2 = (((double) bme280->temperature) / 131072.0 - ((double) bme280->calibration.dig_T1) / 8192.0);
    var2 = (var2 * var2) * ((double) bme280->calibration.dig_T3);
    bme280->calibration.t_fine = (int32_t) (var1 + var2);
    temperature = (var1 + var2) / 5120.0;

    if (temperature < temperature_min)
    {
        temperature = temperature_min;
    }
    else if (temperature > temperature_max)
    {
        temperature = temperature_max;
    }

    return temperature;
}

double BME280_CompensatePressure(BME280 *bme280)
{
    double var1;
    double var2;
    double var3;
    double pressure;
    double pressure_min = 30000.0;
    double pressure_max = 110000.0;

    var1 = ((double) bme280->calibration.t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double) bme280->calibration.dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double) bme280->calibration.dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double) bme280->calibration.dig_P4) * 65536.0);
    var3 = ((double) bme280->calibration.dig_P3) * var1 * var1 / 524288.0;
    var1 = (var3 + ((double) bme280->calibration.dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double) bme280->calibration.dig_P1);

    if (var1 > (0.0))
    {
        pressure = 1048576.0 - (double) bme280->pressure;
        pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1;
        var1     = ((double) bme280->calibration.dig_P9) * pressure * pressure / 2147483648.0;
        var2     = pressure * ((double) bme280->calibration.dig_P8) / 32768.0;
        pressure = pressure + (var1 + var2 + ((double) bme280->calibration.dig_P7)) / 16.0;

        if (pressure < pressure_min)
        {
            pressure = pressure_min;
        }
        else if (pressure > pressure_max)
        {
            pressure = pressure_max;
        }
    }
    else
    {
        pressure = pressure_min;
    }

    return pressure;
}

double BME280_CompensateHumidity(BME280 *bme280)
{
    double humidity;
    double humidity_min = 0.0;
    double humidity_max = 100.0;
    double var1;
    double var2;
    double var3;
    double var4;
    double var5;
    double var6;

    var1     = ((double) bme280->calibration.t_fine) - 76800.0;
    var2     = (((double) bme280->calibration.dig_H4) * 64.0 + (((double) bme280->calibration.dig_H5) / 16384.0) * var1);
    var3     = ((double) bme280->humidity) - var2;
    var4     = ((double) bme280->calibration.dig_H2) / 65536.0;
    var5     = (1.0 + (((double) bme280->calibration.dig_H3) / 67108864.0) * var1);
    var6     = 1.0 + (((double) bme280->calibration.dig_H6) / 67108864.0) * var1 * var5;
    var6     = var3 * var4 * (var5 * var6);
    humidity = var6 * (1.0 - ((double) bme280->calibration.dig_H1) * var6 / 524288.0);

    if (humidity > humidity_max)
    {
        humidity = humidity_max;
    }
    else if (humidity < humidity_min)
    {
        humidity = humidity_min;
    }

    return humidity;
}

double BME280_Altitude(double pressure)
{
    return 44330.0 * (1.0 - pow((pressure / 100.0) / 1013.25, 0.1903));
}
