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
    Playlist(void);

    Playlist(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom);

    void Initialize(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom);

    String NextRandomFilename(void);

private:
    int fileCount;
    Ruzzie::SimpleRandom simpleRandom;
    File root;
};


#endif //FUNSOUNDBOX_PLAYLIST_H
