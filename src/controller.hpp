#ifndef CONTROLLER_H
#define CONTROLLER_H

/* mraa headers */
#include "mraa/common.hpp"
#include "mraa/spi.hpp"
#include "mraa/i2c.hpp"

namespace controller {
    int init();
    int shutdown();
    void update(int drawFrame);

    int initSerial();
    void debugSerial();
    void closeSerial();

    void getControls();

    void setArcade(int led, int arcadecol=0);
    void setArcadePattern(int led, int arcadecol=0);
    void setArcadeRGB(int led, int value, int R, int G, int B);

    void setArcadeBrightness(int led, int value);

    // to be removed:
    // void setArcadeLeds();

    void setVu(int vu, int value);
    void setLed(int led, int value);
    void clearLeds();
    void setAllLeds();
}

#endif