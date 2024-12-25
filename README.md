# PotatoDynamics
Projectile tracker.
Originally made to measure the dynamics of a potato during its flight launched from [a potato cannon](https://en.wikipedia.org/wiki/Potato_cannon).

## Measurement Flow
PotatoTracker measures at a set frequency (500Hz by default, a compromise between measurement time and write to SD time).

- Acceleration (x, y, z)
- Angular Velocity (x, y, z)
- Pressure (for height approximation)
- Temperature
- Humidity

PotatoTracker uses a double-buffer system. While into the first buffer data is written, the data from the second buffer is read and copied onto the MicroSD card.
Then buffers swap. This measurement frequency.

## Components
- ATmega328P (in the form of arduino nano)
- MPU6050 (accelerometer)
- BME280 (barometer)
- SD/MicroSD card module (MicroSD is used in my case)
