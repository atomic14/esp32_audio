# ESP32 I2S ADC Sampling

[![Demo Video](https://img.youtube.com/vi/pPh3_ciEmzs/0.jpg)](https://www.youtube.com/watch?v=pPh3_ciEmzs)

This project demonstrates how to use the ESP32 built-in Analog to Digital Converters.

There are three projects in this repository: `loop_sampling`, `i2s_sampling` and `server`.

## loop_sampling

This project shows how to use the Arduino `analogRead` function and the Espressif `adc1_get_raw` function.

It also demonstrates how to get a calibrated value back from the ADC to give you the actual voltage at the input.

## i2s_sampling

This project demonstrates how to use the I2S peripheral for high-speed sampling using DMA to transfer samples directly to RAM from the ADC without using the CPU resources.

Samples are read from the DMA buffers and sent to a server running on your laptop/desktop machine.

## server

This is a simple `node` server that writes the samples received from the ESP32 to a file.
