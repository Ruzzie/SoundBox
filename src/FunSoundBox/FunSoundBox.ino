// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <SD.h>
#include "Adafruit_VS1053.h"
#include "Playlist.h"


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

void setup() {
    pinMode(STATUS_LED, OUTPUT);
    pinMode(BUTTON_ONE, INPUT);
    pinMode(BUTTON_TWO, INPUT);

    digitalWrite(STATUS_LED, LOW);

    Serial.begin(9600);
    printlnToSerial("Initializing");

    while (!musicPlayer.begin()) { // initialise the music player
        printlnToSerial(F("Couldn't find VS1053, do you have the right pins defined?"));
        Blink(STATUS_LED, 3, 500, 500);
    }

    printlnToSerial(F("VS1053 found"));

    musicPlayer.setVolume(0, 255);

    while (!musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT)) {
        printlnToSerial(F("DREQ is not on an interrupt pin"));
        Blink(STATUS_LED, 2, 200, 200);
    }

    //Play success initialization tone
    musicPlayer.sineTest(27, 750);

    // initialise the SD card
    while (!SD.begin(CARDCS)) {
        printlnToSerial(F("Couldn't initialise SD card"));
        //If the sd card could not be initialised beep twice and blink
        musicPlayer.sineTest(0x44, 750);
        Blink(STATUS_LED, 5, 200, 400);
    }

    Ruzzie::SimpleRandom simpleRandom = Ruzzie::SimpleRandom(analogRead(VOLUME_INPUT));
    printlnToSerial("Randomizer created: first random: "+String(simpleRandom.Next()));

    defaultPlaylist.Initialize("/", simpleRandom, true);
    defaultPlaylist.PrintFilenames(Serial);

    digitalWrite(STATUS_LED, HIGH);

    if (SD.exists("INIT.MP3")) {
        printlnToSerial("INIT.MP3 found start playing.");
        musicPlayer.playFullFile("INIT.MP3");
        printlnToSerial("INIT.MP3 done playing.");
    }
}

void Blink(const uint8_t port, const uint8_t times, const unsigned long intervalInMs, const unsigned long durationInMs) {
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

const float volumeFactor = 255 / 1024;

void loop() {
    /*Tweak delays to fine-tune interaction*/

    int currentVolumeInputValue = analogRead(VOLUME_INPUT);
    if (abs(currentVolumeInputValue - volumeInputValue) > VOLUME_TOLERANCE) {
        digitalWrite(STATUS_LED, LOW);
        volumeInputValue = currentVolumeInputValue;
        uint8_t volume = volumeFactor * volumeInputValue;
        musicPlayer.setVolume(volume, volume);
        printlnToSerial("Volume raw: " + String(volumeInputValue));
        digitalWrite(STATUS_LED, HIGH);
        delay(20);
    }

    ButtonPollOrAction(BUTTON_ONE, STATUS_LED);

    delay(10);
}

void ButtonPollOrAction(const uint8_t buttonPin, const uint8_t statusLedPin) {
    //  When button is pressed and still pressed after the delay skip to next random file
    //    when you keep the button pressed it will skip automatically
    /* Tweak delays to fine-tune interaction */
    while (digitalRead(buttonPin) == HIGH) {
        delay(40);
        if (digitalRead(buttonPin) == HIGH) {
            digitalWrite(statusLedPin, LOW);
            if (musicPlayer.playingMusic) {
                musicPlayer.stopPlaying();
            }

            const char *nextFileName = defaultPlaylist.NextRandomFilename();
            if (!musicPlayer.startPlayingFile(nextFileName)) {
                printlnToSerial("Error: could not play file: " + String(nextFileName));
                Blink(statusLedPin, 2, 50, 100);
            } else {
                digitalWrite(statusLedPin, HIGH);
            }
            delay(10);
        }
    }
}
