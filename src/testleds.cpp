#include "Arduino.h"

#include <FastLED.h>

#define NUM_LEDS 5
#define DATA_PIN 2 // D4 == GPIO2
#define BUTTON_PIN 0 // D3 == GPIO0

#define USE_BUTTON 1
//#define SIMPLE_MODE 1

CRGB leds[NUM_LEDS];

const uint8_t debounceDelay = 200;
volatile bool buttonPressed = false;
volatile unsigned long lastDebounceTime = 0;
uint8_t mode = 0;

#ifdef USE_BUTTON
ICACHE_RAM_ATTR void processButton() {
    unsigned long currentTime = millis();
    if ((currentTime - lastDebounceTime) > debounceDelay) {
        buttonPressed = true;
        lastDebounceTime = currentTime;
    }
}
#endif

void setup() {
#ifdef USE_BUTTON
    pinMode(BUTTON_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), processButton, FALLING);
#endif
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void turnOffLEDs() {
    for (CRGB &led : leds) {
        led.setColorCode(0);
    }
    FastLED.show();
}

void fireRGBW(int d) {
    unsigned int color = 0xFF0000; // 0xRRGGBB
    for (uint8_t c=0; c<3; c++) {
        for (CRGB &led : leds) {
            led.setColorCode(color);
        }
#ifdef USE_BUTTON
        if (buttonPressed) return;
#endif
        FastLED.show();
        delay(d);
        color >>= 8u; // 0xFF0000 -> 0x00FF00 -> 0x0000FF
    }
    for (CRGB &led : leds) {
        led.setColorCode(~color); // 0xFFFFFF
    }
#ifdef USE_BUTTON
    if (buttonPressed) return;
#endif
    FastLED.show();
    delay(d);
    for (CRGB &led : leds) {
        led.setColorCode(color); // 0x000000
    }
    FastLED.show();
    delay(100);
}

void setLEDsBrightness(int d) {
    const uint8_t step = 5;
    for (uint8_t c=0; c<255; c+=step) {
        for (CRGB &led : leds) {
            led.setRGB(c, c, c);
        }
#ifdef USE_BUTTON
        if (buttonPressed) return;
#endif
        FastLED.show();
        delay(10);
    }
    delay(d);
    for (uint8_t c=255; c>0; c-=step) {
        for (CRGB &led : leds) {
            led.setRGB(c, c, c);
        }
#ifdef USE_BUTTON
        if (buttonPressed) return;
#endif
        FastLED.show();
        delay(10);
    }
    turnOffLEDs();
    delay(d);
}

void fireOneByOne() {
    for (CRGB &led : leds) {
#ifdef USE_BUTTON
        if (buttonPressed) return;
#endif
        led.setColorCode(0xFFFFFF);
        FastLED.show();
        delay(100);
        led.setColorCode(0);
        FastLED.show();
        delay(50);
    }
}

void turnOnOneByOne(int d) {
    for (CRGB &led : leds) {
#ifdef USE_BUTTON
        if (buttonPressed) return;
#endif
        led.setColorCode(0xFFFFFF);
        FastLED.show();
        delay(100);
    }
    delay(d);
    for (CRGB &led : leds) {
#ifdef USE_BUTTON
        if (buttonPressed) return;
#endif
        led.setColorCode(0);
        FastLED.show();
        delay(100);
    }
}

void loop() {
#ifdef SIMPLE_MODE
    uint8_t steps = 2;
#else
    uint8_t steps = 4;
#endif
#ifdef USE_BUTTON
    if (buttonPressed) {
            if (mode < steps) {
            ++mode;
        } else {
            mode = 0;
        }
        buttonPressed = false;
    }
#else
    if (mode < steps) {
        mode++;
    } else {
        mode = 0;
    }
#endif

    switch (mode) {
        case 0: {
            turnOffLEDs();
            break;
        }
        case 1: {
            fireRGBW(1000);
            break;
        }
        case 2: {
            setLEDsBrightness(1000);
            break;
        }
#ifndef SIMPLE_MODE
        case 3: {
            fireOneByOne();
            break;
        }
        case 4: {
            turnOnOneByOne(500);
            break;
        }
#endif
        default: break;
    }
}