#ifndef FUNCTIONS_H
#define FUNCTIONS_H
/*This source code copyrighted by Rob Bothof (2004-2014)
  and may not be redistributed without written permission.*/
// #include <SDL2/SDL.h>

// #include <SDL2/SDL_gpu.h>
// #include <string>
//#include <vector>

#include "filesystem.hpp"

// struct colorPalette {
//   SDL_Color colors[20];
// };
// struct bmpFont {
//   int w;
//   uint8_t char_w;
//   uint8_t char_h;
//   GPU_Image* t;
//   GPU_Rect src;
//   colorPalette* palette;
// };

// struct pattern {
//   uint8_t length;
//   uint8_t table [32][64];
// };

// struct track {
//   float tempo;
//   uint8_t speed;
//   std::string title;
//   pattern patterns[128];
//   uint8_t order[128];
//   uint8_t midichannel[8];
//   uint8_t orderpos;
// };

// struct player {
//   uint8_t row;
//   uint8_t orderpos;
//   bool isPlaying;
//   pattern (*p); //current pattern being played ???

//   uint32_t clock;;
//   int clocktick;
//   uint8_t tick;
// };

// struct pattern_editor {
//   uint8_t const (*positions)[2];
//   uint8_t x;
//   uint8_t y;
//   uint8_t edit_x;
//   uint8_t edit_y;
//   SDL_Rect drawrect;
//   SDL_Texture* bar;
//   SDL_Rect bar_rect;
//   uint8_t note;
//   uint8_t octave;
//   uint8_t volume;
//   uint8_t instrument;
//   uint8_t fx;
//   uint8_t fx_value;
//   int kmap [256][7];
//   int mod [6];
//   pattern (*p); //current pattern being editor
//   int patternid; //of number of current pattern
// };
// struct scope {
//   std::vector<float> amp;
//   uint8_t const (*color)[3];
//   SDL_Texture* t;
//   SDL_Rect src;
//   SDL_Rect dest;
//   int x;
//   int y;
// };

// struct loop {
//   SDL_Rect r;
//   int lastIndex;
//   colorPalette* palette;
//   GPU_Target* target;
// };
  
// void renderscope(SDL_Renderer* r, scope &s, int color);
// void renderLoops(GPU_Target* r,IOManager *iom, loop &l);

int getTemp();

// int getKey(pattern_editor &e, int scancode);
// void loadKeyMap(pattern_editor &e, std::string path);
//void drawPatternEditor(SDL_Renderer* r, bmpFont &f, pattern_editor &e, pattern &p, int playrow=0);
//void editNote(pattern &p, pattern_editor &e, SDL_Keysym &k);
// void renderText(GPU_Target* r, bmpFont &f, int x, int y, std::string text);
// void renderDirectory(GPU_Target* r, bmpFont &font, bmpFont &fontBig, int x, int y, int index);

/* void editUp(pattern &p, pattern_editor &e);
void editDown(pattern &p, pattern_editor &e);
void editLeft(pattern &p, pattern_editor &e);
void editRight(pattern &p, pattern_editor &e);
void editTABRight(pattern &p, pattern_editor &e);
void editTABLeft(pattern &p, pattern_editor &e);

void editIns(pattern &p, pattern_editor &e);
void editDel(pattern &p, pattern_editor &e);
void editPgUp(pattern &p, pattern_editor &e);
void editPgDown(pattern &p, pattern_editor &e);

void noteLast(pattern &p, pattern_editor &e);
void noteUp(pattern &p, pattern_editor &e, int part);
void noteDown(pattern &p, pattern_editor &e, int part);
void noteEdit(pattern &p, pattern_editor &e, int part, int value);
void noteOff(pattern &p, pattern_editor &e);
void noteDel(pattern &p, pattern_editor &e);

void instrumentLast(pattern &p, pattern_editor &e);
void instrumentUp(pattern &p, pattern_editor &e, int part);
void instrumentDown(pattern &p, pattern_editor &e, int part);
void instrumentEdit(pattern &p, pattern_editor &e, int part, int value);
void instrumentDel(pattern &p, pattern_editor &e);

void volumeLast(pattern &p, pattern_editor &e);
void volumeUp(pattern &p, pattern_editor &e, int part);
void volumeDown(pattern &p, pattern_editor &e, int part);
void volumeEdit(pattern &p, pattern_editor &e, int part, int value);
void volumeDel(pattern &p, pattern_editor &e);

void fxLast(pattern &p, pattern_editor &e);
void fxUp(pattern &p, pattern_editor &e);
void fxDown(pattern &p, pattern_editor &e);
void fxEdit(pattern &p, pattern_editor &e,int value);
void fxDel(pattern &p, pattern_editor &e);

void fx_valueLast(pattern &p, pattern_editor &e);
void fx_valueUp(pattern &p, pattern_editor &e, int part);
void fx_valueDown(pattern &p, pattern_editor &e, int part);
void fx_valueEdit(pattern &p, pattern_editor &e, int part, int value);
void fx_valueDel(pattern &p, pattern_editor &e);
 */

#endif