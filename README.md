# ESP32 I2S ADC Sampling

Explanatory video [here](https://www.youtube.com/watch?v=pPh3_ciEmzs)

[![Demo Video](https://img.youtube.com/vi/pPh3_ciEmzs/0.jpg)](https://www.youtube.com/watch?v=pPh3_ciEmzs)

This project demonstrates how to use the ESP32 built-in Analog to Digital Converters.

There are three projects in this repository: `loop_sampling`, `i2s_sampling` and `server`.

## loop_sampling

This project shows how to use the Arduino `analogRead` function and the Espressif `adc1_get_raw` function.

It also demonstrates how to get a calibrated value back from the ADC to give you the actual voltage at the input.

## i2s_sampling

This project handles both analogue devices (such as the MAX4466 and the MAX9814) and I2S devices (such as the SPH0645 and INMP441).

This project demonstrates how to use the I2S peripheral for high-speed sampling using DMA to transfer samples directly to RAM.

We can read these samples from the internal ADC or from the I2S perifpheral directly.

Samples are read from the DMA buffers and sent to a server running on your laptop/desktop machine.

The current set of pin assignment in the code are:

### ADC

| Function       | GPIO Pin | Notes                      |
| -------------- | -------- | -------------------------- |
| Analogue input | GPIO34   | ADC_UNIT_1, ADC1_CHANNEL_6 |

### I2S

| Function    | GPIO Pin    | Notes                          |
| ----------- | ----------- | ------------------------------ |
| bck_io_num  | GPIO_NUM_32 | I2S - Serial clock             |
| ws_io_num   | GPIO_NUM_25 | I2S - LRCLK - left right clock |
| data_in_num | GPIO_NUM_33 | I2S - Serial data              |

## server

This is a simple `node` server that writes the samples received from the ESP32 to a file.
