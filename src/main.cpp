#include <Arduino.h>
#include <ArduinoOTA.h>
#include "settings.h"

#include "NetworkManager.h"

#include <TLog.h> 
#include <TelnetSerialStream.h>


void setup() {
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

}

void loop()
{
  Log.loop();
  Network.handle();
  static unsigned  long last_report = millis();
  if (millis() - last_report < 1 * 1000)
    return;

  static int i = 0;
  Log.printf("Hello number %d from the loop\n", i++);
  last_report = millis();
}
