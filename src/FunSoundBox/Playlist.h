//
// Created by Dorus on 15-1-2016.
//

#ifndef FUNSOUNDBOX_PLAYLIST_H
#define FUNSOUNDBOX_PLAYLIST_H
#include <SD.h>
#include <Arduino.h>
#include "SimpleRandom.h"

class Playlist {
public:
    Playlist();

    Playlist(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom, bool debug = true);

    void Initialize(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom, bool debug = true);

    String NextRandomFilename();

private:
    int fileCount;
    Ruzzie::SimpleRandom simpleRandom;
    bool debug;
    File root;
};


#endif //FUNSOUNDBOX_PLAYLIST_H
