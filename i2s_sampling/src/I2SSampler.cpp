#include <Arduino.h>
#include "driver/i2s.h"
#include "driver/adc.h"
#include "I2SSampler.h"

void readerTask(void *param)
{
    I2SSampler *sampler = (I2SSampler *)param;
    while (true)
    {
        // wait for some data to arrive on the queue
        i2s_event_t evt;
        if (xQueueReceive(sampler->i2s_queue, &evt, portMAX_DELAY) == pdPASS)
        {
            if (evt.type == I2S_EVENT_RX_DONE)
            {
                size_t bytesRead = 0;
                do
                {
                    // try and fill up our audio buffer
                    size_t bytesToRead = (ADC_SAMPLES_COUNT - sampler->audioBufferPos) * 2;
                    // read from i2s
                    i2s_read(I2S_NUM_0, (void *)(sampler->currentAudioBuffer + sampler->audioBufferPos), bytesToRead, &bytesRead, 10 / portTICK_PERIOD_MS);
                    sampler->audioBufferPos += bytesRead / 2;
                    if (sampler->audioBufferPos == ADC_SAMPLES_COUNT)
                    {
                        // process the samples
                        for (int i = 0; i < ADC_SAMPLES_COUNT; i++)
                        {
                            sampler->currentAudioBuffer[i] = (2048 - (sampler->currentAudioBuffer[i] & 0xfff)) * 15;
                        }
                        // swap to the other buffer
                        std::swap(sampler->currentAudioBuffer, sampler->capturedAudioBuffer);
                        // reset the sample position
                        sampler->audioBufferPos = 0;
                        // tell the writer task to save the data
                        xTaskNotify(sampler->writerTaskHandle, 1, eIncrement);
                    }
                } while (bytesRead > 0);
            }
        }
    }
}

#define SAMPLE_BUFFER_SIZE (256)

int16_t sampleBuffer[SAMPLE_BUFFER_SIZE];

void readerTaskMedianAverage(void *param)
{
    I2SSampler *sampler = (I2SSampler *)param;
    size_t sampleBufferPos = 0;
    while (true)
    {
        // wait for some data to arrive on the queue
        i2s_event_t evt;
        if (xQueueReceive(sampler->i2s_queue, &evt, portMAX_DELAY) == pdPASS)
        {
            if (evt.type == I2S_EVENT_RX_DONE)
            {
                size_t bytesRead = 0;
                do
                {
                    // try and fill up our audio buffer
                    size_t bytesToRead = (SAMPLE_BUFFER_SIZE - sampleBufferPos) * 2;
                    // read from i2s
                    i2s_read(I2S_NUM_0, (void *)(sampleBuffer + sampleBufferPos), bytesToRead, &bytesRead, 10 / portTICK_PERIOD_MS);
                    sampleBufferPos += bytesRead / 2;
                    if (sampleBufferPos == SAMPLE_BUFFER_SIZE)
                    {
                        sampleBufferPos = 0;
                        // process the samples in the buffer
                        for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
                        {
                            sampleBuffer[i] = (2048 - (sampleBuffer[i] & 0xfff)) * 15;
                        }
                        // take the median value of four samples
                        for (int i = 0; i < SAMPLE_BUFFER_SIZE; i += 4)
                        {
                            std::sort(sampleBuffer + i, sampleBuffer + i + 4);
                            int median = (sampleBuffer[i + 1] + sampleBuffer[i + 2]) / 2;
                            // add the median sample to the audio buffer
                            sampler->currentAudioBuffer[sampler->audioBufferPos] = median;
                            sampler->audioBufferPos++;
                            if (sampler->audioBufferPos == ADC_SAMPLES_COUNT)
                            {
                                // swap to the other buffer
                                std::swap(sampler->currentAudioBuffer, sampler->capturedAudioBuffer);
                                // reset the sample position
                                sampler->audioBufferPos = 0;
                                // tell the writer task to save the data
                                xTaskNotify(sampler->writerTaskHandle, 1, eIncrement);
                            }
                        }
                    }
                } while (bytesRead > 0);
            }
        }
    }
}

I2SSampler::I2SSampler()
{
    audioBuffer1 = (int16_t *)malloc(sizeof(int16_t) * ADC_SAMPLES_COUNT);
    audioBuffer2 = (int16_t *)malloc(sizeof(int16_t) * ADC_SAMPLES_COUNT);

    currentAudioBuffer = audioBuffer1;
    capturedAudioBuffer = audioBuffer2;

    audioBufferPos = 0;
}

I2SSampler::~I2SSampler()
{
    free(audioBuffer1);
    free(audioBuffer2);
}

void I2SSampler::start(TaskHandle_t writerTaskHandle)
{
    this->writerTaskHandle = writerTaskHandle;

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
        .sample_rate = 40000,
        // for median filter use .sample_rate = 160000
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S_LSB,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0};
    //install and start i2s driver
    i2s_driver_install(I2S_NUM_0, &i2s_config, 4, &i2s_queue);
    //init ADC pad
    i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_7);

    // enable the adc
    i2s_adc_enable(I2S_NUM_0);

    // start a task to read samples from I2S
    TaskHandle_t readerTaskHandle;
    // Median filter xTaskCreatePinnedToCore(readerTaskMedianAverage, "Reader Task", 8192, this, 1, &readerTaskHandle, 0);
    xTaskCreatePinnedToCore(readerTask, "Reader Task", 8192, this, 1, &readerTaskHandle, 0);
}
