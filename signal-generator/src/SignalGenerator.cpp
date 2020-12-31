
#include <Arduino.h>
#include "driver/i2s.h"
#include <math.h>
#include <SPIFFS.h>
#include "SignalGenerator.h"

void i2sWriterTask(void *param)
{
    SignalGenerator *output = (SignalGenerator *)param;
    int buffer_position = 0;
    // create a triangle wave
    uint16_t sawTooth[1024];
    for (int i = 0; i < 1024; i += 2)
    {
        sawTooth[i] = sawTooth[i + 1] = (i % 32) << 11;
        Serial.printf("%d\n", sawTooth[i]);
    }
    while (true)
    {
        // wait for some data to be requested
        i2s_event_t evt;
        if (xQueueReceive(output->m_i2sQueue, &evt, portMAX_DELAY) == pdPASS)
        {
            if (evt.type == I2S_EVENT_TX_DONE)
            {
                size_t bytesWritten = 0;
                do
                {
                    if (buffer_position >= 1024)
                    {
                        // Serial.println("Resetting buffer pos");
                        buffer_position = 0;
                    }
                    int availableSamples = 1024 - buffer_position;
                    // write data to the i2s peripheral
                    i2s_write(I2S_NUM_0, sawTooth + buffer_position, availableSamples * 2, &bytesWritten, portMAX_DELAY);
                    buffer_position += bytesWritten / 2;
                    // Serial.printf("Wrote bytes %d, available bytes %d\n", bytesWritten, availableSamples * 2);
                } while (bytesWritten > 0);
            }
        }
    }
}

void SignalGenerator::start(long freq)
{
    // i2s config for writing both channels of I2S
    i2s_config_t i2sConfig = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
        .sample_rate = freq,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 64,
        .use_apll = 1
    };

    //install and start i2s driver
    i2s_driver_install(I2S_NUM_0, &i2sConfig, 4, &m_i2sQueue);
    // enable the DAC channels
    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
    // clear the DMA buffers
    i2s_zero_dma_buffer(I2S_NUM_0);
    // start a task to write samples to the i2s peripheral
    TaskHandle_t writerTaskHandle;
    xTaskCreate(i2sWriterTask, "i2s Writer Task", 4096, this, 1, &writerTaskHandle);
}

void SignalGenerator::set_frequency(long new_freq)
{
    i2s_set_sample_rates(I2S_NUM_0, new_freq);
}
