#ifndef MusicAndLightFX_h
#define MusicAndLightFX_h
// PMSG v4 - Ces25 demo

#include <Arduino.h>
#include <FastLED.h>

// Function declarations for music effects
void playStarWars(int motorPin);
void playKnightRider(int motorPin);
void playNintendo(int motorPin);
void playJetFighter(int motorPin);  // Jet Fighter effect
void playRadar(int motorPin);      // Radar effect
void playSonar(int motorPin);      // Sonar effect

// Function declarations for LED animations
void rainbowCycle(CRGB leds[], int numLeds, int delayTime);
void knightRiderAnimation(CRGB leds[], int numLeds, int delayTime);
void fadeInOut(CRGB leds[], int numLeds, CRGB color, int delayTime);
void directionAnimation(CRGB leds[], int numLeds, const char* direction);
void warnAnimation(CRGB leds[], int numLeds, int motorPin);

#endif