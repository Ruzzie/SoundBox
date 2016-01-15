//
// Created by Dorus on 15-1-2016.
//

#ifndef FUNSOUNDBOX_PLAYLIST_H
#define FUNSOUNDBOX_PLAYLIST_H
#define PLAYLIST_MAX_SIZE 48
#include <SD.h>
#include <Arduino.h>
#include "SimpleRandom.h"

class Playlist {
public:
    Playlist();

    Playlist(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom, bool debug = true);

    void Initialize(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom, bool debug = true);

    int GetFileCount();

    const char *NextRandomFilename();

    void PrintFilenames(Stream &stream);

private:
    int fileCount;
    String fileNames[PLAYLIST_MAX_SIZE];
    Ruzzie::SimpleRandom simpleRandom;
    bool debug;
};


#endif //FUNSOUNDBOX_PLAYLIST_H
