#include "MusicAndLightFX.h"
#include <FastLED.h>

// Jet Fighter Effect: Pitch goes up and down to simulate jet engine
void playJetFighter(int motorPin) {
  const int minFrequency = 100;
  const int maxFrequency = 1500;
  const int rampDuration = 10; // Delay between frequency steps

  // Ramp up
  for (int freq = minFrequency; freq <= maxFrequency; freq += 20) {
    analogWriteFrequency(motorPin, freq);
    analogWrite(motorPin, 128); // 50% duty cycle
    delay(rampDuration);
  }

  // Ramp down
  for (int freq = maxFrequency; freq >= minFrequency; freq -= 20) {
    analogWriteFrequency(motorPin, freq);
    analogWrite(motorPin, 128); // 50% duty cycle
    delay(rampDuration);
  }

  analogWrite(motorPin, 0); // Turn off motor
}

// Radar Effect: Periodic pings at a fixed frequency
void playRadar(int motorPin) {
  const int radarFrequency = 400;
  const int pulseDuration = 200; // Duration of each pulse

  for (int i = 0; i < 5; i++) { // Five radar pings
    analogWriteFrequency(motorPin, radarFrequency);
    analogWrite(motorPin, 128); // 50% duty cycle
    delay(pulseDuration);
    analogWrite(motorPin, 0); // Turn off motor
    delay(pulseDuration);     // Pause between pulses
  }
}

// Sonar Effect: Slow frequency ramping up and down
void playSonar(int motorPin) {
  const int minFrequency = 200;
  const int maxFrequency = 800;
  const int stepDelay = 50; // Delay between frequency steps

  // Ramp up
  for (int freq = minFrequency; freq <= maxFrequency; freq += 10) {
    analogWriteFrequency(motorPin, freq);
    analogWrite(motorPin, 128); // 50% duty cycle
    delay(stepDelay);
  }

  // Ramp down
  for (int freq = maxFrequency; freq >= minFrequency; freq -= 10) {
    analogWriteFrequency(motorPin, freq);
    analogWrite(motorPin, 128); // 50% duty cycle
    delay(stepDelay);
  }

  analogWrite(motorPin, 0); // Turn off motor
}

// Function definition
void rainbowCycle(CRGB leds[], int numLeds, int delayTime) {
  for (int j = 0; j < 256; j++) { // Cycle through all colors
    for (int i = 0; i < numLeds; i++) {
      leds[i] = CHSV((i * 256 / numLeds + j) & 255, 255, 255);
    }
    FastLED.show();
    delay(delayTime);
  }
}

// Play Star Wars tune
void playStarWars(int motorPin) {
  int melody[] = {500, 500, 500, 350, 150, 500, 350, 150, 1000};
  int pauses[] = {50, 50, 50, 50, 50, 50, 50, 50, 150};

  for (int i = 0; i < 9; i++) {
    digitalWrite(motorPin, HIGH);
    delay(melody[i]);
    digitalWrite(motorPin, LOW);
    delay(pauses[i]);
  }
}

// Play Knight Rider tune
void playKnightRider(int motorPin) {
  int melody[] = {200, 200, 200, 200, 200, 200};
  int pauses[] = {100, 100, 100, 100, 100, 100};

  for (int i = 0; i < 6; i++) {
    digitalWrite(motorPin, HIGH);
    delay(melody[i]);
    digitalWrite(motorPin, LOW);
    delay(pauses[i]);
  }
}

// Direction animation (Left, Right, Stop) using RGB colors
void directionAnimation(CRGB leds[], int numLeds, const char* direction) {
  FastLED.clear();
  if (strcmp(direction, "left") == 0) {
    for (int i = numLeds - 1; i >= 0; i--) {
      leds[i] = CRGB::Blue;  // Blue for left
      FastLED.show();
      delay(100);
      leds[i] = CRGB::Black;
    }
  } else if (strcmp(direction, "right") == 0) {
    for (int i = 0; i < numLeds; i++) {
      leds[i] = CRGB::Green;  // Green for right
      FastLED.show();
      delay(100);
      leds[i] = CRGB::Black;
    }
  } else if (strcmp(direction, "stop") == 0) {
    for (int i = 0; i < numLeds; i++) {
      leds[i] = CRGB::Red;  // Red for stop
    }
    FastLED.show();
    delay(1000);
    FastLED.clear();
    FastLED.show();
  }
}

// Warn animation with vibration
void warnAnimation(CRGB leds[], int numLeds, int motorPin) {
  for (int i = 0; i < numLeds; i++) {
    leds[i] = CRGB::Yellow;  // Yellow for warning
  }
  FastLED.show();

  // Motor vibration pattern
  for (int i = 0; i < 3; i++) {
    digitalWrite(motorPin, HIGH);
    delay(300);
    digitalWrite(motorPin, LOW);
    delay(300);
  }

  FastLED.clear();
  FastLED.show();
}

// Knight Rider Animation
void knightRiderAnimation(CRGB leds[], int numLeds, int delayTime) {
  // Forward animation
  for (int i = 0; i < numLeds; i++) {
    leds[i] = CRGB::Red; // Red for Knight Rider effect
    FastLED.show();
    delay(delayTime);
    leds[i] = CRGB::Black; // Turn off LED after the effect
  }

  // Backward animation
  for (int i = numLeds - 2; i >= 0; i--) { // Skip the last LED to prevent duplicate animation
    leds[i] = CRGB::Red; // Red for Knight Rider effect
    FastLED.show();
    delay(delayTime);
    leds[i] = CRGB::Black; // Turn off LED after the effect
  }
}