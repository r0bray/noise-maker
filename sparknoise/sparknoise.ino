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
const int volDownButtonPin = 5;
const int nextTrackPin = 6;
const int prevTrackPin = 7;

// Bounce objects
Bounce playButton = Bounce();
Bounce volUpButton = Bounce();
Bounce volDownButton = Bounce();
Bounce nextTrackButton = Bounce();
Bounce prevTrackButton = Bounce();

// MP3 player
SoftwareSerial serialMP3(8, 9);
SparkFunMY1690 myMP3;

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
  myMP3.setPlayModeNoLoop();
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

  if (nextTrackButton.fell()) {
    myMP3.playNext();
    Serial.println(F("Playing next track"));
  }

  if (prevTrackButton.fell()) {
    myMP3.playPrevious();
    Serial.println(F("Playing previous track"));
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