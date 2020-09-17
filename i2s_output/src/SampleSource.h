#ifndef __sample_source_h__
#define __sample_source_h__

#include <Arduino.h>

/**
 * Base class for our sample generators
 **/
class SampleSource
{
public:
    virtual int sampleRate() = 0;
    // This should fill the samples buffer with the specified number of frames
    // A frame contains a LEFT and a RIGHT sample. Each sample should be signed 16 bits
    virtual void getSamples(int16_t *samples, int number_frames) = 0;
};

#endif