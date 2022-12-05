#include "gui.hpp"
#include "constants.hpp"
#include "masterControl.hpp"
#include <algorithm>

SDL_Color gui::colors[22] = {
    {0xB0, 0xB0, 0xB0,0xFF}, 
    {0x60, 0x60, 0x60,0xFF}, 
    {0x38, 0x38, 0x38,0xFF}, 
    {0x20, 0x20, 0x20,0xFF}, 

    {0xD4, 0x06, 0x00,0xFF}, 
    {0x68, 0x00, 0x06,0xFF}, 
    {0xD9, 0x75, 0x2B,0xFF}, 
    {0x5E, 0x36, 0x19,0xFF}, 

    {0xB7, 0xA7, 0x21,0xFF}, 
    {0x4C, 0x45, 0x16,0xFF}, 
    {0x2D, 0x95, 0x0E,0xFF}, 
    {0x19, 0x3E, 0x0C,0xFF}, 

    {0x05, 0xCA, 0x96,0xFF}, 
    {0x0B, 0x54, 0x40,0xFF}, 
    {0x17, 0x65, 0xA4,0xFF}, 
    {0x10, 0x2D, 0x44,0xFF}, 

    {0x00, 0x00, 0x00,0xFF}, 
    {0x1D, 0x1C, 0x1B,0xFF}, 
    {0x34, 0x35, 0x31,0xFF}, 
    {0x00, 0x00, 0x00,0x00},
    {0xFF, 0xFF, 0xFF,0x10},
    {0xFF, 0x08, 0x08,0x40},

};
SDL_Color gui::colors2[20] = {
    {0xB0, 0xB0, 0xB0,0xFF}, 
    {0x60, 0x60, 0x60,0xFF}, 
    {0x38, 0x38, 0x38,0xFF}, 
    {0x20, 0x20, 0x20,0xFF}, 

    {0xD0, 0x20, 0x18,0xFF}, 
    {0x80, 0x18, 0x10,0xFF}, 
    {0x6B, 0xA1, 0x2C,0xFF}, 
    {0x26, 0x80, 0x08,0xFF}, 
    
    {0xB2, 0x23, 0x2D,0xFF}, 
    {0x6B, 0xA1, 0x2C,0xFF}, 
    {0xD3, 0x82, 0x2A,0xFF}, 
    {0x26, 0x84, 0xAC,0xFF}, 
    {0xB1, 0x3F, 0x7D,0xFF}, 
    {0x47, 0xC9, 0x9D,0xFF}, 
    {0xBB, 0xBB, 0xBB,0xFF}, 
    {0x25, 0x25, 0x25,0xFF}, 
    {0x00, 0x00, 0x00,0xFF}, 
    
    {0x1D, 0x1C, 0x1B,0xFF}, 
    {0x34, 0x35, 0x31,0xFF}, 
    {0x00, 0x00, 0x00,0x00}};

std::string notes[25] = {"C ","C#","D ","D#","E ","F ","F#","G","G#","A","A#","B","C","C#","D ","D#","E ","F ","F#","G","G#","A","A#","B","C"};
const int SCREEN_H = 600;
const int SCREEN_W = 1024;

GPU_Target* screen;
GPU_Rect fontSmall;
GPU_Rect fontBig;
GPU_Image* fontTexSmall; 
GPU_Image* fontTexBig; 
//GPU_Image* layout;
//GPU_Image* layoutTest1;
//GPU_Image* layoutTest2;

GPU_Image* tex_bg;
GPU_Image* tex_bgLoops;

GPU_Image* tex_loops;
int lastIndex = 0;
GPU_Rect loopRect;

int gui::init() {


  //GPU_SetDebugLevel(GPU_DEBUG_LEVEL_MAX);
  screen = GPU_Init(SCREEN_W, SCREEN_H, SDL_WINDOW_BORDERLESS);
  // Did initialization fail?
  if(screen == NULL)
      return 1;

  GPU_SetDefaultAnchor 	(0.0,0.0); 		
  SDL_GL_SetSwapInterval(0);
  SDL_ShowCursor(SDL_DISABLE);

  fontTexSmall = GPU_LoadImage("gui/font/turbofont.png");
  fontTexBig = GPU_LoadImage("gui/font/turbofont-big.png");
  GPU_SetSnapMode(fontTexSmall,GPU_SNAP_NONE);
  GPU_SetSnapMode(fontTexBig,GPU_SNAP_NONE);

  //layout = GPU_LoadImage("gui/background2.png");
  tex_bg = GPU_LoadImage("gui/bg.png");
  tex_bgLoops = GPU_LoadImage("gui/bgLoops.png");
  GPU_SetSnapMode(tex_bg,GPU_SNAP_NONE);
  GPU_SetSnapMode(tex_bgLoops,GPU_SNAP_NONE);

  tex_loops = GPU_CreateImage(768,16*28,GPU_FORMAT_RGBA);
  GPU_LoadTarget(tex_loops);
}
std::string gui::formatValue(int value) {
    value = value*100/255;
    if (value < 10) {
        return "  " + std::to_string(value);
    } else if (value < 100) {
        return " " + std::to_string(value);
    } else {
        return std::to_string(value);
    }
}
std::string gui::formatDTValue(int value) {
    if (value > 127) {
        value = ((value-127)*100)/128;
        if (value < 10) {
            return "  " + std::to_string(value);
        } else if (value < 100) {
            return " " + std::to_string(value);
        } else {
            return std::to_string(value);
        }
    } else {
        if (value < 10) {
            return "  " + std::to_string(value) +"t";
        } else if (value < 100) {
            return " " + std::to_string(value) + "t";
        } else {
            return std::to_string(value) + "t";
        }

    }
}
void gui::update(int drawFrame) {
    //drawframe++;
    if (drawFrame == 0) {
        // framerate once every 20 loops
        //drawframe = 0;
        GPU_SetShapeBlending(false);
        //renderLoops(screen,&iom,loopgfx);
        GPU_SetShapeBlending(true);

        GPU_ClearColor(screen,gui::colors[16]);
    //      GPU_Blit(looptex, NULL, screen, 120,10);
        if (MC::mode == mode_File) {
            GPU_Blit(tex_bg, NULL, screen, 0,0);
            gui::renderTextSmall(2, 0, "\xF2[\xF0");
            gui::renderTextSmall(3, 0, "FILESYSTEM\xF2]");
            gui::hLine(2,1,12,1);

            gui::renderDirectory(2, 2, fs::dirIndex,16);
            gui::vLine(40,1,19,2);

        } else if (MC::mode == mode_FX) {
            GPU_Blit(tex_bg, NULL, screen, 0,0);
            gui::renderTextSmall(0, 0, "\xF2[\xF1");
            gui::renderTextSmall(1, 0, "EFFECTS\xF2]");
            gui::hLine(0,1,9,1);
            gui::renderTextSmall(50, 0, "\xF1 root: \xF0" + notes[MC::root]);
            gui::renderTextSmall(59, 0, "\xF1 BPM: \xF0" + std::to_string(MC::realBPM).substr(0,6));

            if (Fx::Set==1) {
                gui::renderTextSmall(0,  2, "\xF1 Boost: \xF1");
                gui::renderTextSmall(0,  3, "\xF1 Compress: \xF1");
            } else {
                gui::renderTextSmall(0,  2, "\xF3 Boost: \xF3");
                gui::renderTextSmall(0,  3, "\xF3 Compress: \xF3");
            }
            gui::renderTextSmall(24, 2, gui::formatValue(Fx::Settings1[0]));
            gui::renderTextSmall(37, 2, gui::formatValue(Fx::Settings2[0]));
            gui::renderTextSmall(51, 2, gui::formatValue(Fx::Settings3[0]));
            gui::renderTextSmall(64, 2, gui::formatValue(Fx::Settings4[0]));
            gui::renderTextSmall(24, 3, gui::formatValue(Fx::Settings1[1]));
            gui::renderTextSmall(37, 3, gui::formatValue(Fx::Settings2[1]));
            gui::renderTextSmall(51, 3, gui::formatValue(Fx::Settings3[1]));
            gui::renderTextSmall(64, 3, gui::formatValue(Fx::Settings4[1]));

            if (Fx::Set==2) {
                gui::renderTextSmall(0,  4, "\xF6 Side-in: \xF6");
                gui::renderTextSmall(0,  5, "\xF6 Side-out: \xF6");
            } else {
                gui::renderTextSmall(0,  4, "\xF3 Side-in: \xF3");
                gui::renderTextSmall(0,  5, "\xF3 Side-out: \xF3");
            }
            gui::renderTextSmall(24, 4, gui::formatValue(Fx::Settings1[2]));
            gui::renderTextSmall(37, 4, gui::formatValue(Fx::Settings2[2]));
            gui::renderTextSmall(51, 4, gui::formatValue(Fx::Settings3[2]));
            gui::renderTextSmall(64, 4, gui::formatValue(Fx::Settings4[2]));
            gui::renderTextSmall(24, 5, gui::formatValue(Fx::Settings1[3]));
            gui::renderTextSmall(37, 5, gui::formatValue(Fx::Settings2[3]));
            gui::renderTextSmall(51, 5, gui::formatValue(Fx::Settings3[3]));
            gui::renderTextSmall(64, 5, gui::formatValue(Fx::Settings4[3]));

            if (Fx::Set==3) {
                gui::renderTextSmall(0,  7, "\xFC EQ-freq: \xFC");
                gui::renderTextSmall(0,  8, "\xFC EQ-gain: \xFC");
            } else {
                gui::renderTextSmall(0,  7, "\xF3 EQ-freq: \xF3");
                gui::renderTextSmall(0,  8, "\xF3 EQ-gain: \xF3");
            }
            gui::renderTextSmall(24, 7, gui::formatValue(Fx::Settings1[4]));
            gui::renderTextSmall(37, 7, gui::formatValue(Fx::Settings2[4]));
            gui::renderTextSmall(51, 7, gui::formatValue(Fx::Settings3[4]));
            gui::renderTextSmall(64, 7, gui::formatValue(Fx::Settings4[4]));
            gui::renderTextSmall(24, 8, gui::formatValue(Fx::Settings1[5]));
            gui::renderTextSmall(37, 8, gui::formatValue(Fx::Settings2[5]));
            gui::renderTextSmall(51, 8, gui::formatValue(Fx::Settings3[5]));
            gui::renderTextSmall(64, 8, gui::formatValue(Fx::Settings4[5]));

            if (Fx::Set==5) {
                gui::renderTextSmall(0, 10, "\xF4 Distort: \xF4");
                gui::renderTextSmall(0, 11, "\xF4 Colour:  \xF4");
            } else {
                gui::renderTextSmall(0, 10, "\xF3 Distort: \xF3");
                gui::renderTextSmall(0, 11, "\xF3 Colour:  \xF3");
            }
            gui::renderTextSmall(24,10, gui::formatValue(Fx::Settings1[8]));
            gui::renderTextSmall(37,10, gui::formatValue(Fx::Settings2[8]));
            gui::renderTextSmall(51,10, gui::formatValue(Fx::Settings3[8]));
            gui::renderTextSmall(64,10, gui::formatValue(Fx::Settings4[8]));
            gui::renderTextSmall(24,11, gui::formatValue(Fx::Settings1[9]));
            gui::renderTextSmall(37,11, gui::formatValue(Fx::Settings2[9]));
            gui::renderTextSmall(51,11, gui::formatValue(Fx::Settings3[9]));
            gui::renderTextSmall(64,11, gui::formatValue(Fx::Settings4[9]));

            if (Fx::Set==6) {
                gui::renderTextSmall(0, 12, "\xF8 BitCrush: \xF8");
                gui::renderTextSmall(0, 13, "\xF8 SampleRate: \xF8");
            } else {
                gui::renderTextSmall(0, 12, "\xF3 BitCrush: \xF3");
                gui::renderTextSmall(0, 13, "\xF3 SampleRate: \xF3");
            }
            gui::renderTextSmall(24,12, gui::formatValue(Fx::Settings1[10]));
            gui::renderTextSmall(37,12, gui::formatValue(Fx::Settings2[10]));
            gui::renderTextSmall(51,12, gui::formatValue(Fx::Settings3[10]));
            gui::renderTextSmall(64,12, gui::formatValue(Fx::Settings4[10]));
            gui::renderTextSmall(24,13, gui::formatValue(Fx::Settings1[11]));
            gui::renderTextSmall(37,13, gui::formatValue(Fx::Settings2[11]));
            gui::renderTextSmall(51,13, gui::formatValue(Fx::Settings3[11]));
            gui::renderTextSmall(64,13, gui::formatValue(Fx::Settings4[11]));
      
            if (Fx::Set==7) {
                gui::renderTextSmall(0, 15, "\xFA DoubleDelay: \xFA");
                gui::renderTextSmall(0, 16, "\xFA Sync/Free: \xFA");
            } else {
                gui::renderTextSmall(0, 15, "\xF3 DoubleDelay: \xF3");
                gui::renderTextSmall(0, 16, "\xF3 Sync/Free: \xF3");
            }
            gui::renderTextSmall(24,15, gui::formatValue(Fx::Settings1[12]));
            gui::renderTextSmall(37,15, gui::formatValue(Fx::Settings2[12]));
            gui::renderTextSmall(51,15, gui::formatValue(Fx::Settings3[12]));
            gui::renderTextSmall(64,15, gui::formatValue(Fx::Settings4[12]));
            gui::renderTextSmall(24,16, gui::formatDTValue(Fx::DelayTime[1]));
            gui::renderTextSmall(37,16, gui::formatDTValue(Fx::DelayTime[2]));
            gui::renderTextSmall(51,16, gui::formatDTValue(Fx::DelayTime[3]));
            gui::renderTextSmall(64,16, gui::formatDTValue(Fx::DelayTime[4]));

            if (Fx::Set==8) {
                gui::renderTextSmall(0, 17, "\xFE Reverb: \xFE");
                gui::renderTextSmall(0, 18, "\xFE BrightNess: \xFE");
            } else {
                gui::renderTextSmall(0, 17, "\xF3 Reverb: \xF3");
                gui::renderTextSmall(0, 18, "\xF3 BrightNess: \xF3");
            }
            gui::renderTextSmall(24,17, gui::formatValue(Fx::Settings1[14]));
            gui::renderTextSmall(37,17, gui::formatValue(Fx::Settings2[14]));
            gui::renderTextSmall(51,17, gui::formatValue(Fx::Settings3[14]));
            gui::renderTextSmall(64,17, gui::formatValue(Fx::Settings4[14]));
            gui::renderTextSmall(24,18, gui::formatValue(Fx::Settings1[15]));
            gui::renderTextSmall(37,18, gui::formatValue(Fx::Settings2[15]));
            gui::renderTextSmall(51,18, gui::formatValue(Fx::Settings3[15]));
            gui::renderTextSmall(64,18, gui::formatValue(Fx::Settings4[15]));

        } else if (MC::mode == mode_Loop) {
            GPU_Blit(tex_bgLoops, NULL, screen, 0,0);
            gui::renderTextSmall(0, 0, "\xF2[\xF0");
            gui::renderTextSmall(1, 0, "L00P\xF2]");
            if (MC::source==source_drums) gui::renderTextSmall(8,0,"src:\xF1 Dr.Beat");
            if (MC::source==source_synth) gui::renderTextSmall(8,0,"src:\xF1 Synthy");
            gui::renderTextSmall(50, 0, "\xF1 root: \xF0" + notes[MC::root]);
            gui::renderTextSmall(59, 0, "\xF1 BPM: \xF0" + std::to_string(MC::realBPM).substr(0,6));
            gui::hLine(0,1,6,1);
            gui::renderLoops();
            loopRect.y=0;
            loopRect.w=7*14-2;
            loopRect.h=16*28;
            for (int j=0;j<8;j++) {
                loopRect.x=j*(7*14-2);
                GPU_Blit(tex_loops, &loopRect, screen, (j*7+2)*14+8+1+84,2*28+7);
            }
            //draw pitches
            if (MC::pitchFollow[1]) {
                if (MC::pitches[1] < 0) {
                    gui::renderTextSmall(65, 2, "\xFD " + std::to_string(MC::pitches[1]).substr(0,5));
                } else {
                    gui::renderTextSmall(65, 2, "\xFD +" + std::to_string(MC::pitches[1]).substr(0,4));
                }
            } else {
                if (MC::pitches[1] < 0) {
                    gui::renderTextSmall(65, 2, "\xF1 " + std::to_string(MC::pitches[1]).substr(0,5));
                } else {
                    gui::renderTextSmall(65, 2, "\xF1 +" + std::to_string(MC::pitches[1]).substr(0,4));
                }
            }
            if (MC::pitchFollow[2]) {
                if (MC::pitches[2] < 0) {
                    gui::renderTextSmall(65, 6, "\xF7 " + std::to_string(MC::pitches[2]).substr(0,5));
                } else {
                    gui::renderTextSmall(65, 6, "\xF7 +" + std::to_string(MC::pitches[2]).substr(0,4));
                }
            } else {
                if (MC::pitches[2] < 0) {
                    gui::renderTextSmall(65, 6, "\xF1 " + std::to_string(MC::pitches[2]).substr(0,5));
                } else {
                    gui::renderTextSmall(65, 6, "\xF1 +" + std::to_string(MC::pitches[2]).substr(0,4));
                }
            }
            if (MC::pitchFollow[3]) {
                if (MC::pitches[3] < 0) {
                    gui::renderTextSmall(65, 10, "\xFF " + std::to_string(MC::pitches[3]).substr(0,5));
                } else {
                    gui::renderTextSmall(65, 10, "\xFF +" + std::to_string(MC::pitches[3]).substr(0,4));
                }
            } else {
                if (MC::pitches[3] < 0) {
                    gui::renderTextSmall(65, 10, "\xF1 " + std::to_string(MC::pitches[3]).substr(0,5));
                } else {
                    gui::renderTextSmall(65, 10, "\xF1 +" + std::to_string(MC::pitches[3]).substr(0,4));
                }
            }
            if (MC::pitchFollow[4]) {
                if (MC::pitches[4] < 0) {
                    gui::renderTextSmall(65, 14, "\xF9 " + std::to_string(MC::pitches[4]).substr(0,5));
                } else {
                    gui::renderTextSmall(65, 14, "\xF9 +" + std::to_string(MC::pitches[4]).substr(0,4));
                }
            } else {
                if (MC::pitches[4] < 0) {
                    gui::renderTextSmall(65, 14, "\xF1 " + std::to_string(MC::pitches[4]).substr(0,5));
                } else {
                    gui::renderTextSmall(65, 14, "\xF1 +" + std::to_string(MC::pitches[4]).substr(0,4));
                }
            }
            //draw gain
            // gui::renderTextSmall(66, 3, "\xF1 " + std::to_string(MC::pitches[1]).substr(0,5));
 
        } else if (MC::mode == mode_Test2) {
            GPU_Blit(tex_bg, NULL, screen, 0,0);
            gui::renderTextSmall(2, 0, "\xF2[\xF6");
            gui::renderTextSmall(3, 0, "INSTRUMENT\xF2]");
            gui::hLine(2,1,12,1);
            gui::renderTextSmall(1, 2+0, "\xF0\x80\x81");
            gui::renderTextSmall(4, 2+0, "COLOR !0 \x01 \xA0");
            gui::renderTextSmall(1, 2+1, "\xF1\x82\x83");
            gui::renderTextSmall(4, 2+1, "COLOR @1 \x02 \xA1");
            gui::renderTextSmall(1, 2+2, "\xF2\x84\x85");
            gui::renderTextSmall(4, 2+2, "COLOR #2 \x03 \xA2");
            gui::renderTextSmall(1, 2+3, "\xF3\x86\x87");
            gui::renderTextSmall(4, 2+3, "COLOR $3 \x04 \xA3");
            gui::renderTextSmall(1, 2+4, "\xF4\x88\x89");
            gui::renderTextSmall(4, 2+4, "Color %4 \x05 \xA4");
            gui::renderTextSmall(1, 2+5, "\xF5\x8A\x8B");
            gui::renderTextSmall(4, 2+5, "Color ^5 \x06 \xA5");
            gui::renderTextSmall(1, 2+6, "\xF6\x8C\x8D");
            gui::renderTextSmall(4, 2+6, "Color &6 \x07 \x90\x91");
            gui::renderTextSmall(1, 2+7, "\xF7\x8E\x8F");
            gui::renderTextSmall(4, 2+7, "Color *7 \x08 \x09 \x92\x93\x94\x95\x96");
            gui::renderTextSmall(1, 2+9, "\xF8\x80\x81");
            gui::renderTextSmall(4, 2+9, "COLOR !0 \x01 \xA0");
            gui::renderTextSmall(1, 2+10, "\xF9\x82\x83");
            gui::renderTextSmall(4, 2+10, "COLOR @1 \x02 \xA1");
            gui::renderTextSmall(1, 2+11, "\xFA\x84\x85");
            gui::renderTextSmall(4, 2+11, "COLOR #2 \x03 \xA2");
            gui::renderTextSmall(1, 2+12, "\xFB\x86\x87");
            gui::renderTextSmall(4, 2+12, "COLOR $3 \x04 \xA3");
            gui::renderTextSmall(1, 2+13, "\xFC\x88\x89");
            gui::renderTextSmall(4, 2+13, "Color %4 \x05 \xA4");
            gui::renderTextSmall(1, 2+14, "\xFD\x8A\x8B");
            gui::renderTextSmall(4, 2+14, "Color ^5 \x06 \xA5");
            gui::renderTextSmall(1, 2+15, "\xFE\x8C\x8D");
            gui::renderTextSmall(4, 2+15, "Color &6 \x07 \x90\x91");
            gui::renderTextSmall(1, 2+16, "\xFF\x8E\x8F");
            gui::renderTextSmall(4, 2+16, "Color *7 \x08 \x09 \x92\x93\x94\x95\x96");
        }
    

        //draw VU
//        GPU_RectangleFilled(screen,120,520,135,590,gui::colors[7]);
//        GPU_RectangleFilled(screen,140,520,155,590,gui::colors[7]);

        //GPU_RectangleFilled(screen,122,588-iom.vu1*70,133,588,gui::colors[13]);
        //PU_RectangleFilled(screen,142,588-iom.vu2*70,153,588,gui::colors[13]);
    }
    
    if (drawFrame==4) {
        GPU_Flip(screen);
    }
}
void gui::hLine(int x, int y, int len, int col) {
    GPU_Line(screen,x*14+8,y*28+7,(x+len)*14+8,y*28+7,colors[col]);
}
void gui::vLine(int x, int y, int len, int col) {
    GPU_Line(screen,x*14+8,y*28+7,x*14+8,(y+len)*28+7,colors[col]);
}

void gui::shutdown() {
  GPU_FreeImage(fontTexSmall);
  GPU_FreeImage(fontTexBig);
  GPU_FreeImage(tex_bg);
  GPU_FreeImage(tex_bgLoops);
  GPU_FreeTarget(tex_loops->target);
  GPU_FreeImage(tex_loops);
  GPU_Quit();
  SDL_Quit();
}

void gui::renderTextSmall(int x, int y, std::string text,int maxlen) {
    fontSmall.w = 14;
    fontSmall.h = 28;
    int len;
    if (maxlen == 0) {
        len = text.length();
    } else {
        len = std::min(maxlen,(int) text.length());
    }
    for( int i = 0; i < len; ++i ) {
        uint8_t chr = (uint8_t)text[ i ];
        //set color 0-15 if ascii value is 240-255
        if (chr > 239) {
            // i do not specify a default earlier, so color can inherit from a previous render
            GPU_SetColor(fontTexSmall,gui::colors[chr-240]);
        } else {
        //not a color, render the chars
        //locate the position in the bitmapfont texture
        fontSmall.x = (chr %  32) * 14;
        fontSmall.y = (chr /  32) * 28;
        GPU_Blit(fontTexSmall, &fontSmall, screen, x*14+8,y*28+7);
        x++; // advance cursor
        }
    }
}
void gui::renderTextBig(int x, int y, std::string text,int maxlen) {
    fontBig.w = 28;
    fontBig.h = 56;
    int xx=0;
    int len;
    if (maxlen == 0) {
        len = text.length();
    } else {
        len = std::min(maxlen,(int) text.length());
    }
    for( int i = 0; i < len; ++i ) {
        uint8_t chr = (uint8_t)text[ i ];
        //set color 0-15 if ascii value is 240-255
        if (chr > 239) {
            // i do not specify a default earlier, so color can inherit from a previous render
            GPU_SetColor(fontTexBig,gui::colors[chr-240]);
        } else {
        //not a color, render the chars
        //locate the position in the bitmapfont texture
        fontBig.x = (chr %  32) * 28;
        fontBig.y = (chr /  32) * 56;
        GPU_Blit(fontTexBig, &fontBig, screen, x*14+xx*28+8,y*56+7);
        xx++; // advance cursor
        }
    }
}
void gui::renderBlock(int x, int y, int w, int h, int c) {
    GPU_RectangleFilled(screen,x*14+8,y*28+7,x*28+8+w*28,y*28+7+h*28,gui::colors[c]);
}
void gui::dirDown(int amount) {
    fs::dirIndex++;    
    if (fs::dirIndex > fs::directoryContents.size()-1) fs::dirIndex = fs::directoryContents.size()-1;
}
void gui::dirUp(int amount) {
    fs::dirIndex--;    
    if (fs::dirIndex < 0) fs::dirIndex = 0;
}
void gui::dirStart() {
    fs::dirIndex=0;    
}
void gui::dirEnd() {
    fs::dirIndex = fs::directoryContents.size()-1;
}
void gui::renderDirectory(int x, int y, int index, int maxlen) {
    int dirsize = fs::directoryContents.size(); 
    int filesOffset=0;
    renderTextSmall(x, y, "\xF0" + fs::directoryCurrent);
    if (index < 4) {
        int current = y+filesOffset+index;
        std::string fcol = "\xF1";
        renderBlock(x, (y+filesOffset+index)*2, 18,2,4);        
        //renderTextBig(x, y+filesOffset+index, "\xFD\x88\x89");
        for (unsigned i=0;i<std::min(7,(int) dirsize);i++) {
            if (i==index) {
                fcol = "\xF0";
            } else {
                fcol = "\xF1";
            }
            if (fs::directoryContents[i].isDir) {
                gui::renderTextBig(x, y+i+filesOffset, fcol + "\x80\x81");
            } else {
                gui::renderTextBig(x, y+i+filesOffset, fcol + "\x86\x87");
            }
            gui::renderTextBig(x+5, y+i+filesOffset, fs::directoryContents[i].filename,maxlen);
        }
    } else if (index > dirsize-4) {
        int current = y+filesOffset+7-dirsize+index;
        std::string fcol = "\xF1";
        renderBlock(x, (y+filesOffset+7-dirsize+index)*2, 18,2,4);
        //renderTextBig(x, y+filesOffset+7-dirsize+index, "\xFD\x88\x89");
        for (unsigned i=dirsize-7;i<dirsize;i++) {
            if (i==index) {
                fcol = "\xF0";
            } else {
                fcol = "\xF1";
            }
            if (fs::directoryContents[i].isDir) {
                gui::renderTextBig(x, y+i+filesOffset-dirsize+7, fcol + "\x80\x81");
            } else {
                gui::renderTextBig(x, y+i+filesOffset-dirsize+7, fcol + "\x86\x87");
            }
            gui::renderTextBig(x+5, y+i+filesOffset-dirsize+7, fs::directoryContents[i].filename,maxlen);
        }

    } else {
        int current = y+3+filesOffset;
        std::string fcol = "\xF1";

        renderBlock(x, (y+3+filesOffset)*2, 18,2,4);
        for (unsigned i=index-3;i<std::min(index-3+7,(int) dirsize);i++) {
            if (i==index) {
                fcol = "\xF0";
            } else {
                fcol = "\xF1";
            }
            if (fs::directoryContents[i].isDir) {
                gui::renderTextBig(x, y+i+filesOffset-(index-3), fcol + "\x80\x81");
            } else {
                gui::renderTextBig(x, y+i+filesOffset-(index-3), fcol + "\x86\x87");
            }
            gui::renderTextBig(x+5, y+i+filesOffset-(index-3), fs::directoryContents[i].filename,maxlen);
        }
    }
}


void gui::renderLoops() {
    int index = MC::loopIndex; 
    
    if (index < lastIndex) {
      index=768;
    }

    for (int i = lastIndex; i <index;i++) {
      int v1 = MC::loop1buffer[i];
      int v2 = MC::loop2buffer[i];
      int v3 = MC::loop3buffer[i];
      int v4 = MC::loop4buffer[i];

      int p1 = (int) -MC::pitch1buffer[i];
      int p2 = (int) -MC::pitch2buffer[i];
      int p3 = (int) -MC::pitch3buffer[i];
      int p4 = (int) -MC::pitch4buffer[i];

      if (v1 < 1) v1 = 1;
      if (v2 < 1) v2 = 1;
      if (v3 < 1) v3 = 1;
      if (v4 < 1) v4 = 1;

      //clear old samples
      GPU_SetShapeBlending(false);
      GPU_RectangleFilled(tex_loops->target,i,0,i+1,16*28,gui::colors[19]);
      //GPU_SetShapeBlending(true);
      //GPU_SetShapeBlendMode(GPU_BLEND_ADD);
      //draw samples
      GPU_RectangleFilled(tex_loops->target,i,2*28-v1,i+1,2*28+v1,gui::colors[12]);
      GPU_RectangleFilled(tex_loops->target,i,2*28-v1*0.92,i+1,2*28+v1*0.92,gui::colors[13]);
    //   GPU_RectangleFilled(tex_loops->target,i,2*28-v1*0.33,i+1,2*28+v1*0.33,gui::colors[1]);

      GPU_RectangleFilled(tex_loops->target,i,6*28-v2,i+1,6*28+v2,gui::colors[6]);
      GPU_RectangleFilled(tex_loops->target,i,6*28-v2*0.92,i+1,6*28+v2*0.92,gui::colors[7]);
    //   GPU_RectangleFilled(tex_loops->target,i,6*28-v2*0.33,i+1,6*28+v2*0.33,gui::colors[1]);
      
      GPU_RectangleFilled(tex_loops->target,i,10*28-v3,i+1,10*28+v3,gui::colors[14]);
      GPU_RectangleFilled(tex_loops->target,i,10*28-v3*0.92,i+1,10*28+v3*0.92,gui::colors[15]);
    //   GPU_RectangleFilled(tex_loops->target,i,10*28-v3*0.5,i+1,10*28+v3*0.5,gui::colors[1]);
      
      GPU_RectangleFilled(tex_loops->target,i,14*28-v4,i+1,14*28+v4*1,gui::colors[8]);
      GPU_RectangleFilled(tex_loops->target,i,14*28-v4*0.92,i+1,14*28+v4*0.92,gui::colors[9]);
    //   GPU_RectangleFilled(tex_loops->target,i,14*28-v4*0.33,i+1,14*28+v4*0.33,gui::colors[1]);
      
      //draw Pitches
      GPU_RectangleFilled(tex_loops->target,i, 2*28+p1+1,i+1, 2*28+p1,gui::colors[0]);
      GPU_RectangleFilled(tex_loops->target,i, 6*28+p2+1,i+1, 6*28+p2,gui::colors[0]);
      GPU_RectangleFilled(tex_loops->target,i,10*28+p3+1,i+1,10*28+p3,gui::colors[0]);
      GPU_RectangleFilled(tex_loops->target,i,14*28+p4+1,i+1,14*28+p4,gui::colors[0]);


    }
    if (index==768) {
      lastIndex=0;
    } else {
      lastIndex=index;
    }

    GPU_SetShapeBlending(true);
    int smppointer = lastIndex + lastIndex/48;
    int endIndex1= lastIndex - MC::loopSize[1];
    int endIndex2= lastIndex - MC::loopSize[2];
    int endIndex3= lastIndex - MC::loopSize[3];
    int endIndex4= lastIndex - MC::loopSize[4];



    if (MC::recstart[1]!=-1) {
        int recIndex = MC::recstart[1] + MC::recstart[1]/48;
        if (recIndex > smppointer) {
            GPU_RectangleFilled(screen,recIndex+37+84, 7+4*28-48,768+16+37+84, 7+4*28+48,gui::colors[21]);
            GPU_RectangleFilled(screen,37+84, 7+4*28-48,smppointer+37+84, 7+4*28+48,gui::colors[21]);
        } else {
            GPU_RectangleFilled(screen,smppointer+37+84, 7+4*28-48,recIndex+37+84, 7+4*28+48,gui::colors[21]);
        }
    }

    if (MC::recstart[2]!=-1) {
        int recIndex = MC::recstart[2] + MC::recstart[2]/48;
        if (recIndex > smppointer) {
            GPU_RectangleFilled(screen,recIndex+37+84, 7+8*28-48,768+16+37+84, 7+8*28+48,gui::colors[21]);
            GPU_RectangleFilled(screen,37+84, 7+8*28-48,smppointer+37+84, 7+8*28+48,gui::colors[21]);
        } else {
            GPU_RectangleFilled(screen,smppointer+37+84, 7+8*28-48,recIndex+37+84, 7+8*28+48,gui::colors[21]);
        }
    }

    if (MC::recstart[3]!=-1) {
        int recIndex = MC::recstart[3] + MC::recstart[3]/48;
        if (recIndex > smppointer) {
            GPU_RectangleFilled(screen,recIndex+37+84, 7+12*28-48,768+16+37+84, 7+12*28+47,gui::colors[21]);
            GPU_RectangleFilled(screen,37+84, 7+12*28-48,smppointer+37+84, 7+12*28+47,gui::colors[21]);
        } else {
            GPU_RectangleFilled(screen,smppointer+37+84, 7+12*28-48,recIndex+37+84, 7+12*28+48,gui::colors[21]);
        }
    }

    if (MC::recstart[4]!=-1) {
        int recIndex = MC::recstart[4] + MC::recstart[4]/48;
        if (recIndex > smppointer) {
            GPU_RectangleFilled(screen,recIndex+37+84, 7+16*28-48,768+16+37+84, 7+16*28+48,gui::colors[21]);
            GPU_RectangleFilled(screen,37+84, 7+16*28-48,smppointer+37+84, 7+16*28+48,gui::colors[21]);
        } else {
            GPU_RectangleFilled(screen,smppointer+37+84, 7+16*28-48,recIndex+37+84, 7+16*28+48,gui::colors[21]);
        }
    }

    if (endIndex1 < 0) {
        endIndex1 = (endIndex1+768) + (endIndex1+768)/48;
        GPU_RectangleFilled(screen,endIndex1+37+84, 7+4*28-48,768+16+37+84, 7+4*28+48,gui::colors[20]);
        GPU_RectangleFilled(screen,37+84, 7+4*28-48,smppointer+37+84, 7+4*28+48,gui::colors[20]);
    } else {
        endIndex1 = (endIndex1) + (endIndex1)/48;
        GPU_RectangleFilled(screen,smppointer+37+84, 7+4*28-48,endIndex1+37+84, 7+4*28+48,gui::colors[20]);
    }
    if (endIndex2 < 0) {
        endIndex2 = (endIndex2+768) + (endIndex2+768)/48;
        GPU_RectangleFilled(screen,endIndex2+37+84, 7+8*28-48,768+16+37+84, 7+8*28+48,gui::colors[20]);
        GPU_RectangleFilled(screen,37+84, 7+8*28-48,smppointer+37+84, 7+8*28+48,gui::colors[20]);
    } else {
        endIndex2 = (endIndex2) + (endIndex2)/48;
        GPU_RectangleFilled(screen,smppointer+37+84, 7+8*28-48,endIndex2+37+84, 7+8*28+48,gui::colors[20]);
    }
    if (endIndex3 < 0) {
        endIndex3 = (endIndex3+768) + (endIndex3+768)/48;
        GPU_RectangleFilled(screen,endIndex3+37+84, 7+12*28-48,768+16+37+84, 7+12*28+48,gui::colors[20]);
        GPU_RectangleFilled(screen,37+84, 7+12*28-48,smppointer+37+84, 7+12*28+48,gui::colors[20]);
    } else {
        endIndex3 = (endIndex3) + (endIndex3)/48;
        GPU_RectangleFilled(screen,smppointer+37+84, 7+12*28-48,endIndex3+37+84, 7+12*28+48,gui::colors[20]);
    }
    if (endIndex4 < 0) {
        endIndex4 = (endIndex4+768) + (endIndex4+768)/48;
        GPU_RectangleFilled(screen,endIndex4+37+84, 7+16*28-48,768+16+37+84, 7+16*28+48,gui::colors[20]);
        GPU_RectangleFilled(screen,37+84, 7+16*28-48,smppointer+37+84, 7+16*28+48,gui::colors[20]);
    } else {
        endIndex4 = (endIndex4) + (endIndex4)/48;
        GPU_RectangleFilled(screen,smppointer+37+84, 7+16*28-48,endIndex4+37+84, 7+16*28+48,gui::colors[20]);
    }
    GPU_SetShapeBlending(false);

    GPU_RectangleFilled(tex_loops->target,lastIndex,  2*28-48,lastIndex+1, 2*28+48,gui::colors[1]);
    GPU_RectangleFilled(tex_loops->target,lastIndex,  6*28-48,lastIndex+1, 6*28+48,gui::colors[1]);
    GPU_RectangleFilled(tex_loops->target,lastIndex, 10*28-48,lastIndex+1,10*28+48,gui::colors[1]);
    GPU_RectangleFilled(tex_loops->target,lastIndex, 14*28-48,lastIndex+1,14*28+48,gui::colors[1]);

}



