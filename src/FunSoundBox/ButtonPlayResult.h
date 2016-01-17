//
// Created by dverhoeckx on 17-1-2016.
//

#ifndef FUNSOUNDBOX_BUTTONPLAYRESULT_H
#define FUNSOUNDBOX_BUTTONPLAYRESULT_H

struct ButtonPlayResult {
    ButtonPlayResult(): should_get_next_file(false), played_successful(false), button_was_pressed(false){}
    bool should_get_next_file;
    bool played_successful;
    bool button_was_pressed;
};

#endif //FUNSOUNDBOX_BUTTONPLAYRESULT_H
