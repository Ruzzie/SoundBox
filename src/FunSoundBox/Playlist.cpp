//
// Created by Dorus on 15-1-2016.
//

#include "Playlist.h"

Playlist::Playlist(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom, bool debug) {

    Initialize(rootPath, simpleRandom, debug);
}
//todo: random aanpassen
String Playlist::NextRandomFilename() {
    root.rewindDirectory();
    int random = simpleRandom.Next(fileCount);
    //Serial.println("RAND: "+ String(random));
    String fileName;

    if (!root && debug) {
        Serial.println("Could not open root ");
        return String("");
    }

    int i = 0;
    while(i < fileCount){
        File entry = root.openNextFile();
        if (!entry) {
            if (debug) {
                Serial.println("No more files found.");
            }
            break;
        }

        //only get the file names in the current directory, not in the subdirectory
        if (!entry.isDirectory()) {
            if (i == random) {
                fileName = /*String(rootPath) +*/ String(entry.name());
                fileName.toUpperCase();
                //Serial.println("Random: " + String(random) + " filename: " + fileName);
                entry.close();
                break;

            }
            ++i;
        }
        entry.close();
    }
    return fileName;
}

Playlist::Playlist() {
  fileCount = 0;
}

void Playlist::Initialize(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom, bool debug) {
    fileCount = 0;

    this->simpleRandom = simpleRandom;
    this->debug = debug;

    this->root = SD.open(rootPath);

    if (debug) {
        Serial.println("New playlist created. Start listing files: ");
    }

    if (!root && debug) {
        Serial.println("Could not open root: " + String(rootPath));
        return;
    }

    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            if (debug) {
                Serial.println("No more files found.");
            }
            break;
        }

        //only get the file names in the current directory, not in the subdirectory
        if (!entry.isDirectory()) {

            String fileName = String(rootPath) + String(entry.name());
            if (debug) {
                Serial.println("File: " + fileName);
            }
            ++fileCount;
        } else {
            if (debug) {
                Serial.println("Directory found " + String(entry.name()) + " skipping..");
            }
        }
        entry.close();
    }
    root.rewindDirectory();
    if (debug) {
        Serial.println("Playlist initialized with: " + String(fileCount) + " files.");
    }
}
