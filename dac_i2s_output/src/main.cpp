#include <Arduino.h>
#include <SPIFFS.h>
#include "WAVFileReader.h"
#include "DACOutput.h"

SampleSource *sampleSource;
DACOutput *output;

void setup()
{
  Serial.begin(115200);

  Serial.println("Starting up");

  SPIFFS.begin();

  Serial.println("Created sample source");

  sampleSource = new WAVFileReader("/sample.wav");

  Serial.println("Starting I2S Output");
  output = new DACOutput();
  output->start(sampleSource);
}

void loop()
{
  // nothing to do here - everything is taken care of by tasks
}