/*************************************************** 
  This is an example for the Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// include SPI, MP3 and SD libraries. Woo hoo!
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// These are the pins used for the breakout example
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  //Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
  // create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

////

// Setting up Pushbuttons Hooray!
const int playButtonPin = 2;  // the number of the PLAY pushbutton pin
const int forwardButtonPin = 3;  // the number of the FWD pushbutton pin
const int backButtonPin = 4;  // the number of the BACK pushbutton pin
const int volUpButtonPin = 5;  // the number of the VOL UP pushbutton pin
const int volDownButtonPin = 6;  // the number of the VOL DOWN pushbutton pin

int playButtonState = 0;  // variable for reading the pushbutton status
int playButtonPressed = 0;  // variable for holding state of the pushbutton 

int forwardButtonState = 0;  // variable for reading the pushbutton status
int forwardButtonPressed = 0;  // variable for holding state of the pushbutton 

int backButtonState = 0;  // variable for reading the pushbutton status
int backButtonPressed = 0;  // variable for holding state of the pushbutton 

int volUpButtonState = 0;  // variable for reading the pushbutton status
int volUpButtonPressed = 0;  // variable for holding state of the pushbutton 

int volDownButtonState = 0;  // variable for reading the pushbutton status
int volDownButtonPressed = 0;  // variable for holding state of the pushbutton 

int volValue = 20; // variable for default volume value
int volStep = 5;  // variable for amount we should step the vol up or down when button is pressed


////

void setup() {
  Serial.begin(9600);
 
  // * Button Setups *
  // initialize our pushbutton pins as inputs:
  pinMode(playButtonPin, INPUT);
  pinMode(forwardButtonPin, INPUT);
  pinMode(backButtonPin, INPUT);
  pinMode(volUpButtonPin, INPUT);
  pinMode(volDownButtonPin, INPUT);
  

  Serial.println("Adafruit VS1053 Library Test");

  // initialise the music player
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
 
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  
  // list files
  // printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(volValue,volValue);

  /***** Two interrupt options! *******/ 
  // This option uses timer0, this means timer1 & t2 are not required
  // (so you can use 'em for Servos, etc) BUT millis() can lose time
  // since we're hitchhiking on top of the millis() tracker
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT);
  
  // This option uses a pin interrupt. No timers required! But DREQ
  // must be on an interrupt pin. For Uno/Duemilanove/Diecimilla
  // that's Digital #2 or #3
  // See http://arduino.cc/en/Reference/attachInterrupt for other pins
  // *** This method is preferred
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));
}

void loop() {  
  // Alternately, we can just play an entire file at once
  // This doesn't happen in the background, instead, the entire
  // file is played and the program will continue when it's done!
  // musicPlayer.playFullFile("track001.ogg");

    playButtonState = digitalRead(playButtonPin);
    // Prints 1 when pushed, otherwise 0
    // Serial.println(digitalRead(playButtonPin));


  // GPIO1 is the Play button
  // if (musicPlayer.GPIO_digitalRead(2) == HIGH) {

  if (playButtonState == HIGH) {
      Serial.println("Button pushed!");
      playButtonPressed = 1;
      // Start playing a file, then we can do stuff while waiting for it to finish
      if (! musicPlayer.startPlayingFile("/track001.mp3")) {
        Serial.println("Could not open file track001.mp3");
        // this will hold the code in a "endless loop" until the board is reset.
        while (1);
      }
      Serial.println(F("Started playing"));

      while (musicPlayer.playingMusic) {
        // file is now playing in the 'background' so now's a good time
        // to do something else like handling LEDs or buttons :)
        // Serial.print(".");
        // delay(1000);
        // volume();
        //delay(1000);

         if (forwardButtonPin == HIGH) {
            Serial.println(F("FWD button pressed"));
            // Code should go here to skip to the next track
         }

         if (backButtonPin == HIGH) {
            Serial.println(F("BACK button pressed"));
            // Code should go here to restart the track
            // TODO: Figure out how to catch two button presses to go back to previous track.
         }

         if (volUpButtonPin == HIGH) {
            Serial.println(F("VOL UP button pressed"));
            // Increase the volume (lower the number).
            // TODO: Figure out how to see if we are hitting the threshold number. If so, Beep and Do not exceed.
            volValue = volValue - volStep;
            musicPlayer.setVolume(volValue,volValue);
            Serial.print(F("VOL is now "));
            Serial.println(volValue);
         }

         if (volDownButtonPin == HIGH) {
            Serial.println(F("VOL DOWN button pressed"));
            // Decrease the volume (raise the number).
            volValue = volValue + volStep;
            musicPlayer.setVolume(volValue,volValue);
            Serial.print(F("VOL is now "));
            Serial.println(volValue);
         }


      }
      playButtonPressed = 0;
  }

  // Serial.println("Done playing music");

}


/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

