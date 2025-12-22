#include <WiFi.h>
#include <FastLED.h>

// -------- WiFi Hotspot --------
const char* ssid = "SensorBox_HOTSPOT";
const char* password = "12345678";
WiFiServer server(3333);
WiFiClient client;

// -------- Sensors (ACTIVE HIGH) --------
int sensorPins[] = {2, 4, 15};
int sensorCount = 3;

// We want PLAY when sensor becomes HIGH
const int TRIGGER_STATE = HIGH;

// Debounce
unsigned long lastTrigger = 0;
const unsigned long debounceMs = 400;

// -------- ARGB Strip --------
#define LED_PIN 14
#define NUM_LEDS 230      // change for your strip length
CRGB leds[NUM_LEDS];

bool effectActive = false;
unsigned long effectStartTime = 0;
const unsigned long effectDuration = 2000; // ms

// LED Effect (rainbow)
void runEffect()
{
    for (int b = 50; b <= 255; b++) {
    fill_solid(leds, NUM_LEDS, CRGB::Cyan); // choose color
    FastLED.setBrightness(b);
    FastLED.show();
    delay(4);
  }

  // Fade OUT
  for (int b = 255; b >= 50; b--) {
    fill_solid(leds, NUM_LEDS, CRGB::Cyan);
    FastLED.setBrightness(b);
    FastLED.show();
    delay(4);
  }
}

void clearStrip()
{
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
}

void setup()
{
    Serial.begin(115200);

    // Sensor pins
    for (int i = 0; i < sensorCount; i++)
        pinMode(sensorPins[i], INPUT); // ACTIVE HIGH sensor

    // ARGB strip
    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    clearStrip();

    // WiFi AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());

    server.begin();
}

void loop()
{
    // Accept client
    if (!client || !client.connected())
        client = server.available();

    // Check sensors
    for (int i = 0; i < sensorCount; i++)
    {
        int state = digitalRead(sensorPins[i]);

        if (state == TRIGGER_STATE)
        {
            unsigned long now = millis();

            if (now - lastTrigger > debounceMs)
            {
                lastTrigger = now;

                Serial.println("Sensor Triggered → Sending PLAY");

                if (client && client.connected())
                    client.print("PLAY\n");

                // Start ARGB effect
                effectActive = true;
                effectStartTime = millis();
            }
        }
    }

    // Handle ARGB effect
    if (effectActive)
    {
        if (millis() - effectStartTime < effectDuration)
        {
            runEffect();
        }
        else
        {
            effectActive = false;
            clearStrip();
        }
    }

    delay(10);
}
