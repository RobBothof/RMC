#ifndef MIDILOCAL_H
#define MIDILOCAL_H

#include "RtMidi.h"

namespace midiLocal {
    int init();
    int shutdown();
    int receive();
    int send(uint8_t b1);
    int send(uint8_t b1,uint8_t b2);
    int send(uint8_t b1,uint8_t b2, uint8_t b3);
}
#endif