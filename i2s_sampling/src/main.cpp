#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "WiFiCredentials.h"
#include "I2SSampler.h"

WiFiClient *wifiClient = NULL;
HTTPClient *httpClient = NULL;
I2SSampler *sampler = NULL;

// replace this with your machines IP Address
#define SERVER_URL "http://10.0.1.18:5003/samples"

// Task to write samples to our server
void writerTask(void *param)
{
  sampler = (I2SSampler *)param;
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100);
  while (true)
  {
    // wait for some samples to save
    uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
    if (ulNotificationValue > 0)
    {
      // send them off to the server
      digitalWrite(2, HIGH);
      Serial.println("Sending data");
      httpClient->begin(*wifiClient, SERVER_URL);
      httpClient->addHeader("content-type", "application/octet-stream");
      httpClient->POST((uint8_t *)sampler->sampleBuffer(), sampler->numSamples() * sizeof(uint16_t));
      httpClient->end();
      digitalWrite(2, LOW);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  // launch WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("Started up");
  // indicator LED
  pinMode(2, OUTPUT);
  // setup the HTTP Client
  wifiClient = new WiFiClient();
  httpClient = new HTTPClient();

  // create our sampler
  sampler = new I2SSampler();
  // set up the sample writer task
  TaskHandle_t writerTaskHandle;
  xTaskCreatePinnedToCore(writerTask, "Writer Task", 8192, sampler, 1, &writerTaskHandle, 1);
  // start sampling
  sampler->start(writerTaskHandle);
}

void loop()
{
  // nothing to do here - everything is taken care of by tasks
}