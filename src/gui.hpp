#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_gpu.h>
#include <string>

#include "filesystem.hpp"

namespace gui {
    extern SDL_Color colors[22];
    extern SDL_Color colors2[20];

    int init();
    void update(int drawFrame);
    void shutdown();

    void renderTextSmall(int x, int y, std::string text,int maxlen=0);
    void renderTextBig(int x, int y, std::string text,int maxlen=0);
    void renderBlock(int x, int y, int w, int h, int c);

    void renderLoops();
    void renderDirectory(int x, int y, int index, int maxlen);
    void dirDown(int amount);
    void dirUp(int amount);
    void dirStart();
    void dirEnd();
    std::string formatValue(int value);
    std::string formatDTValue(int value);

    void hLine(int x, int y, int len, int col);
    void vLine(int x, int y, int len, int col);
}
#endif