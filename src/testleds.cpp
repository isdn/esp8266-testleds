#include "Arduino.h"

#include <FastLED.h>

#define NUM_LEDS 5
#define DATA_PIN 2 // D4 == GPIO2
#define BUTTON_PIN 0 // D3 == GPIO0

CRGB leds[NUM_LEDS];

const unsigned short int debounceDelay = 100;
volatile boolean buttonPressed = false;
volatile unsigned long lastDebounceTime = 0;
unsigned short int mode = 0;

ICACHE_RAM_ATTR void processButton() {
    unsigned long currentTime = millis();
    if ((currentTime - lastDebounceTime) > debounceDelay) {
        buttonPressed = true;
        lastDebounceTime = currentTime;
    }
}

void setup() {
    pinMode(BUTTON_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), processButton, FALLING);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void setLEDOff(CRGB *led) {
    led->setColorCode(0);
    FastLED.show();
}

void setLEDOn(CRGB *led) {
    led->setColorCode(0xFFFFFF);
    FastLED.show();
}

void setLEDBlink(CRGB *led) {
    setLEDOn(led);
    delay(100);
    setLEDOff(led);
    delay(50);
}

void setLEDRGBW(CRGB *led) {
    unsigned int color = 0xFF0000; // 0xRRGGBB
    for (int i=0; i<3; i++) {
        led->setColorCode(color);
        FastLED.show();
        delay(300);
        color >>= 8u; // 0xFF0000 -> 0x00FF00 -> 0x0000FF
    }
    led->setColorCode(~color); // 0xFFFFFF
    FastLED.show();
    delay(300);
    led->setColorCode(color); // 0x000000
    FastLED.show();
}

void setLEDBrightness(CRGB *led) {
    const unsigned short int step = 10;
    for (int i=0; i<256; i+=step) {
        led->setRGB(i, i, i);
        FastLED.show();
        delay(10);
    }
    led->setColorCode(0xFFFFFF);
    FastLED.show();
    delay(10);
    for (int i=255; i>0; i-=step) {
        led->setRGB(i, i, i);
        FastLED.show();
        delay(10);
    }
    led->setColorCode(0);
    FastLED.show();
    delay(10);
}

void loop() {
    if (buttonPressed) {
        if (mode < 4) {
            ++mode;
        } else {
            mode = 0;
        }
        buttonPressed = false;
    }
    switch (mode) {
        case 0: {
            for (CRGB &led : leds) {
                if (buttonPressed) break;
                setLEDOff(&led);
            }
            break;
        }
        case 1: {
            FastLED.setBrightness(100);
            for (CRGB &led : leds) {
                if (buttonPressed) break;
                setLEDBlink(&led);
            }
            break;
        }
        case 2: {
            FastLED.setBrightness(100);
            for (CRGB &led : leds) {
                if (buttonPressed) break;
                setLEDBrightness(&led);
            }
            break;
        }
        case 3: {
            FastLED.setBrightness(1);
            for (CRGB &led : leds) {
                if (buttonPressed) break;
                setLEDRGBW(&led);
            }
            break;
        }
        case 4: {
            FastLED.setBrightness(100);
            for (CRGB &led : leds) {
                if (buttonPressed) break;
                setLEDOn(&led);
            }
            break;
        }
        default: break;
    }
}