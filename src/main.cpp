#include <SPI.h>
#include <Arduino.h>
#include <SdFat.h>

extern "C"
{
#include "I2C.h"
#include "MPU6050.h"
#include "BME280.h"
}

#define SD_SLAVE_SELECT 10
#define SD_DYNAMICS_DIRECTORY "/DYNAMICS"
#define SD_DYNAMICS_FILE_NAME_FORMAT "/DYNAMICS/DYN%i.bin"
#define SD_CALIBRATION_FILE_NAME "/DYNAMICS/CALIB.bin"

static SdFat32 s_SD;
static char    s_DynamicsFileName[19];

static void SD_Init()
{
    if (!s_SD.begin(SD_SLAVE_SELECT))
    {
        return;
    }

    if (!s_SD.exists(SD_DYNAMICS_DIRECTORY))
    {
        s_SD.mkdir(SD_DYNAMICS_DIRECTORY);
    }
}

static void SD_FindDynamicsFileName()
{
    for (uint8_t i = 0; i < 100; ++i)
    {
        char file_name[19];
        sprintf(file_name, SD_DYNAMICS_FILE_NAME_FORMAT, i);

        if (!s_SD.exists(file_name))
        {
            memcpy(s_DynamicsFileName, file_name, 19);
            break;
        }
    }
}

static void SD_DeleteAllDynamicsFiles()
{
    PORTD &= ~(1 << PD2);

    s_SD.remove(SD_CALIBRATION_FILE_NAME);

    for (uint8_t i = 0; i < 100; ++i)
    {
        char file_name[19];
        sprintf(file_name, SD_DYNAMICS_FILE_NAME_FORMAT, i);

        PORTD ^= (1 << PD2);

        s_SD.remove(file_name);

        delay(1);
    }

    PORTD &= ~(1 << PD2);
}

static void SD_WriteCalibrationData(BME280 *bme280)
{
    File32 file = s_SD.open(SD_CALIBRATION_FILE_NAME, O_WRITE | O_CREAT);
    file.write(bme280->calibration.data, 33);
    file.close();
}

static MPU6050 s_MPU6050;
static BME280  s_BME280;

typedef union
{
    struct
    {
        MPU6050_Vector3 acceleration;
        MPU6050_Vector3 angular_velocity;

        int32_t pressure;
        int32_t temperature;
        int32_t humidity;
    };

    uint8_t data[24];

} DynamicsSample;

#define CHRONOLOGY_BUFFER_CAPACITY 10
#define SAMPLE_NUMBER 1000

static volatile DynamicsSample s_ChronologyBuffer1[CHRONOLOGY_BUFFER_CAPACITY];
static volatile DynamicsSample s_ChronologyBuffer2[CHRONOLOGY_BUFFER_CAPACITY];

static volatile DynamicsSample *s_WriteBuffer = s_ChronologyBuffer1;
static volatile DynamicsSample *s_ReadBuffer  = s_ChronologyBuffer2;

static volatile uint8_t s_SampleIndex     = 0;
static volatile bool    s_ReadBufferReady = false;
static bool             s_Sampling        = false;
static uint16_t         s_SampleNumber    = 0;

static void SD_WriteDynamicsSamples(const char *file_name)
{
    File32 file = s_SD.open(file_name, O_WRITE | O_APPEND);

    for (uint8_t i = 0; i < CHRONOLOGY_BUFFER_CAPACITY; ++i)
    {
        file.write((uint8_t *) s_ReadBuffer[i].data, 24);
    }

    file.close();
}

#define TIMER_PERIOD 32000

void setup()
{
    DDRD |= (1 << PD2);
    DDRD &= ~(1 << PD3);
    PORTD |= (1 << PD3); // Pull-Up Resistor

    PORTD &= ~(1 << PD2);

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

    SD_Init();
    if ((PIND & (1 << PD3)) == 0)
    {
        SD_DeleteAllDynamicsFiles();
    }
    delay(1000);
    SD_WriteCalibrationData(&s_BME280);

    cli();
    TCCR1A = 0x00;
    TCCR1B = 0x01;
    TCNT1  = 0xFFFF - TIMER_PERIOD;
    sei();

    PORTD |= (1 << PD2);
}

void loop()
{
    if (!s_Sampling && ((PIND & (1 << PD3)) == 0))
    {
        SD_FindDynamicsFileName();

        File32 file = s_SD.open(s_DynamicsFileName, O_WRITE | O_CREAT);
        file.write(0xFF);
        file.close();

        s_Sampling = true;

        TIMSK1 |= (1 << TOIE1);
    }

    if (s_ReadBufferReady)
    {
        SD_WriteDynamicsSamples(s_DynamicsFileName);
        s_ReadBufferReady = false;

        PORTD ^= (1 << PD2);

        if (++s_SampleNumber >= SAMPLE_NUMBER)
        {
            s_SampleNumber = 0;
            s_Sampling     = false;

            PORTD |= (1 << PD2);

            TIMSK1 &= ~(1 << TOIE1);
            TCNT1 = 0xFFFF - TIMER_PERIOD;
        }
    }
}

ISR(TIMER1_OVF_vect)
{
    MPU6050_Read(&s_MPU6050);
    BME280_Read(&s_BME280);

    s_WriteBuffer[s_SampleIndex].acceleration.x = s_MPU6050.accelerometer.x;
    s_WriteBuffer[s_SampleIndex].acceleration.y = s_MPU6050.accelerometer.y;
    s_WriteBuffer[s_SampleIndex].acceleration.z = s_MPU6050.accelerometer.z;

    s_WriteBuffer[s_SampleIndex].angular_velocity.x = s_MPU6050.gyroscope.x;
    s_WriteBuffer[s_SampleIndex].angular_velocity.y = s_MPU6050.gyroscope.y;
    s_WriteBuffer[s_SampleIndex].angular_velocity.z = s_MPU6050.gyroscope.z;

    s_WriteBuffer[s_SampleIndex].pressure    = s_BME280.pressure;
    s_WriteBuffer[s_SampleIndex].temperature = s_BME280.temperature;
    s_WriteBuffer[s_SampleIndex].humidity    = s_BME280.humidity;

    if (++s_SampleIndex >= CHRONOLOGY_BUFFER_CAPACITY)
    {
        s_SampleIndex = 0;

        volatile DynamicsSample *temp = s_ReadBuffer;
        s_ReadBuffer  = s_WriteBuffer;
        s_WriteBuffer = temp;

        s_ReadBufferReady = true;
    }

    TCNT1 = 0xFFFF - TIMER_PERIOD;
}
