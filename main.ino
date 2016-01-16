#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NeoPixelBus.h>

#define pixelCount 150 // Strip has amount of pixelCount NeoPixels (5m * 30 leds/m)
#define pixelPin 2 // Strip is attached to GPIO2 on ESP-01
unsigned int localPort = 2342;
WiFiUDP udp;

const int MAX_PACKET_SIZE = 1024;
byte packetBuffer[ MAX_PACKET_SIZE];

NeoPixelBus strip = NeoPixelBus(pixelCount, pixelPin);

const char *ssid = "C3D2";
const char *password = "";

void setup()
{
  Serial.begin ( 115200 );
  Serial.println("setup begin");
  
  WiFi.begin ( ssid, password );

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println("wifi connected");

  udp.begin(localPort);
  strip.Begin();
  strip.Show();

  Serial.println("setup done");
}


void loop()
{
  int cb = udp.parsePacket();
  if (!cb) {
    //no packet yet
  }
  else {
    if (cb > 10) //cb is packet physical length
    {

      udp.read(packetBuffer, MAX_PACKET_SIZE);

      int prio = packetBuffer[0]; //prio 0-255
      int command = packetBuffer[1]; //we currently only support 0)

      int supposedLength = byte(packetBuffer[2]) << 8 | byte(packetBuffer[3]);

      if (cb != (supposedLength + 4)) //header
      {
        Serial.println("package like live - too short." + String(cb) + "!=" + String(supposedLength));
      }
      else
      {
        setStripByData(packetBuffer,supposedLength);
      }
    }
  }
}

void setStripByData(byte* data, int sl)
{
  Serial.println(sl);
  int led = 0;
  for (int i = 4; i <= sl; i = i + 3) {
   /* if (led < 3) //love for leds
    {
      Serial.print(data[i]); Serial.print(" ");
      Serial.print(data[i + 1]); Serial.print(" ");
      Serial.print(data[i + 2]); Serial.print(" ");
    }
   */
    strip.SetPixelColor(led, RgbColor(data[i], data[i + 1], data[i + 2]));
    led++;
  }
  strip.Show();
}
