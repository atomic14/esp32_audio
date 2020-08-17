#include <Arduino.h>
#include <WiFi.h>
#include "esp_adc_cal.h"

// calibration values for the adc
#define DEFAULT_VREF 1100
esp_adc_cal_characteristics_t *adc_chars;

void setup()
{
  Serial.begin(115200);
  Serial.println("Started up");

  //Range 0-4096
  adc1_config_width(ADC_WIDTH_BIT_12);
  // full voltage range
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);

  // check to see what calibration is available
  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
  {
    Serial.println("Using voltage ref stored in eFuse");
  }
  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
  {
    Serial.println("Using two point values from eFuse");
  }
  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_DEFAULT_VREF) == ESP_OK)
  {
    Serial.println("Using default VREF");
  }
  //Characterize ADC
  adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
}

void loop()
{
  // for a more accurate reading you could read multiple samples here

  // read a sample from the adc using GPIO35
  int sample = adc1_get_raw(ADC1_CHANNEL_7);
  // get the calibrated value
  int milliVolts = esp_adc_cal_raw_to_voltage(sample, adc_chars);

  Serial.printf("Sample=%d, mV=%d\n", sample, milliVolts);

  delay(500);
}