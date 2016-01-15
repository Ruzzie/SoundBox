//
// Created by Dorus on 15-1-2016.
//

#include "Playlist.h"

Playlist::Playlist(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom, bool debug) {

    Initialize(rootPath, simpleRandom, debug);
}

int Playlist::GetFileCount() {
    return fileCount;
}

void Playlist::PrintFilenames(Stream &stream) {
    for (int i = 0; i < fileCount; ++i) {
        stream.println(fileNames[i]);
    }
}

const char *Playlist::NextRandomFilename() {
    int random = simpleRandom.Next(fileCount);
    String &filenameStr = fileNames[random];

    const char *randomFilename = filenameStr.c_str();
    if (debug) {
        Serial.println("Random number: " + String(random) + " filename: " + filenameStr);
    }
    return randomFilename;
}

Playlist::Playlist() {

}

void Playlist::Initialize(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom, bool debug) {
    fileCount = 0;

    this->simpleRandom = simpleRandom;
    this->debug = debug;

    File root = SD.open(rootPath);

    if (debug) {
        Serial.println("New playlist created. Start listing files: ");
    }

    if (!root && debug) {
        Serial.println("Could not open root: " + String(rootPath));
        return;
    }

    while (true) {
        if (fileCount > PLAYLIST_MAX_SIZE) {
            if (debug) {
                Serial.println("More files than " + String(PLAYLIST_MAX_SIZE) + "  found. Stopped adding file names.");
            }
            root.rewindDirectory();
            break;
        }

        File entry = root.openNextFile();
        if (!entry) {
            if (debug) {
                Serial.println("No more files found.");
            }
            root.rewindDirectory();
            break;
        }

        //only get the file names in the current directory, not in the subdirectory
        if (!entry.isDirectory()) {

            String fileName = String(rootPath) + "/" + String(entry.name());
            if (debug) {
                Serial.println("File: " + fileName);
            }
            fileNames[fileCount] = fileName;
            fileCount++;
        } else {
            if (debug) {
                Serial.println("Directory found " + String(entry.name()) + " skipping..");
            }
        }
        entry.close();
    }

    root.rewindDirectory();
    root.close();

    if (debug) {
        Serial.println("Playlist initialized with: " + String(fileCount) + " files.");
    }
}
