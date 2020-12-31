#include <Arduino.h>
#include <driver/dac.h>
#include <U8x8lib.h>

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/15, /* data=*/4, /* reset=*/16);

void setup()
{
  Serial.begin(115200);
  delay(5000);
  Serial.println("Started up...");
  dac_output_enable(DAC_CHANNEL_1);
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
}

void loop()
{
  char line1[200];
  char line2[200];
  for (int i = 0; i < 256; i += 15)
  {
    Serial.printf("Sending %2d, expecting %.2f output\n", i, i * 3.3 / 255.0f);
    sprintf(line1, "Sending %2d", i);
    sprintf(line2, "expecting %.2fv", float(i) * 3.3 / 255.0f);
    u8x8.clearLine(0);
    u8x8.drawString(0, 0, line1);
    u8x8.clearLine(1);
    u8x8.drawString(0, 1, line2);
    dac_output_voltage(DAC_CHANNEL_1, i);
    delay(1000);
  }
}