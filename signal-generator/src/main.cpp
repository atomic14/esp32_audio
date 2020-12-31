#include <Arduino.h>
#include <U8x8lib.h>
#include "SignalGenerator.h"

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/15, /* data=*/4, /* reset=*/16);

SignalGenerator *signalGenerator;

char line1[200];
long freq = 10000;

void show_freq()
{
  sprintf(line1, "Freq %ldKHz", freq / 1000);
  u8x8.clearLine(0);
  u8x8.drawString(0, 0, line1);
}

void setup()
{
  Serial.begin(115200);
  delay(2000);
  Serial.println("Started up...");
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  pinMode(0, INPUT_PULLUP);
  signalGenerator = new SignalGenerator();
  signalGenerator->start(freq);
  show_freq();
}

void loop()
{
  if (digitalRead(0) == 0)
  {
    freq += 10000;
    signalGenerator->set_frequency(freq);
    show_freq();
    delay(1000);
  }
}