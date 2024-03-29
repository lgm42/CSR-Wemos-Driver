#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <Ethernet.h>
#include <TLog.h> 
#include <TelnetSerialStream.h>

class NetworkManager 
{
  public:
	  NetworkManager();
  	virtual ~NetworkManager();

    void setup();
    void handle();

  private:
    void updateNTP();
    
    unsigned long _tickNTPUpdate;
    WiFiManager _wifiManager;
    WiFiClient _wifiClient;
    TelnetSerialStream _telnetSerialStream;

};

#if !defined(NO_GLOBAL_INSTANCES)
extern NetworkManager Network;
#endif
