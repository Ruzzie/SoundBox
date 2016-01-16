//
// Created by Dorus on 15-1-2016.
//

#include "Playlist.h"

Playlist::Playlist(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom) {
    Initialize(rootPath, simpleRandom);
}

String Playlist::NextRandomFilename(void) {
    root.rewindDirectory();
    int random = simpleRandom.Next(fileCount);
    String fileName;

    if (!root) {
        if(Serial) {
            Serial.println(F("Could not open root"));
        }
        return fileName;
    }

    int i = 0;
    while(i < fileCount){
        File entry = root.openNextFile();
        if (!entry) {
#if defined(DEBUG)
            if(Serial){
                Serial.println(F("No more files found."));
            }
#endif
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

Playlist::Playlist(void) {
  fileCount = 0;
}

void Playlist::Initialize(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom) {
    fileCount = 0;

    this->simpleRandom = simpleRandom;

    this->root = SD.open(rootPath);

#if defined(DEBUG)
    if (debug) {
        if(Serial){
            Serial.println("New playlist created. Start listing files: ");
        }
    }
#endif

    if (!root) {
#if defined(DEBUG)
        if(Serial){
            Serial.println("Could not open root: " + String(rootPath));
        }
#endif
        return;
    }

    while (true) {
        File entry = root.openNextFile();
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
            ++fileCount;
        } else {
#if defined(DEBUG)
            if(Serial){
                Serial.println("Directory found " + String(entry.name()) + " skipping..");
            }
#endif
        }
        entry.close();
    }
    root.rewindDirectory();
#if defined(DEBUG)
    if(Serial){
        Serial.println("Playlist initialized with: " + String(fileCount) + " files.");
    }
#endif
}
