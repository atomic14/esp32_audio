#ifndef __sampler_base_h__
#define __sampler_base_h__

#include <Arduino.h>
#include "driver/i2s.h"

/**
 * Base Class for both the ADC and I2S sampler
 **/
class SignalGenerator
{
private:
    // I2S write task
    TaskHandle_t m_i2sWriterTaskHandle;
    // i2s writer queue
    QueueHandle_t m_i2sQueue;

public:
    void start(long freq);
    void set_frequency(long new_freq);

    friend void i2sWriterTask(void *param);
};

#endif