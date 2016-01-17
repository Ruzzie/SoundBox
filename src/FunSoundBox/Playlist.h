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

    void initialize(const char *rootPath, const Ruzzie::SimpleRandom simpleRandom);

    String nextRandomFilename(void);

private:
    int fileCount_ = 0;
    Ruzzie::SimpleRandom simpleRandom_;
    File root_;
};


#endif //FUNSOUNDBOX_PLAYLIST_H
