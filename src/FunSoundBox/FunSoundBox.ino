// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <SD.h>
#include "Adafruit_VS1053.h"
#include "Playlist.h"
#include "SimpleRandom.h"

// define the pins used
//#define CLK 13       // SPI Clock, shared with SD card
//#define MISO 12      // Input data, from VS1053/SD card
//#define MOSI 11      // Output data, to VS1053/SD card
// Connect CLK, MISO and MOSI to hardware SPI pins. 
// See http://arduino.cc/en/Reference/SPI "Connections"
// These are common pins between breakout and shield
#define DREQ             3      // VS1053 Data request, ideally an Interrupt pin
#define CARDCS           4     // Card chip select pin

#define BREAKOUT_DCS     8      // VS1053 Data/command select pin (output)
#define BREAKOUT_RESET   9      // VS1053 reset pin (output)
#define BREAKOUT_CS      10     // VS1053 chip select pin (output)

#define STATUS_LED       5
#define BUTTON_ONE       6       //start / next normal playlist
#define BUTTON_TWO       7       // start / next raunchy playlist
#define VOLUME_INPUT     A0
#define VOLUME_TOLERANCE 31

Adafruit_VS1053_FilePlayer musicPlayer =
        Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);

Playlist defaultPlaylist;

int volumeInputValue = 1024;
String nextFileToPlay;

void setup() {
    pinMode(STATUS_LED, OUTPUT);
    pinMode(BUTTON_ONE, INPUT);
    pinMode(BUTTON_TWO, INPUT);

    digitalWrite(STATUS_LED, LOW);

    Serial.begin(9600);
    //printlnToSerial("Initializing");

    while (!musicPlayer.begin()) { // initialise the music player
        printlnToSerial(F("Couldn't find VS1053, do you have the right pins defined?"));
        Blink(STATUS_LED, 3, 500, 500);
    }

    //printlnToSerial("VS1053 found");

    musicPlayer.setVolume(0, 255);

    while (!musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT)) {
        printlnToSerial(F("DREQ is not on an interrupt pin"));
        Blink(STATUS_LED, 2, 200, 200);
    }

    //Play success initialization tone
    musicPlayer.sineTest(50, 500);

    // initialise the SD card
    while (!SD.begin(CARDCS)) {
        printlnToSerial(F("Couldn't initialise SD card"));
        //If the sd card could not be initialised beep twice and blink
        musicPlayer.sineTest(0x44, 500);
        Blink(STATUS_LED, 5, 200, 400);
    }

    Ruzzie::SimpleRandom simpleRandom = Ruzzie::SimpleRandom((long) analogRead(VOLUME_INPUT));

    defaultPlaylist.Initialize("/", simpleRandom);
    nextFileToPlay = defaultPlaylist.NextRandomFilename();
    digitalWrite(STATUS_LED, HIGH);

    const char *initFileNameToPlay = "INIT.MP3";
    if (SD.exists(initFileNameToPlay)) {
        //printlnToSerial("INIT.MP3 found start playing.");
        musicPlayer.playFullFile(initFileNameToPlay);
    }
}

void Blink(const uint8_t &port, const uint8_t times, const unsigned int intervalInMs, const unsigned int durationInMs) {
    for (uint8_t i = 0; i < times; ++i) {
        digitalWrite(port, HIGH);
        delay(durationInMs);
        digitalWrite(port, LOW);
        delay(intervalInMs);
    }
}

void printlnToSerial(const char *text) {
    if (Serial) {
        Serial.println(text);
    }
}

void printlnToSerial(const String &text) {
    if (Serial) {
        Serial.println(text);
    }
}

void printlnToSerial(const __FlashStringHelper *ifsh) {
    if (Serial) {
        Serial.println(ifsh);
    }
}

const uint8_t volumeShiftFactor = 2;

void loop() {
    /*Tweak delays to fine-tune interaction*/
    int currentVolumeInputValue = analogRead(VOLUME_INPUT);
    if (abs(currentVolumeInputValue - volumeInputValue) > VOLUME_TOLERANCE) {
        digitalWrite(STATUS_LED, LOW);
        volumeInputValue = currentVolumeInputValue;
        uint8_t volume = 0;
        if (volumeInputValue > 0) {
            volume = 255 - (volumeInputValue >> volumeShiftFactor) + 1; //divide by 4 (right shift 2) in this case
        }
        musicPlayer.setVolume(volume, 255);//we only use the left channel for now
        //printlnToSerial("Volume raw: " + String(volumeInputValue));
        //printlnToSerial("Volume calculated: " + String(volume));
        digitalWrite(STATUS_LED, HIGH);
        delay(20);
    }

    bool shouldGetNextFileButtonOne = ButtonPollOrAction(BUTTON_ONE, STATUS_LED, nextFileToPlay);
    bool shouldGetNextFileButtonTwo = ButtonPollOrAction(BUTTON_TWO, STATUS_LED, nextFileToPlay);

    //Use the idle time to get the next random file when needed
    //Todo: Check if this works, since another file maybe playing at the moment (and thus opened by the SD)
    if (shouldGetNextFileButtonOne || shouldGetNextFileButtonTwo) {
        nextFileToPlay = defaultPlaylist.NextRandomFilename();
        delay(1);//Todo: tweak this
    } else {
        delay(5);
    }
}

bool ButtonPollOrAction(const uint8_t &buttonPin, const uint8_t &statusLedPin, const String &fileToPlay) {
    //  When button is pressed and still pressed after the delay skip to next random file
    //    when you keep the button pressed it will skip automatically
    /* Tweak delay to fine-tune interaction */
    while (digitalRead(buttonPin) == HIGH) {
        delay(50);
        if (digitalRead(buttonPin) == HIGH) {

            digitalWrite(statusLedPin, LOW);

            if (musicPlayer.playingMusic) {
                musicPlayer.stopPlaying();
                //wait until stopped
                while(!musicPlayer.stopped());
            }

            if (!musicPlayer.startPlayingFile(fileToPlay.c_str())) {
                printlnToSerial(String(F("Error: could not play file: ")) + String(nextFileToPlay));
                Blink(statusLedPin, 2, 50, 100);
                return true; // also return true, since we want to get the next file if the current gives an error
            } else {
                digitalWrite(statusLedPin, HIGH);
                return true;
            }
        }
    }
    return false;
}