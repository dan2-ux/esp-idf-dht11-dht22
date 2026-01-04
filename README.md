# DHT11 & DHT22 Temperature and Humidity Sensor

This repository demonstrates how to interface DHT11 and DHT22 temperature and humidity sensors using a single-wire digital protocol.

Both sensors provide temperature and relative humidity data and are commonly used in embedded and IoT projects.

<img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/64293a98-953e-4d7e-b100-586ffc03facf" />

## Sensor Overview
- DHT11 is suitable for basic applications

- DHT22 provides higher accuracy and a wider measurement range

## How It Works

DHT sensors communicate using a single data wire with a strict timing-based protocol.

#### The microcontroller:

1. Sends a start signal to the sensor

2. Waits for the sensor response

3. Reads 40 bits of data:

- 16 bits humidity

- 16 bits temperature

- 8 bits checksum

## GPIO Configuration

#### Data pin is configured as:

- Output during the start signal

- Input while receiving data

A pull-up resistor is required on the data line (usually built into the module).

## Communication Timing

- Timing accuracy is critical

- Each bit is determined by how long the data line stays HIGH

- A checksum is used to verify data integrity

If timing is incorrect, the read operation will fail.
