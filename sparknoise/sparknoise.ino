/*
  Play an MP3 over software serial using the MY1690X MP3 IC
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 10th, 2021
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  The MY1690 has a large number of features. This example presents the user
  with a serial menu to control the various aspects of the IC.

  Feel like supporting our work? Buy a board from SparkFun!
  MY1690X Serial MP3 Player Shield: https://www.sparkfun.com/sparkfun-serial-mp3-player-shield-my1690x.html
  MY1690X Audio Player Breakout: https://www.sparkfun.com/sparkfun-audio-player-breakout-my1690x-16s.html

  Hardware Connections:
  MY1690 Pin -> Arduino Pin
  -------------------------------------
  TXO -> 8
  RXI -> 9
  VIN -> 5V
  GND -> GND

  Don't forget to load some MP3s on your sdCard and plug it in too!
  Note: Track must be named 0001.mp3 to myMP3.playTrackNumber(1)
*/

#include <Bounce2.h>
#include "SparkFun_MY1690_MP3_Library.h"
#include <SoftwareSerial.h>

// Pins
const int playButtonPin = 2;
const int volUpButtonPin = 3;
const int volDownButtonPin = 7;
const int nextTrackPin = 10;
const int prevTrackPin = 11;

// Bounce objects
Bounce playButton = Bounce();
Bounce volUpButton = Bounce();
Bounce volDownButton = Bounce();
Bounce nextTrackButton = Bounce();
Bounce prevTrackButton = Bounce();

// MP3 player
SoftwareSerial serialMP3(8, 9);
SparkFunMY1690 myMP3;

// Variables
unsigned long nextTrackButtonHoldStart = 0; // Tracks when the button was first pressed
unsigned long prevTrackButtonHoldStart = 0; // Tracks when the button was first pressed
static unsigned long lastPrevTrackPressTime = 0; // Tracks the last button press time
static int prevTrackPressCount = 0; // Tracks the number of button presses

void setup() {
  Serial.begin(115200);

  // Initialize buttons
  playButton.attach(playButtonPin, INPUT_PULLUP);
  volUpButton.attach(volUpButtonPin, INPUT_PULLUP);
  volDownButton.attach(volDownButtonPin, INPUT_PULLUP);
  nextTrackButton.attach(nextTrackPin, INPUT_PULLUP);
  prevTrackButton.attach(prevTrackPin, INPUT_PULLUP);

  playButton.interval(50);
  volUpButton.interval(50);
  volDownButton.interval(50);
  nextTrackButton.interval(50);
  prevTrackButton.interval(50);

  // Initialize MP3 player
  serialMP3.begin(9600);
  if (myMP3.begin(serialMP3) == false) {
    Serial.println(F("Device not detected. Check wiring. Freezing."));
    while (1);
  }

  myMP3.setVolume(15);
  myMP3.setPlayModeFull(); // After song finishes the next will play.
  mainMenu();
}

void loop() {
  // Update button states
  playButton.update();
  volUpButton.update();
  volDownButton.update();
  nextTrackButton.update();
  prevTrackButton.update();

  // Check for button presses
  if (playButton.fell()) {
    Serial.print(F("Track Number: "));
    Serial.println((myMP3.getTrackNumber()));
    myMP3.playPause();
    Serial.println(F("Play/Pause toggled"));
  }

  if (volUpButton.fell()) {
    myMP3.volumeUp();
    Serial.print(F("Volume: "));
    Serial.println(myMP3.getVolume());
  }

  if (volDownButton.fell()) {
    myMP3.volumeDown();
    Serial.print(F("Volume: "));
    Serial.println(myMP3.getVolume());
  }

  // Handle nextTrackButton hold and press
  static bool fastForwardTriggered = false; // Tracks if fast forward was triggered
  static unsigned long lastFastForwardTime = 0; // Tracks the last time fastForward() was called

  if (nextTrackButton.read() == LOW) { // Button is pressed
    if (nextTrackButtonHoldStart == 0) {
      nextTrackButtonHoldStart = millis(); // Record the time when the button was pressed
    } else if (millis() - nextTrackButtonHoldStart >= 1000) { // Check if held for 1 second
      if (!fastForwardTriggered) {
        fastForwardTriggered = true; // Mark fast forward as triggered
        lastFastForwardTime = millis(); // Initialize the timer for repeated calls
        myMP3.fastForward();
        Serial.println(F("Fast forwarding"));
      } else if (millis() - lastFastForwardTime >= 500) { // Repeat every 500ms
        lastFastForwardTime = millis();
        myMP3.fastForward();
        Serial.println(F("Fast forwarding"));
      }
    }
  } else { // Button is released
    if (!fastForwardTriggered && nextTrackButtonHoldStart != 0 && millis() - nextTrackButtonHoldStart < 1000) {
      Serial.print(F("Current Track Number: "));
      Serial.println((myMP3.getTrackNumber()));
      myMP3.playNext();
      Serial.println(F("Playing next track..."));
      delay(500); // wait for the track to load before getting the number
      Serial.print(F("Track number is now: "));
      Serial.println((myMP3.getTrackNumber()));
    }
    // Reset variables
    nextTrackButtonHoldStart = 0;
    fastForwardTriggered = false;
  }

  // Handle prevTrackButton single press, double press, and hold
  static bool rewindTriggered = false; // Tracks if rewind was triggered
  static unsigned long lastRewindTime = 0; // Tracks the last time rewind() was called

  if (prevTrackButton.read() == LOW) { // Button is pressed
    if (prevTrackButtonHoldStart == 0) {
      prevTrackButtonHoldStart = millis(); // Record the time when the button was pressed
    }
  } else { // Button is released
    if (prevTrackButtonHoldStart != 0 && millis() - prevTrackButtonHoldStart < 1000) {
      // Button was pressed and released within 1 second
      unsigned long currentTime = millis();
      if (currentTime - lastPrevTrackPressTime <= 1500) { // Check for double press within 1.5 seconds
        prevTrackPressCount++;
      } else {
        prevTrackPressCount = 1; // Reset press count if more than 1.5 seconds has passed
      }
      lastPrevTrackPressTime = currentTime;

      if (prevTrackPressCount == 1) {
        // Single press: Restart the current track
        uint16_t currentTrackNum = myMP3.getTrackNumber(); // Get the current track number
        Serial.print(F("Single Press. Restarting current track: "));
        Serial.println(currentTrackNum);
        myMP3.playPrevious();
        myMP3.playNext();
      } else if (prevTrackPressCount == 2) {
        // Double press: Go to the previous track
        Serial.print(F("Double press. Playing previous track: "));
        myMP3.playPrevious();
        delay(300);
        uint16_t currentTrackNum = myMP3.getTrackNumber(); // Get the current track number
        Serial.println(currentTrackNum);
        prevTrackPressCount = 0; // Reset press count after double press
      }
    } else if (millis() - prevTrackButtonHoldStart >= 1000) { // Check if held for 1 second
      if (!rewindTriggered) {
        rewindTriggered = true; // Mark rewind as triggered
        lastRewindTime = millis(); // Initialize the timer for repeated rewinding
        if (millis() - lastRewindTime >= 500) { // Rewind every 500ms
          lastRewindTime = millis();
          myMP3.rewind();
          Serial.println(F("Rewinding..."));
        }
      }
    }
    // Reset variables
    prevTrackButtonHoldStart = 0;
    rewindTriggered = false;
  }
}

void mainMenu() {
  Serial.println();
  Serial.println(F("SparkFun MY1690 Menu:"));
  Serial.println(F("s) Stop play"));
  Serial.println(F("x) Reset IC"));
  Serial.println(F("a) Volume up"));
  Serial.println(F("z) Volume down"));
  Serial.println(F("f) Fast forward"));
  Serial.println(F("r) Reverse"));
  Serial.println(F("p) Play/Pause toggle"));
  Serial.println(F("l) Set loop mode"));
  Serial.println(F("e) Set EQ"));
  Serial.println(F("m) Set play mode"));
  Serial.println(F("<) Play previous"));
  Serial.println(F(">) Play next"));
  Serial.println(F("#) Play track number"));
  Serial.println(F("c) Current track number"));
  Serial.println(F("t) Track elapsed time"));
  Serial.println(F("T) Track total time"));
  Serial.println(F("Enter command:"));
}