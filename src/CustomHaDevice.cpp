#include <TLog.h>

#include "CustomHaDevice.h"
#include "settings.h"

CustomHaDevice::CustomHaDevice(String uniqueId, Client& client) : 
    _uniqueId(uniqueId),
    _device(_uniqueId.c_str()),
    _mqtt(client, _device),
    _networkFrequency(0),
    _networkFrequencyName(_uniqueId + String("_ac_frequency")),
    _networkFrequencyEntity("network_frequency", HABaseDeviceType::PrecisionP2), //_networkFrequencyName.c_str()
    _uptimeSensorEntity("Uptime"),
    _last_report(0)
{
}

CustomHaDevice::~CustomHaDevice()
{
}

void CustomHaDevice::setup()
{
    Log.printf("Setup Ha device\n\r");
    _device.setName("CSR Wemos Driver");
    _device.setSoftwareVersion(VERSION);
    _mqtt.begin(BROKER_ADDR, BROKER_USER, BROKER_PASSWORD);
    
    _networkFrequencyEntity.setIcon("mdi:sine-wave");
    _networkFrequencyEntity.setName("Network frequency");
    _networkFrequencyEntity.setUnitOfMeasurement("Hz");
    _networkFrequencyEntity.setDeviceClass("FREQUENCY");
    //_networkFrequencyEntity.setMode(HANumber::ModeBox);
    _uptimeSensorEntity.setIcon("mdi:timer-outline");
    _uptimeSensorEntity.setName("Uptime");
    _uptimeSensorEntity.setUnitOfMeasurement("s");

    _last_report = millis();
}

void CustomHaDevice::loop()
{
    _mqtt.loop();
    if (millis() - _last_report < 5 * 1000)
        return;

    Log.printf("Reporting network Frequency value\n\r");
    _networkFrequencyEntity.setValue(_networkFrequency);
    _uptimeSensorEntity.setValue(millis() / 1000.f);
    _last_report = millis();
}

void CustomHaDevice::networkFrequency(float value)
{
    _networkFrequency = value;
}