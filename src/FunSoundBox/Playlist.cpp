//
// Created by Dorus on 15-1-2016.
//

#include "Playlist.h"

Playlist::Playlist(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom) {
    initialize(rootPath, simpleRandom);
}

String Playlist::nextRandomFilename(void) {
    root_.rewindDirectory();
    int random = (int) simpleRandom_.next((const unsigned long) fileCount_);
    String fileName;

    if (!root_) {
        if(Serial) {
            Serial.println(F("Could not open root"));
        }
        return fileName;
    }

    int i = 0;
    while(i < fileCount_){
        File entry = root_.openNextFile();
#if defined(DEBUG)
        if (!entry) {
            if(Serial){
                Serial.println(F("No more files found."));
            }
            break;
        }
#endif
        //only get the file names in the current directory, not in the subdirectory
        if (!entry.isDirectory()) {
            if (i == random) {
                fileName = /*String(rootPath) +*/ String(entry.name());
                fileName.toUpperCase();
                //Serial.println("Random: " + String(random) + " filename: " + fileName);
                entry.close();
                return fileName;
            }
            ++i;
        }
        entry.close();
    }
    return fileName;
}

Playlist::Playlist(void) {
}

void Playlist::initialize(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom) {
    fileCount_ = 0;

    this->simpleRandom_ = simpleRandom;

    this->root_ = SD.open(rootPath);

#if defined(DEBUG)  
    if(Serial){
        Serial.println("New playlist created. Start listing files: ");
    }    
#endif

    if (!root_) {
#if defined(DEBUG)
        if(Serial){
            Serial.println("Could not open root: " + String(rootPath));
        }
#endif
        return;
    }

    while (true) {
        File entry = root_.openNextFile();
        if (!entry) {
#if defined(DEBUG)
            if(Serial){
                Serial.println("No more files found.");
            }
#endif
            break;
        }

        //only get the file names in the current directory, not in the subdirectory
        if (!entry.isDirectory()) {
#if defined(DEBUG)
            if(Serial){
                String fileName = String(rootPath) + String(entry.name());
                Serial.println("File: " + fileName);
            }
#endif
            ++fileCount_;
        } else {
#if defined(DEBUG)
            if(Serial){
                Serial.println("Directory found " + String(entry.name()) + " skipping..");
            }
#endif
        }
        entry.close();
    }
    root_.rewindDirectory();
#if defined(DEBUG)
    if(Serial){
        Serial.println("Playlist initialized with: " + String(fileCount_) + " files.");
    }
#endif
}
