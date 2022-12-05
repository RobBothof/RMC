#ifndef MASTERCONTROL_H
#define MASTERCONTROL_H

#include <cstdint>
#include <SDL2/SDL.h>

namespace arcade {
    extern bool edit;
}
namespace Macro {
    extern int Settings1[16];
    extern int Settings2[16];
    extern int Settings3[16];
    extern int Settings4[16];
    extern int SettingsButton1[16];    
    extern int SettingsButton2[16];    
    extern int SettingsButton3[16];    
    extern int SettingsButton4[16];    
}
namespace Fx {
    extern int Set;
    extern int Settings1[16];
    extern int Settings2[16];
    extern int Settings3[16];
    extern int Settings4[16];
    extern int DelayTime[5];
}
namespace MC {
    //globals
    extern int loopIndex;
    extern int loop1buffer[768];
    extern int loop2buffer[768];
    extern int loop3buffer[768];
    extern int loop4buffer[768];
    extern float pitch1buffer[768];
    extern float pitch2buffer[768];
    extern float pitch3buffer[768];
    extern float pitch4buffer[768];
    extern float pitches[5];
    extern bool pitchFollow[5];

    extern bool debug;

    extern int editLeds;
    extern int noteLeds;
    extern int loopSize[5]; //in number of midiclock ticks, 6, 24, 48,  96, 192, 384, 678
    extern uint8_t controlBuffer[63];
    extern int recstart[5];

    extern int mode;

    extern bool patternEdit;

    extern int BPM;
    extern float realBPM;
    extern int root;
    extern int source;

    // extern int FxSet;
    // extern int Fx1Setting[16];
    // extern int Fx2Setting[16];
    // extern int Fx3Setting[16];
    // extern int Fx4Setting[16];
    // extern int FxDelayTime[5];
//    uint32_t Fx1DeltaTime[16];
//    uint32_t Fx2DeltaTime[16];
//    uint32_t Fx3DeltaTime[16];
//    uint32_t Fx4DeltaTime[16];

    struct polynote {
    int voice;
    int note;
    };

    int getTimeStep(int newVal);

    int init();
    void update(int drawFrame, double deltatime);
    int shutdown();

    void updateArcadeLeds();

    void processControllerInput ();

    void setKeyRepeat(int key);
    void keyPadUp(int key);

    void keyPadDown(int key); 

    void PitchBend(int value);
    void ArcadeDown(int button);
    void ArcadeUp(int button);

    void FxChange(int enc, int diff);
    void MacroButton(int button, int value);

    void MacroChange(int enc, int diff);

    void notesAdd(int n);
    void notesRemove(int n);
    int  notesLast();

    void notesAdd2(int n);
    void notesRemove2(int n);

}




#endif