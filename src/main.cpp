#include <Arduino.h>
#include <ArduinoOTA.h>
#include "settings.h"

#include "NetworkManager.h"

#include <TLog.h>
#include <TelnetSerialStream.h>

unsigned long zeroAt;
unsigned long lastZeroAt;

#define ZEROES_SAMPLE   20
unsigned int zeroes[ZEROES_SAMPLE];
int zeroesPos;

void zeroCrossCallback();

void setup()
{
    Serial.begin(115200);

    delay(100);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, 0);
    delay(200);
    digitalWrite(LED_BUILTIN, 1);
    delay(200);
    digitalWrite(LED_BUILTIN, 0);
    delay(200);
    digitalWrite(LED_BUILTIN, 1);
    delay(200);

    // Initialize Logger
    Log.begin();

    Log.println();
    Log.println("=== CSR Wemos Driver ===");
    Log.println(String(F("  Version: ")) + F(VERSION));
    Log.println(String(F("  Build: ")) + F(__DATE__) + " " + F(__TIME__));

    Network.setup();
    digitalWrite(LED_BUILTIN, 0);

    lastZeroAt = 0;
    zeroAt = 0;
    zeroesPos = 0;

    attachInterrupt(digitalPinToInterrupt(ZERO_CROSS_PIN), zeroCrossCallback, CHANGE);
}

ICACHE_RAM_ATTR 
void zeroCrossCallback()
{
    lastZeroAt = zeroAt;
    zeroAt = micros();
    zeroes[zeroesPos] = zeroAt - lastZeroAt;
    zeroesPos++;
    if (zeroesPos >= ZEROES_SAMPLE)
        zeroesPos = 0;
}

void loop()
{
    Log.loop();
    Network.handle();
    static unsigned long last_report = millis();
    if (millis() - last_report < 1 * 1000)
        return;

    int zeroFreq = 0;
    for (int i = 0; i < ZEROES_SAMPLE; ++i)
        zeroFreq += zeroes[i];

    zeroFreq = zeroFreq / ZEROES_SAMPLE;

    Log.printf("Period of zero cross: %d\n\r", zeroFreq);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    last_report = millis();
}
