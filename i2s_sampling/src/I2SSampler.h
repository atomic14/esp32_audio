#ifndef __i2s_sampler_h__
#define __i2s_sampler_h__

#define ADC_SAMPLES_COUNT 32768 // approx ~0.8 seconds at 40KHz, 

class I2SSampler
{
public:
    // double buffer so we can be capturing samples while sending data
    int16_t *audioBuffer1;
    int16_t *audioBuffer2;
    // current position in the audio buffer
    int32_t audioBufferPos = 0;
    // current audio buffer
    int16_t *currentAudioBuffer;
    // buffer containing samples that have been captured already
    int16_t *capturedAudioBuffer;
    // writer task
    TaskHandle_t writerTaskHandle;
    // reader queue
    QueueHandle_t i2s_queue;

public:
    I2SSampler();
    ~I2SSampler();
    virtual int16_t *sampleBuffer()
    {
        return capturedAudioBuffer;
    }
    int numSamples()
    {
        return ADC_SAMPLES_COUNT;
    }
    void start(TaskHandle_t writerTaskHandle);
};

#endif