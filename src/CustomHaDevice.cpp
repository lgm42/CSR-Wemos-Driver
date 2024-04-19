#include <TLog.h>

#include "CustomHaDevice.h"
#include "settings.h"

CustomHaDevice::CustomHaDevice() : 
    _mqtt(nullptr),
    _device(nullptr),
    _networkFrequency(0),
    _last_report(0),
    _setPointValue(0.f),
    _networkFrequencyEntity("CSR-dev1-network_frequency", HABaseDeviceType::PrecisionP2),
    _uptimeSensorEntity("CSR-dev1-uptime"),
    _powerSetPoint("CSR-dev1-power_setpoint"),

    _testNumber("CSR-dev1-test_number")
{
}

CustomHaDevice::~CustomHaDevice()
{
}

void CustomHaDevice::setup()
{
    String uniqueId = String("CSR-dev2-_") + WiFi.macAddress();
    uniqueId.replace(":", "");
    uniqueId.toLowerCase();
    _uniqueId = uniqueId;

    _device = new HADevice(_uniqueId.c_str());

    Log.printf("Setup Ha device\n\r");
    _device->setName("CSR Wemos Driver-dev2");
    _device->setSoftwareVersion(VERSION);
    
    _mqtt = new HAMqtt(_wifiClient, *_device);


    Log.printf("Configurating entities...\n\r");

    _networkFrequencyEntity.setIcon("mdi:sine-wave");
    _networkFrequencyEntity.setName("Network frequency");
    _networkFrequencyEntity.setUnitOfMeasurement("Hz");
    _networkFrequencyEntity.setDeviceClass("FREQUENCY");
    _mqtt->addDeviceType(&_networkFrequencyEntity);

    _uptimeSensorEntity.setIcon("mdi:timer-outline");
    _uptimeSensorEntity.setName("Uptime");
    _uptimeSensorEntity.setUnitOfMeasurement("s");
    _mqtt->addDeviceType(&_uptimeSensorEntity);

    _powerSetPoint.onCommand(CustomHaDevice::onSetPointCommand);
    _powerSetPoint.setIcon("mdi:flash");
    _powerSetPoint.setName("Power SetPoint2");
    _powerSetPoint.setUnitOfMeasurement("W");
    _powerSetPoint.setDeviceClass("POWER");
    _powerSetPoint.setMin(0);
    _powerSetPoint.setMax(4000);
    _setPointValue = 0.f;
    _mqtt->addDeviceType(&_powerSetPoint);

    //_powerSetPoint->setAvailability(true);

    _testNumber.onCommand(CustomHaDevice::onSetPointCommand);
    _testNumber.setIcon("mdi:home");
    _testNumber.setName("My number");
    _mqtt->addDeviceType(&_testNumber);

    _last_report = millis();

    Log.printf("Connecting to MQTT...\n\r");
    _mqtt->begin(BROKER_ADDR, BROKER_USER, BROKER_PASSWORD);
    Log.printf("Ha Device setup finished.\n\r");
}

void CustomHaDevice::loop()
{
    _mqtt->loop();
    if (millis() - _last_report < 5 * 1000)
        return;

    Log.printf("Reporting network Frequency value: %f\n\r", _networkFrequency);
    _networkFrequencyEntity.setValue(_networkFrequency);
    _uptimeSensorEntity.setValue(millis() / 1000.f);
    _last_report = millis();
}

float CustomHaDevice::setPoint() const
{
    return _setPointValue;
}

void CustomHaDevice::networkFrequency(float value)
{
    _networkFrequency = value;
}

void CustomHaDevice::onSetPointCommand(HANumeric number, HANumber* sender)
{
    if (!number.isSet())
        HaDevice._setPointValue = 0;
    HaDevice._setPointValue = number.toFloat();
    Log.printf("onSetPointCommand, setpoint set to %f -> %f\n\r", sender->getCurrentState().toFloat(), HaDevice._setPointValue);
    sender->setState(HaDevice._setPointValue);
}

#if !defined(NO_GLOBAL_INSTANCES) 
CustomHaDevice HaDevice;
#endif