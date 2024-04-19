#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ArduinoHA.h>
#include "settings.h"

#include "NetworkManager.h"

#include <TLog.h>
#include <TelnetSerialStream.h>
#include "CustomHaDevice.h"

unsigned long zeroAt;
unsigned long lastZeroAt;

// (80Mhz frequence ESP8266) 1 cycle = 12.5ns => 1 µ seconde / 12.5 ns = 80 
#define US_TO_TICK  5
unsigned long numPsec        = 312600; 
#define HIGH_CSR_DURATION_TIME  500 //µs
#define MIN_PWM_TIME 100.f //µs


#define ZEROES_SAMPLE   20
unsigned int zeroes[ZEROES_SAMPLE];
int zeroesPos;
float halfNetworkPeriod_ms;
float timerExpiration;

void zeroCrossCallback();
void onTimerInterrupt();

bool csr_low;
int nbIt = 0;
bool managePower = false;

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
    
    HaDevice.setup();
    
    pinMode(SCR_PIN, OUTPUT);
    digitalWrite(SCR_PIN, LOW);

    Log.printf("Configuring IT...\n\r");

    //disable interrupts
    noInterrupts();

    attachInterrupt(digitalPinToInterrupt(ZERO_CROSS_PIN), zeroCrossCallback, CHANGE);

    timer1_isr_init();
    timer1_attachInterrupt(onTimerInterrupt);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);

    csr_low = true;

    //enable interrupts
    interrupts();

    Network.onOTAStart([&]() {
        // we prevent IT and cut power
        timer1_disable();
        managePower = false;
        digitalWrite(SCR_PIN, LOW);
    });

    managePower = true;
    Log.printf("Setup finished.\n\r");
}

ICACHE_RAM_ATTR 
void zeroCrossCallback()
{
    if (managePower)
    {
        lastZeroAt = zeroAt;
        zeroAt = micros();
        zeroes[zeroesPos] = zeroAt - lastZeroAt;
        zeroesPos++;
        if (zeroesPos >= ZEROES_SAMPLE)
            zeroesPos = 0;

        float zeroFreq = 0.f;
        for (int i = 0; i < ZEROES_SAMPLE; ++i)
            zeroFreq += (float)zeroes[i];

        zeroFreq = zeroFreq / (float)ZEROES_SAMPLE;
        halfNetworkPeriod_ms = zeroFreq;

        //we compute the time when the timer should fire
        float pwm = HaDevice.setPoint();
        if (pwm > 10)
        {
            pwm = std::max(0.f, std::min(100.f, pwm));
            //100% of pwm must fire in halfNetworkPeriod_ms µs
            pwm = halfNetworkPeriod_ms * (100.f - pwm) / 100.f;
            // we have to take into account the high time of the csr in the max value we can set to let the high time in the allowed time
            pwm = min(pwm, halfNetworkPeriod_ms);
            pwm = max(pwm, MIN_PWM_TIME);
            timerExpiration = pwm;
            
            digitalWrite(SCR_PIN, LOW);
            csr_low = true;
            //managed a 100% pwm requested
            timer1_write(US_TO_TICK * pwm);
        }
    }
}
 
void onTimerInterrupt(void)
{
    noInterrupts();
    if (managePower)
    {
        nbIt++;
        //digitalWrite(SCR_PIN, HIGH);
        //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

        if (csr_low)
        {
            //digitalWrite(LED_BUILTIN, HIGH);
            digitalWrite(LED_BUILTIN, LOW);

            //we have to set CSR pin to high
            csr_low = false;
            //we set the CSR pin
            digitalWrite(SCR_PIN, HIGH);

            //we clear the csr pin just after a little time
            timer1_write(US_TO_TICK * HIGH_CSR_DURATION_TIME);
        }
        else
        {
            csr_low = true;
            digitalWrite(LED_BUILTIN, HIGH);

            //we clear the CSR pin
            digitalWrite(SCR_PIN, LOW);
            //we don't re-arm the timer here
        }
    }
    interrupts(); 
}

void loop()
{
    Log.loop();
    Network.handle();
    HaDevice.loop();

    static unsigned long last_report = millis();
    if (millis() - last_report < 1 * 20)
        return;
    if (halfNetworkPeriod_ms != 0)
    {
        float freq = 1.f / (2.f * halfNetworkPeriod_ms / 1000000.f);
        HaDevice.networkFrequency(freq);
    }
    // Log.printf("Half Period of zero cross: %f\n\r", halfNetworkPeriod_ms);
    // Log.printf("timerExpiration: %f\n\r", timerExpiration);
    // Log.printf("computation: %f\n\r", computation);
    // Log.printf("nbIt: %d\n\r", nbIt);
    // Log.printf("setPoint: %f\n\r", HaDevice.setPoint());

    last_report = millis();
}
