// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <SD.h>
#include "Adafruit_VS1053.h"

// define the pins used
//#define CLK 13       // SPI Clock, shared with SD card
//#define MISO 12      // Input data, from VS1053/SD card
//#define MOSI 11      // Output data, to VS1053/SD card
// Connect CLK, MISO and MOSI to hardware SPI pins. 
// See http://arduino.cc/en/Reference/SPI "Connections"

// These are common pins between breakout and shield
#define DREQ             2      // VS1053 Data request, ideally an Interrupt pin
#define CARDCS           6     // Card chip select pin

#define BREAKOUT_DCS     8      // VS1053 Data/command select pin (output)
#define BREAKOUT_RESET   9      // VS1053 reset pin (output)
#define BREAKOUT_CS      10     // VS1053 chip select pin (output)


#define STATUS_LED       5
#define BUTTON_ONE       3       
//#define BUTTON_TWO       
#define VOLUME_INPUT     A0
#define VOLUME_TOLERANCE 31

Adafruit_VS1053_FilePlayer musicPlayer =
        Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);

int fileCount =0;
String filesToPlay[64];
int volumeInputValue = 1024;

void setup() {
    pinMode(STATUS_LED, OUTPUT);
    pinMode(BUTTON_ONE, INPUT);
//    pinMode(BUTTON_TWO, INPUT);

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

    //Play success initialization bleeps
    musicPlayer.sineTest(193, 500);
    
    // initialise the SD card
    while (!SD.begin(CARDCS)) {
        printlnToSerial(F("Couldn't initialise SD card"));
        //If the sd card could not be initialised beep twice and blink
        musicPlayer.sineTest(0x44, 500);
        musicPlayer.sineTest(0x44, 500);
        Blink(STATUS_LED, 5, 200, 400);
    }

    digitalWrite(STATUS_LED, HIGH);

    File root = SD.open("/");
    printlnToSerial("Files: ");
    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            break;
        }
        if (!entry.isDirectory()) {
            char* fileName = entry.name();
            printlnToSerial(fileName);
            filesToPlay[fileCount] = fileName;
            fileCount++;
        }
        if (entry) {
            entry.close();
        }
    }

    root.close();

    if (SD.exists("init.wav")) {
        musicPlayer.playFullFile("init.wav");
    }

    randomSeed(analogRead(VOLUME_INPUT));
}

void Blink(const uint8_t port, const uint8_t times, const unsigned long intervalInMs, const unsigned long durationInMs) {
    for (uint8_t i = 0; i < times; ++i) {
        digitalWrite(port, HIGH);
        delay(durationInMs);
        digitalWrite(port, LOW);
        delay(intervalInMs);
    }
}

void printlnToSerial(const char * text){
    if(Serial){
        Serial.println(text);
    }
}

void printlnToSerial(const String &text){
    if(Serial){
        Serial.println(text);
    }
}

void printlnToSerial(const __FlashStringHelper *ifsh){
    if(Serial){
        Serial.println(ifsh);
    }
}
const float volumeFactor = 255/1024;

void loop() {

    int currentVolumeInputValue = analogRead(VOLUME_INPUT);
    if (abs(currentVolumeInputValue - volumeInputValue) > VOLUME_TOLERANCE) {
        digitalWrite(STATUS_LED, LOW);
        volumeInputValue = currentVolumeInputValue;
        uint8_t volume = volumeFactor * volumeInputValue;
        musicPlayer.setVolume(volume, volume);
        printlnToSerial("Volume: " + String(volumeInputValue));
        digitalWrite(STATUS_LED, HIGH);
        delay(20);
    }

    while (digitalRead(BUTTON_ONE) == HIGH) {
        delay(40);
        if (digitalRead(BUTTON_ONE) == HIGH) {
            digitalWrite(STATUS_LED, LOW);
            if (musicPlayer.playingMusic) {
                musicPlayer.stopPlaying();
            }
            if (!musicPlayer.startPlayingFile(filesToPlay[random(fileCount)].c_str())) {
                Blink(STATUS_LED, 2, 50, 100);
            }
            digitalWrite(STATUS_LED, HIGH);
            delay(10);
        }
    }
    delay(1);
}
