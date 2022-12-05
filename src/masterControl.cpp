#include "masterControl.hpp"
#include "controller.hpp"
#include "midiLocal.hpp"
#include "audio.hpp"
#include "gui.hpp"
#include <SDL2/SDL.h>
#include "constants.hpp"
#include <vector>
#include <cstring> //memset
#include <cmath>
//#include <string>         // std::string
//#include <cstddef>         // std::size_t

bool inputArcadeButtons[36];
bool inputEncodersButtons[16];
bool inputMixerButtons[4];
uint8_t inputFaders[6];
uint8_t inputKnobs[20];
float inputEncoders[16];
uint8_t inputKeypad[33];
uint8_t inputFootPedal[2];
uint8_t inputModulation=0;

int MC::mode;
float pbscroll;
//globals!
uint8_t MC::controlBuffer[63];
int MC::loopIndex;
int MC::loop1buffer[768];
int MC::loop2buffer[768];
int MC::loop3buffer[768];
int MC::loop4buffer[768];
float MC::pitch1buffer[768];
float MC::pitch2buffer[768];
float MC::pitch3buffer[768];
float MC::pitch4buffer[768];
float MC::pitches[5] = {0,0,0,0,0};
bool MC::pitchFollow[5] = {false,false,false,false,false};

// int Fx::Set=0;
// int Fx::Settings1[16] = {0,0,0,0,127,0,0,0,0,0,0,0,0,64,0,0};
// int Fx::Settings2[16] = {0,0,0,0,127,0,0,0,0,0,0,0,0,64,0,0};
// int Fx::Settings3[16] = {0,0,0,0,127,0,0,0,0,0,0,0,0,64,0,0};
// int Fx::Settings4[16] = {0,0,0,0,127,0,0,0,0,0,0,0,0,64,0,0};
// int Fx::DelayTime[5] = {0,18,18,18,18};

int Fx::Set=0;
int Fx::Settings1[16] = {0,0,0,0,127,0,0,0,0,0,0,0,0,64,0,0};
int Fx::Settings2[16] = {0,0,0,0,127,0,0,0,0,0,0,0,0,64,0,0};
int Fx::Settings3[16] = {0,0,0,0,127,0,0,0,0,0,0,0,0,64,0,0};
int Fx::Settings4[16] = {0,0,0,0,127,0,0,0,0,0,0,0,0,64,0,0};
int Fx::DelayTime[5] = {0,18,18,18,18};

int Macro::Settings1[16] = {0,0,0,0,127,0,0,0,0,0,0,0,0,64,0,0};
int Macro::Settings2[16] = {0,0,0,0,127,0,0,0,0,0,0,0,0,64,0,0};
int Macro::Settings3[16] = {0,0,0,0,127,0,0,0,0,0,0,0,0,64,0,0};
int Macro::Settings4[16] = {0,0,0,0,127,0,0,0,0,0,0,0,0,64,0,0};
int Macro::SettingsButton1[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int Macro::SettingsButton2[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int Macro::SettingsButton3[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int Macro::SettingsButton4[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


namespace Fx {
  // uint32_t deltaTime1[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  // uint32_t deltaTime2[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  // uint32_t deltaTime3[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  // uint32_t deltaTime4[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  int timeStep[16]={1,2,4,6,8,12,16,18,24,36,48,54,66,72,96};
}

bool MC::debug = false;
int MC::source=1;

int MC::loopSize[5] = {0,96,96,96,96};
int MC::recstart[5] = {0,-1,-1,-1,-1};

int keyRepeatDelay=500;
int keyRepeatRate=100;
double keyRepeatTimer = 0;
int lastkey=32;

int MC::editLeds=0;
int MC::noteLeds=0;

int MC::BPM;
float MC::realBPM;
int MC::root;

int R=0;
int G=0;
int B=0;

int lastMode = mode_Loop;
double returnTimer = 0;
double returnTime=2500;





namespace arcade {
  bool ch1 = false;
  bool ch2 = false;
  bool ch3 = false;
  bool ch4 = false;
  bool rec = false;
  bool shift = false;
  bool hold = false;
  bool arp = false;
  bool repeat = false;
  bool edit = false;
  bool chord = false;
  bool pitch = false;
  int octave = 3;
//  bool pattern[16] = {true,false,true,false,true,false,true,false,true,false,true,false,true,false,true,false};
  bool held[16] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};
  bool down[16] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};
  int positions[16] = {7,8,9,10,13,14,15,16,19,20,21,22,25,26,27,28};
  int notes[16] = {12,13,14,15,8,9,10,11,4,5,6,7,0,1,2,3};  
}
namespace arp {
  bool pattern[4] = {true,true,true,true};
  int patternSteps[4] = {1,1,1,1};
  bool patternRepeat[4] = {true,true,true,true};
  int positions[16] = {7,13,19,25,8,14,20,26,9,15,21,27,10,16,22,28};
  int step=0;
  int oldStep=0;
  int note=0;
}


std::vector<int> notesList;
std::vector<MC::polynote> polyList;

void MC::notesAdd2(int n) {
  polynote tmpp;
  tmpp.note=n;
  if (polyList.size() < 4) {
    tmpp.voice=polyList.size();
    polyList.push_back(tmpp);
  } else { 
    //is there a free voice ?
    //yes:: move that voice to the end
    int free=-1;
    for (int i=0; i<polyList.size(); ++i) {
      if (polyList[i].note==-1) {
        free=i;
        break;
      }
    }
    if (free != -1) {
      tmpp.voice = polyList[free].voice;
      polyList.erase(polyList.begin()+free);
      polyList.push_back(tmpp);
    } else {
      //no::
      //steal first note
      tmpp.voice = polyList[0].voice;
      polyList.erase(polyList.begin());
      polyList.push_back(tmpp);
    }
  }
}
void MC::notesRemove2(int n) {
  for (int i=0; i<polyList.size(); ++i) {
    if (n==polyList[i].note) {
      polyList[i].note=-1;
    }
  }
}

void MC::notesAdd(int n) {
  notesList.push_back(n);
}

void MC::notesRemove(int n) {
  for (int i=0; i<notesList.size(); ++i) {
    if (n==notesList[i]) {
      notesList.erase(notesList.begin()+i);
    }
  }
}

int MC::notesLast() {
  if (notesList.size() > 0) {
    return notesList[notesList.size()-1];
  } else {
    return -1;
  }
}


int MC::init() {
    memset(inputArcadeButtons, 0, sizeof(inputArcadeButtons));
    memset(inputEncodersButtons, 0, sizeof(inputEncodersButtons));
    memset(inputMixerButtons, 0, sizeof(inputMixerButtons));
    memset(inputEncoders, 0, sizeof(inputEncoders));
    memset(inputFaders, 0, sizeof(inputFaders));
    memset(inputKnobs, 0, sizeof(inputKnobs));
    memset(inputKeypad, 0, sizeof(inputKnobs));

    MC::mode = mode_Loop;
    arcade::edit = false;
    pbscroll = 0;
    MC::BPM=128;
    lpd::sendFloat("Main-BPM",MC::BPM);

    MC::loopSize[0]=96;
    MC::loopSize[1]=96;
    MC::loopSize[2]=96;
    MC::loopSize[3]=96;
    MC::loopSize[4]=96;

    controller::setArcade(35-arcade::octave*6,7);
    MC::updateArcadeLeds();

    lpd::sendFloat("Rec",0);
    lpd::sendFloat("ArpMode",0);
    lpd::sendFloat("ChordMode",0);

}

void MC::updateArcadeLeds() {
    //EDIT MODE
    if (arcade::edit) {
    //clear leds
      for (int i=0;i<16;i++) {
          controller::setArcadePattern(arp::positions[i],0);
      }
      //set step leds
      for (int i=0;i<4;i++) {
        if (arp::patternSteps[i]>0) {
          controller::setArcadePattern(arp::positions[4*i+arp::patternSteps[i]-1],3);
        }
      }
      // set note leds
      for (int i=0;i<4;i++) {
        if (arp::pattern[i]) {
          controller::setArcadePattern(i+1,3);
        } else {
          controller::setArcadePattern(i+1,0);
        }
      }
      //set repeat leds
      for (int i=0;i<4;i++) {
        if (arp::patternRepeat[i]) {
          controller::setArcadePattern(i+31,3);
        } else {
          controller::setArcadePattern(i+31,0);
        }
      }
      if (editLeds>0) {
        controller::setArcadePattern(editLeds,2);
      }
    //PLAY MODE
    } else {
      //clear leds
      for (int i=0;i<16;i++) {
          controller::setArcade(arcade::positions[i],0);
      }
      for (int i=0;i<16;i++) {
        //hold mode
        if (arcade::hold) {
          if (arcade::held[i]) {
            //chord mode
            if ((arcade::arp) && (arcade::chord)) {
              controller::setArcade(arcade::positions[i],4);
              if (arp::pattern[editLeds-1]) {
                if (arcade::rec) {
                  controller::setArcade(arcade::positions[i],1);
                } else {
                  controller::setArcade(arcade::positions[i],7);
                }
              } 
            }
            //arp mode
            if ((arcade::arp) && (!arcade::chord)) {
              controller::setArcade(arcade::positions[i],4);
              for (int j=0;j<16;j++) {
                if (arcade::notes[j]==MC::noteLeds) {
                  if (arcade::rec) {
                    controller::setArcade(arcade::positions[j],1);
                  } else {
                    controller::setArcade(arcade::positions[j],7);
                  }
                }
              }
            }
            //free play
            if ((!arcade::arp)) {
              if (arcade::rec) {
                controller::setArcade(arcade::positions[i],1);
              } else {
                controller::setArcade(arcade::positions[i],4);
              }
            }
          }
        //play mode mode
        } else {
          if (arcade::down[i]) {
            //chord mode
            if ((arcade::arp) && (arcade::chord)) {
               if (editLeds > 0) {
                if (arcade::rec) {
                  controller::setArcade(arcade::positions[i],1);
                } else {
                  controller::setArcade(arcade::positions[i],7);
                }
               } 
            } 
            //arp mode
            if ((arcade::arp) && (!arcade::chord)) {
              for (int j=0;j<16;j++) {
                if (arcade::notes[j]==MC::noteLeds) {
                  if (arcade::rec) {
                    controller::setArcade(arcade::positions[j],1);
                  } else {
                    controller::setArcade(arcade::positions[j],7);
                  }
                }
              }
            }
            //free play mode
            if ((!arcade::arp)) {
                if (arcade::rec) {
                  controller::setArcade(arcade::positions[i],1);
                } else {
                  controller::setArcade(arcade::positions[i],7);
                }
            }
          }
        }
      }
    }
}
void MC::update(int drawFrame, double deltatime) {
    MC::processControllerInput ();

    controller::setVu(3,(MC::loop1buffer[MC::loopIndex]/5));
    controller::setVu(2,(MC::loop2buffer[MC::loopIndex]/5));
    controller::setVu(1,(MC::loop3buffer[MC::loopIndex]/5));
    controller::setVu(0,(MC::loop4buffer[MC::loopIndex]/5));

    //std::cout << "deltatime: " << deltatime << std::endl;
    MC::updateArcadeLeds();
    
    keyRepeatTimer+=deltatime;
    if (keyRepeatTimer>keyRepeatDelay+keyRepeatRate) {
      keyRepeatTimer-=keyRepeatRate;
      inputKeypad[lastkey]=0;
    }

    if (returnTimer > 0) {
      returnTimer-=deltatime;
      if (returnTimer <=0) {
        returnTimer=0;
        MC::mode=lastMode;
      }
    }
}
void MC::setKeyRepeat(int key) {
  if (key != lastkey) {
    lastkey=key;
    keyRepeatTimer=0;
  }
}
int MC::shutdown() {
}
int MC::getTimeStep(int newVal) {
  int time = 15 - (int) ((newVal*16)/128);
  int retval = 18;
  return Fx::timeStep[time];
}
void MC::MacroButton(int button, int value) {
  int setting=0;
  int newVal;
  if (value > 0) {
    if (setting==0) {
      if (button==0) {
          newVal = (Macro::SettingsButton1[setting]+value)%2;
          Macro::SettingsButton1[setting]=newVal;
          lpd::sendFloat("MacroButton00-1",newVal);
      }
      if (button==1) {
          newVal = (Macro::SettingsButton2[setting]+value)%2;
          Macro::SettingsButton2[setting]=newVal;
          lpd::sendFloat("MacroButton00-2",newVal);
      }
      if (button==2) {
          newVal = (Macro::SettingsButton3[setting]+value)%2;
          Macro::SettingsButton3[setting]=newVal;
          lpd::sendFloat("MacroButton00-3",newVal);
      }
      if (button==3) {
          newVal = (Macro::SettingsButton4[setting]+value)%2;
          Macro::SettingsButton4[setting]=newVal;
          lpd::sendFloat("MacroButton00-4",newVal);
      }
    }
  }
}
void MC::MacroChange(int enc, int diff) {
  int setting=0;
  int newVal;
  if (setting==0) {
    if (enc==0) {
        newVal = std::min(254, std::max(Macro::Settings1[setting]+diff, 0));
        Macro::Settings1[setting]=newVal;
        lpd::sendFloat("Macro00-1",newVal);

    }
    if (enc==1) {
        newVal = std::min(254, std::max(Macro::Settings2[setting]+diff, 0));
        Macro::Settings2[setting]=newVal;
        lpd::sendFloat("Macro00-2",newVal);
    }
    if (enc==2) {
        newVal = std::min(254, std::max(Macro::Settings3[setting]+diff, 0));
        Macro::Settings3[setting]=newVal;
        lpd::sendFloat("Macro00-3",newVal);
    }
    if (enc==3) {
        newVal = std::min(254, std::max(Macro::Settings4[setting]+diff, 0));
        Macro::Settings4[setting]=newVal;
        lpd::sendFloat("Macro00-4",newVal);
    }
  }
}

void MC::FxChange(int enc, int diff) {
  if (Fx::Set == 4) {
    if (enc==3) {
      MC::BPM = std::min(254, std::max(MC::BPM+diff, 0));
      lpd::sendFloat("Main-BPM",MC::BPM);
    }
  } else {
    MC::mode=mode_FX;
    returnTimer=returnTime;
    int newVal;
    if (Fx::Set > 0 && Fx::Set < 16) {
      if (enc==0) {
        int setting=Fx::Set*2 -2;
        // int multiplier = SDL_GetTicks() - fx::deltaTime1[setting]; 
        // if ((multiplier > 20) && (multiplier < 1000)) {
        //   diff *= (int) (500/multiplier);
        // }
        // fx::deltaTime1[setting] = SDL_GetTicks();
        newVal = std::min(254, std::max(Fx::Settings1[setting]+diff, 0));
        Fx::Settings1[setting]=newVal;

        if (Fx::Set==1) lpd::sendFloat("L1-boost",newVal);
        if (Fx::Set==2) lpd::sendFloat("L1-sideIn",newVal);
        if (Fx::Set==3) lpd::sendFloat("L1-EQfreq",newVal);
        if (Fx::Set==5) lpd::sendFloat("L1-dist",newVal);
        if (Fx::Set==6) lpd::sendFloat("L1-bitCrush",newVal);
        if (Fx::Set==7) lpd::sendFloat("L1-delay",newVal);
        if (Fx::Set==8) lpd::sendFloat("L1-verb",newVal);
      }
      if (enc==1) {
        int setting=Fx::Set*2 -2;
        // int multiplier = SDL_GetTicks() - fx::deltaTime2[setting]; 
        // if ((multiplier > 10) && (multiplier < 1000)) {
        //   diff *= (int) (200/multiplier);
        // }
        // fx::deltaTime2[setting] = SDL_GetTicks();
        newVal = std::min(254, std::max(Fx::Settings2[setting]+diff, 0));
        Fx::Settings2[setting]=newVal;
        if (Fx::Set==1) lpd::sendFloat("L2-boost",newVal);
        if (Fx::Set==2) lpd::sendFloat("L2-sideIn",newVal);
        if (Fx::Set==3) lpd::sendFloat("L2-EQfreq",newVal);
        if (Fx::Set==5) lpd::sendFloat("L2-dist",newVal);
        if (Fx::Set==6) lpd::sendFloat("L2-bitCrush",newVal);
        if (Fx::Set==7) lpd::sendFloat("L2-delay",newVal);
        if (Fx::Set==8) lpd::sendFloat("L2-verb",newVal);
      }
      if (enc==2) {
        int setting=Fx::Set*2 -2;
        // int multiplier = SDL_GetTicks() - fx::deltaTime3[setting]; 
        // if ((multiplier > 100) && (multiplier < 1000)) {
        //   diff *= (int) (1000/multiplier);
        // }
        // fx::deltaTime3[setting] = SDL_GetTicks();
        newVal = std::min(254, std::max(Fx::Settings3[setting]+diff, 0));
        Fx::Settings3[setting]=newVal;
        if (Fx::Set==1) lpd::sendFloat("L3-boost",newVal);
        if (Fx::Set==2) lpd::sendFloat("L3-sideIn",newVal);
        if (Fx::Set==3) lpd::sendFloat("L3-EQfreq",newVal);
        if (Fx::Set==5) lpd::sendFloat("L3-dist",newVal);
        if (Fx::Set==6) lpd::sendFloat("L3-bitCrush",newVal);
        if (Fx::Set==7) lpd::sendFloat("L3-delay",newVal);
        if (Fx::Set==8) lpd::sendFloat("L3-verb",newVal);
      }
      if (enc==3) {
        int setting=Fx::Set*2 -2;
        // int multiplier = SDL_GetTicks() - fx::deltaTime4[setting]; 
        // if ((multiplier > 100) && (multiplier < 1000)) {
        //   diff *= (int) (1000/multiplier);
        // }
        // fx::deltaTime4[setting] = SDL_GetTicks();
        newVal = std::min(254, std::max(Fx::Settings4[setting]+diff, 0));
        Fx::Settings4[setting]=newVal;
        if (Fx::Set==1) lpd::sendFloat("L4-boost",newVal);
        if (Fx::Set==2) lpd::sendFloat("L4-sideIn",newVal);
        if (Fx::Set==3) lpd::sendFloat("L4-EQfreq",newVal);
        if (Fx::Set==5) lpd::sendFloat("L4-dist",newVal);
        if (Fx::Set==6) lpd::sendFloat("L4-bitCrush",newVal);
        if (Fx::Set==7) lpd::sendFloat("L4-delay",newVal);
        if (Fx::Set==8) lpd::sendFloat("L4-verb",newVal);
      }

      if (enc==4) {
        int setting=Fx::Set*2 -1;
        // int multiplier = SDL_GetTicks() - fx::deltaTime1[setting]; 
        // if ((multiplier > 100) && (multiplier < 1000)) {
        //   diff *= (int) (1000/multiplier);
        // }
        // fx::deltaTime1[setting] = SDL_GetTicks();
        newVal = std::min(254, std::max(Fx::Settings1[setting]+diff, 0));
        Fx::Settings1[setting]=newVal;
        if (Fx::Set==1) lpd::sendFloat("L1-compress",newVal);
        if (Fx::Set==2) lpd::sendFloat("L1-sideOut",newVal);
        if (Fx::Set==3) lpd::sendFloat("L1-EQgain",newVal);
        if (Fx::Set==5) lpd::sendFloat("L1-distCol",newVal);
        if (Fx::Set==6) lpd::sendFloat("L1-bitSampl",newVal);
        if (Fx::Set==7) {
          if (newVal < 128) {
            int timestep = MC::getTimeStep(newVal);
            lpd::sendFloat("L1-dsync",timestep);
            Fx::DelayTime[1]=timestep;
          } else {
            Fx::DelayTime[1]=newVal;
            lpd::sendFloat("L1-dsync",newVal);
          }
        }
        if (Fx::Set==8) lpd::sendFloat("L1-verbBright",newVal);
      }
      if (enc==5) {
        int setting=Fx::Set*2 -1;
        // int multiplier = SDL_GetTicks() - fx::deltaTime2[setting]; 
        // if ((multiplier > 100) && (multiplier < 1000)) {
        //   diff *= (int) (1000/multiplier);
        // }
        // fx::deltaTime2[setting] = SDL_GetTicks();
        newVal = std::min(254, std::max(Fx::Settings2[setting]+diff, 0));
        Fx::Settings2[setting]=newVal;
        if (Fx::Set==1) lpd::sendFloat("L2-compress",newVal);
        if (Fx::Set==2) lpd::sendFloat("L2-sideOut",newVal);
        if (Fx::Set==3) lpd::sendFloat("L2-EQgain",newVal);
        if (Fx::Set==5) lpd::sendFloat("L2-distCol",newVal);
        if (Fx::Set==6) lpd::sendFloat("L2-bitSampl",newVal);
        if (Fx::Set==7) {
          if (newVal < 128) {
            int timestep = MC::getTimeStep(newVal);
            lpd::sendFloat("L2-dsync",timestep);
            Fx::DelayTime[2]=timestep;
          } else {
            Fx::DelayTime[2]=newVal;
            lpd::sendFloat("L2-dsync",newVal);
          }
        }
        if (Fx::Set==8) lpd::sendFloat("L2-verbBright",newVal);
      }
      if (enc==6) {
        int setting=Fx::Set*2 -1;
        // int multiplier = SDL_GetTicks() - fx::deltaTime3[setting]; 
        // if ((multiplier > 100) && (multiplier < 1000)) {
        //   diff *= (int) (1000/multiplier);
        // }
        // fx::deltaTime3[setting] = SDL_GetTicks();
        newVal = std::min(254, std::max(Fx::Settings3[setting]+diff, 0));
        Fx::Settings3[setting]=newVal;
        if (Fx::Set==1) lpd::sendFloat("L3-compress",newVal);
        if (Fx::Set==2) lpd::sendFloat("L3-sideOut",newVal);
        if (Fx::Set==3) lpd::sendFloat("L3-EQgain",newVal);
        if (Fx::Set==5) lpd::sendFloat("L3-distCol",newVal);
        if (Fx::Set==6) lpd::sendFloat("L3-bitSampl",newVal);
        if (Fx::Set==7) {
          if (newVal < 128) {
            int timestep = MC::getTimeStep(newVal);
            lpd::sendFloat("L3-dsync",timestep);
            Fx::DelayTime[3]=timestep;
          } else {
            Fx::DelayTime[3]=newVal;
            lpd::sendFloat("L3-dsync",newVal);
          }
        }
        if (Fx::Set==8) lpd::sendFloat("L3-verbBright",newVal);
      }
      if (enc==7) {
        int setting=Fx::Set*2 -1;
        // int multiplier = SDL_GetTicks() - fx::deltaTime4[setting]; 
        // if ((multiplier > 100) && (multiplier < 1000)) {
        //   diff *= (int) (1000/multiplier);
        // }
        // fx::deltaTime4[setting] = SDL_GetTicks();
        newVal = std::min(254, std::max(Fx::Settings4[setting]+diff, 0));
        Fx::Settings4[setting]=newVal;
        if (Fx::Set==1) lpd::sendFloat("L4-compress",newVal);
        if (Fx::Set==2) lpd::sendFloat("L4-sideOut",newVal);
        if (Fx::Set==3) lpd::sendFloat("L4-EQgain",newVal);
        if (Fx::Set==5) lpd::sendFloat("L4-distCol",newVal);
        if (Fx::Set==6) lpd::sendFloat("L4-bitSampl",newVal);
        if (Fx::Set==7) {
          if (newVal < 128) {
            int timestep = MC::getTimeStep(newVal);
            lpd::sendFloat("L4-dsync",timestep);
            Fx::DelayTime[4]=timestep;
          } else {
            Fx::DelayTime[4]=newVal;
            lpd::sendFloat("L4-dsync",newVal);
          }
        }
        if (Fx::Set==8) lpd::sendFloat("L4-verbBright",newVal);
      }
    }
  }
}
void MC::ArcadeDown(int button) {
   if (arcade::pitch) {
      for (int b=0;b<16;b++) {
        if (arcade::positions[b] == button) {
          if (arcade::ch1) {
            lpd::sendFloat("pitchWrite1",1);
            lpd::sendFloat("Pshift1", arcade::octave*12+arcade::notes[b]-36);
          }
          if (arcade::ch2) {
            lpd::sendFloat("pitchWrite2",1);
            lpd::sendFloat("Pshift2", arcade::octave*12+arcade::notes[b]-36);
          }
          if (arcade::ch3) {
            lpd::sendFloat("pitchWrite3",1);
            lpd::sendFloat("Pshift3", arcade::octave*12+arcade::notes[b]-36);
          }
          if (arcade::ch4) {
            lpd::sendFloat("pitchWrite4",1);
            lpd::sendFloat("Pshift4", arcade::octave*12+arcade::notes[b]-36);
          }
        }
      }
   }
   if (arcade::edit) {
    for (int b=0;b<16;b++) {
      if (arp::positions[b] == button) {
        int step=b/4;
        int value=b-step*4+1;
        if (arp::patternSteps[step]==value) {
          value=0;
        }
        arp::patternSteps[step]=value;
        if (step==0) lpd::sendFloat("PatternStep1",value);
        if (step==1) lpd::sendFloat("PatternStep2",value);
        if (step==2) lpd::sendFloat("PatternStep3",value);
        if (step==3) lpd::sendFloat("PatternStep4",value);
      }
    }
   }
   if (!arcade::edit && !arcade::pitch) {
    for (int b=0;b<16;b++) {
      if (arcade::positions[b] == button) {
        if (arcade::hold) {
          for (int c=0;c<16;c++) {
            if (!arcade::down[c]) {
              if (!arcade::shift) {
                arcade::held[c]=false;
                MC::notesRemove(arcade::notes[c]);
                MC::notesRemove2(arcade::notes[c]);
              }
            }
          }
          if (arcade::held[b]) {
              if (arcade::shift) {
                arcade::held[b]=false;
                MC::notesRemove(arcade::notes[b]);
                MC::notesRemove2(arcade::notes[b]);
              }
          } else {
            arcade::held[b]=true;
            MC::notesAdd(arcade::notes[b]);
            MC::notesAdd2(arcade::notes[b]);
          }
        } else {
          for (int c=0;c<16;c++) {
            if (!arcade::down[c]) {
              arcade::held[c]=false;
              MC::notesRemove(arcade::notes[c]);
              MC::notesRemove2(arcade::notes[c]);
            }
          }
          MC::notesAdd(arcade::notes[b]);
          MC::notesAdd2(arcade::notes[b]);
        }
        arcade::down[b]=true;
      }
    }
    //->pd send arcade on b
    //lpd::sendFloat("NoteOn", arcade::notes[b]);
    //lpd::sendFloat("NotesMono",MC::notesLast());
    lpd::sendList("NotesPoly",notesList);
    lpd::sendList("listPoly",polyList);
   }

  //omni buttons (in all edit modes)
  switch (button)  {
    case 0:
      arcade::shift=true;
      controller::setArcade(button,2);
      controller::setArcadePattern(button,2);
      break;
    case 24:
      if (arcade::hold) {
        arcade::hold=false;
        controller::setArcade(button,0);
        controller::setArcadePattern(button,0);
        for (int i=0; i < 16;i++) {
          if (arcade::held[i]==true) {
            //controller::setArcade(arcade::positions[i],0);
            //lpd::sendFloat("NoteOff", arcade::notes[i]);
            //MC::notesRemove(arcade::notes[i]);
            arcade::held[i]=false;
          }
        }
        notesList.clear();
        for (int i=0; i < 16;i++) {
          if (arcade::down[i]==true) {
            //controller::setArcade(arcade::positions[i],4);
            MC::notesAdd(arcade::notes[i]);
            MC::notesAdd2(arcade::notes[i]);
            arcade::held[i]=true;
          }
        }
        //lpd::sendFloat("NotesMono",MC::notesLast());
        lpd::sendList("NotesPoly",notesList);
        lpd::sendList("listPoly",polyList);

      } else {
        arcade::hold=true;
        controller::setArcade(button,4);
        controller::setArcadePattern(button,4);
        for (int i=0; i < 16;i++) {
          if (arcade::down[i]) {
            //controller::setArcade(arcade::positions[i],4);
            arcade::held[i]=true;
          }
        }
        //lpd::sendFloat("NotesMono",MC::notesLast());
        lpd::sendList("NotesPoly",notesList);
        lpd::sendList("listPoly",polyList);
      }
      break;
    case 35:
      if (arcade::rec) {
        arcade::rec=false;
        MC::recstart[1]=-1;
        MC::recstart[2]=-1;
        MC::recstart[3]=-1;
        MC::recstart[4]=-1;
        controller::setArcade(button,0);
        controller::setArcadePattern(button,0);
        lpd::sendFloat("looprec",0);
      } else {
        arcade::rec=true;
        
        if (arcade::ch1) MC::recstart[1]=MC::loopIndex;
        if (arcade::ch2) MC::recstart[2]=MC::loopIndex;
        if (arcade::ch3) MC::recstart[3]=MC::loopIndex;
        if (arcade::ch4) MC::recstart[4]=MC::loopIndex;
        controller::setArcade(button,1);
        controller::setArcadePattern(button,1);
        lpd::sendFloat("looprec",1);
      }
      break;
    case 12:
      if (arcade::arp) {
        arcade::arp=false;
        controller::setArcade(button,0);
        controller::setArcadePattern(button,0);
        lpd::sendFloat("ArpMode",0);
        lpd::sendList("NotesPoly",notesList);
        lpd::sendList("listPoly",polyList);

      } else {
        arcade::arp=true;
        controller::setArcade(button,3);
        controller::setArcadePattern(button,3);
        lpd::sendFloat("ArpMode",1);
      }
      break;
    case 18:
      if (arcade::chord) {
        arcade::chord=false;
        controller::setArcade(button,0);
        controller::setArcadePattern(button,0);
        lpd::sendFloat("ChordMode",0);
      } else {
        arcade::chord=true;
        controller::setArcade(button,6);
        controller::setArcadePattern(button,6);
        lpd::sendFloat("ChordMode",1);

      }
      break;
    case 6:
      if (arcade::edit) {
        arcade::edit=false;
        controller::setArcade(button,0);
      } else {
        arcade::edit=true;
        controller::setArcadePattern(button,3);
        if (!arcade::hold) {
          notesList.clear();
          lpd::sendList("NotesPoly",notesList);
          lpd::sendList("listPoly",polyList);
        }

      }
      break;    
          
  }
  //pattern mode
  if (arcade::edit) {
      switch (button) {
        case 31:
          if (arp::patternRepeat[0]) {
            arp::patternRepeat[0]=false;
            lpd::sendFloat("PatternRepeat1",0);

          } else {
            arp::patternRepeat[0]=true;
            lpd::sendFloat("PatternRepeat1",1);
          }
          break;
        case 32:
          if (arp::patternRepeat[1]) {
            arp::patternRepeat[1]=false;
            lpd::sendFloat("PatternRepeat2",0);
          } else {
            arp::patternRepeat[1]=true;
            lpd::sendFloat("PatternRepeat2",1);
          }
          break;
        case 33:
          if (arp::patternRepeat[2]) {
            arp::patternRepeat[2]=false;
            lpd::sendFloat("PatternRepeat3",0);
          } else {
            arp::patternRepeat[2]=true;
            lpd::sendFloat("PatternRepeat3",1);
          }
          break;
        case 34:
          if (arp::patternRepeat[3]) {
            arp::patternRepeat[3]=false;
            lpd::sendFloat("PatternRepeat4",0);
          } else {
            arp::patternRepeat[3]=true;
            lpd::sendFloat("PatternRepeat4",1);
          }
          break;
        case 1:
          if (arp::pattern[0]) {
            arp::pattern[0]=false;
            lpd::sendFloat("PatternOn1",0);
          } else {
            arp::pattern[0]=true;
            lpd::sendFloat("PatternOn1",1);
          }
          break;
        case 2:
          if (arp::pattern[1]) {
            arp::pattern[1]=false;
            lpd::sendFloat("PatternOn2",0);
          } else {
            arp::pattern[1]=true;
            lpd::sendFloat("PatternOn2",1);
          }
          break;
        case 3:
          if (arp::pattern[2]) {
            arp::pattern[2]=false;
            lpd::sendFloat("PatternOn3",0);
          } else {
            arp::pattern[2]=true;
            lpd::sendFloat("PatternOn3",1);
          }
          break;
        case 4:
          if (arp::pattern[3]) {
            arp::pattern[3]=false;
            lpd::sendFloat("PatternOn4",0);
          } else {
            arp::pattern[3]=true;
            lpd::sendFloat("PatternOn4",1);
          }
          break;
      case 29:
        arp::patternSteps[0]=4;
        arp::patternSteps[1]=4;
        arp::patternSteps[2]=4;
        arp::patternSteps[3]=4;
        lpd::sendFloat("PatternStep1",4);
        lpd::sendFloat("PatternStep2",4);
        lpd::sendFloat("PatternStep3",4);
        lpd::sendFloat("PatternStep4",4);
        break;
      case 23:
        arp::patternSteps[0]=3;
        arp::patternSteps[1]=3;
        arp::patternSteps[2]=3;
        arp::patternSteps[3]=3;
        lpd::sendFloat("PatternStep1",3);
        lpd::sendFloat("PatternStep2",3);
        lpd::sendFloat("PatternStep3",3);
        lpd::sendFloat("PatternStep4",3);
        break;
      case 17:
        arp::patternSteps[0]=2;
        arp::patternSteps[1]=2;
        arp::patternSteps[2]=2;
        arp::patternSteps[3]=2;
        lpd::sendFloat("PatternStep1",2);
        lpd::sendFloat("PatternStep2",2);
        lpd::sendFloat("PatternStep3",2);
        lpd::sendFloat("PatternStep4",2);
        break;
      case 11:
        arp::patternSteps[0]=1;
        arp::patternSteps[1]=1;
        arp::patternSteps[2]=1;
        arp::patternSteps[3]=1;
        lpd::sendFloat("PatternStep1",1);
        lpd::sendFloat("PatternStep2",1);
        lpd::sendFloat("PatternStep3",1);
        lpd::sendFloat("PatternStep4",1);
        break;
      case 5:
        arp::pattern[0]=true;
        lpd::sendFloat("PatternOn1",1);
        arp::pattern[1]=true;
        lpd::sendFloat("PatternOn2",1);
        arp::pattern[2]=true;
        lpd::sendFloat("PatternOn3",1);
        arp::pattern[3]=true;
        lpd::sendFloat("PatternOn4",1);
        break;
      }
  }
  //normal mode
  if (!arcade::edit) {
    switch (button) {
      case 30:
        arcade::pitch=true;
        controller::setArcade(button,8);
        break;

      case 1:
        if (arcade::ch1 || arcade::ch2 || arcade::ch3 || arcade::ch4) {
          if (arcade::shift) {
            if (arcade::ch1) MC::loopSize[1]=12;
            if (arcade::ch2) MC::loopSize[2]=12;
            if (arcade::ch3) MC::loopSize[3]=12;
            if (arcade::ch4) MC::loopSize[4]=12;
            lpd::sendFloat("step",12);
            controller::setArcade(button,2);
          } else {
            if (arcade::ch1) MC::loopSize[1]=96;
            if (arcade::ch2) MC::loopSize[2]=96;
            if (arcade::ch3) MC::loopSize[3]=96;
            if (arcade::ch4) MC::loopSize[4]=96;
            controller::setArcade(button,2);
            lpd::sendFloat("step",96);
          }
        }
        break;
      case 2:
        if (arcade::ch1 || arcade::ch2 || arcade::ch3 || arcade::ch4) {
          if (arcade::shift) {
            if (arcade::ch1) MC::loopSize[1]=24;
            if (arcade::ch2) MC::loopSize[2]=24;
            if (arcade::ch3) MC::loopSize[3]=24;
            if (arcade::ch4) MC::loopSize[4]=24;
            lpd::sendFloat("step",24);
            controller::setArcade(button,2);
          } else {
            if (arcade::ch1) MC::loopSize[1]=192;
            if (arcade::ch2) MC::loopSize[2]=192;
            if (arcade::ch3) MC::loopSize[3]=192;
            if (arcade::ch4) MC::loopSize[4]=192;
            lpd::sendFloat("step",192);
            controller::setArcade(button,2);
          }
        }
        break;
      case 3:
        if (arcade::ch1 || arcade::ch2 || arcade::ch3 || arcade::ch4) {
          if (arcade::shift) {
            if (arcade::ch1) MC::loopSize[1]=48;
            if (arcade::ch2) MC::loopSize[2]=48;
            if (arcade::ch3) MC::loopSize[3]=48;
            if (arcade::ch4) MC::loopSize[4]=48;
            lpd::sendFloat("step",48);
            controller::setArcade(button,2);
          } else {
            if (arcade::ch1) MC::loopSize[1]=384;
            if (arcade::ch2) MC::loopSize[2]=384;
            if (arcade::ch3) MC::loopSize[3]=384;
            if (arcade::ch4) MC::loopSize[4]=384;
            lpd::sendFloat("step",384);
            controller::setArcade(button,2);
          }
        }
        break;
      case 4:
        if (arcade::ch1 || arcade::ch2 || arcade::ch3 || arcade::ch4) {
          if (arcade::shift) {
            if (arcade::ch1) MC::loopSize[1]=96;
            if (arcade::ch2) MC::loopSize[2]=96;
            if (arcade::ch3) MC::loopSize[3]=96;
            if (arcade::ch4) MC::loopSize[4]=96;
            lpd::sendFloat("step",96);
            controller::setArcade(button,2);
          } else {
            if (arcade::ch1) MC::loopSize[1]=768;
            if (arcade::ch2) MC::loopSize[2]=768;
            if (arcade::ch3) MC::loopSize[3]=768;
            if (arcade::ch4) MC::loopSize[4]=768;
            lpd::sendFloat("step",768);
            controller::setArcade(button,2);
          }
        }
        break;

      case 31:
        if (arcade::ch1) {
          arcade::ch1=false;
          controller::setArcade(button,0);
          lpd::sendFloat("L1-active",0);
          if (arcade::rec) MC::recstart[1]=-1;
        } else {
          arcade::ch1=true;
          controller::setArcade(button,7);
          lpd::sendFloat("L1-active",1);
          if (arcade::rec) MC::recstart[1]=MC::loopIndex;
        }
        break;
      case 32:
        if (arcade::ch2) {
          arcade::ch2=false;
          controller::setArcade(button,0);
          lpd::sendFloat("L2-active",0);
          if (arcade::rec) MC::recstart[2]=-1;
        } else {
          arcade::ch2=true;
          controller::setArcade(button,7);
          lpd::sendFloat("L2-active",1);
          if (arcade::rec) MC::recstart[2]=MC::loopIndex;
        }
        break;
      case 33:
        if (arcade::ch3) {
          arcade::ch3=false;
          controller::setArcade(button,0);
          lpd::sendFloat("L3-active",0);
          if (arcade::rec) MC::recstart[3]=-1;
        } else {
          arcade::ch3=true;
          controller::setArcade(button,7);
          lpd::sendFloat("L3-active",1);
          if (arcade::rec) MC::recstart[3]=MC::loopIndex;
        }
        break;
      case 34:
        if (arcade::ch4) {
          arcade::ch4=false;
          controller::setArcade(button,0);
          lpd::sendFloat("L4-active",0);
          if (arcade::rec) MC::recstart[4]=-1;
        } else {
          arcade::ch4=true;
          controller::setArcade(button,7);
          lpd::sendFloat("L4-active",1);
          if (arcade::rec) MC::recstart[4]=MC::loopIndex;
        }
        break;      
      case 29:
        arcade::octave=1;
        lpd::sendFloat("octave",1);
        controller::setArcade(29,7);
        controller::setArcade(23,0);
        controller::setArcade(17,0);
        controller::setArcade(11,0);
        controller::setArcade(5,0);
        break;
      case 23:
        arcade::octave=2;
        lpd::sendFloat("octave",2);
        controller::setArcade(29,0);
        controller::setArcade(23,7);
        controller::setArcade(17,0);
        controller::setArcade(11,0);
        controller::setArcade(5,0);
        break;
      case 17:
        arcade::octave=3;
        lpd::sendFloat("octave",3);
        controller::setArcade(29,0);
        controller::setArcade(23,0);
        controller::setArcade(17,7);
        controller::setArcade(11,0);
        controller::setArcade(5,0);
        break;
      case 11:
        arcade::octave=4;
        lpd::sendFloat("octave",4);
        controller::setArcade(29,0);
        controller::setArcade(23,0);
        controller::setArcade(17,0);
        controller::setArcade(11,7);
        controller::setArcade(5,0);
        break;
      case 5:
        arcade::octave=5;
        lpd::sendFloat("octave",5);
        controller::setArcade(29,0);
        controller::setArcade(23,0);
        controller::setArcade(17,0);
        controller::setArcade(11,0);
        controller::setArcade(5,7);
        break;
 

     }
  }

}
void MC::ArcadeUp(int button) {
  if (!arcade::edit) {
   if (arcade::pitch) {
      lpd::sendFloat("pitchWrite1",0);
      lpd::sendFloat("pitchWrite2",0);
      lpd::sendFloat("pitchWrite3",0);
      lpd::sendFloat("pitchWrite4",0);
   }
    for (int b=0;b<16;b++) {
      if (arcade::positions[b] == button) {
        arcade::down[b]=false;
        if (!arcade::hold) {
          //controller :: setArcade(arcade::positions[b],0);
          //->pd send arcade off b
          //lpd::sendFloat("NoteOff", arcade::notes[b]);
          MC::notesRemove(arcade::notes[b]);
          MC::notesRemove2(arcade::notes[b]);
          //lpd::sendFloat("NotesMono",MC::notesLast());
          lpd::sendList("NotesPoly",notesList);
          lpd::sendList("listPoly",polyList);


        } 
      }
    }
   }

  switch (button){
    case 0:
      arcade::shift=false;
      controller::setArcade(button,0);
      controller::setArcadePattern(button,0);
      //controller::setArcadePattern(button,0);
      break;
    case 30:
        arcade::pitch=false;
        controller::setArcade(button,0);
        lpd::sendFloat("pitchWrite1",0);
        lpd::sendFloat("pitchWrite2",0);
        lpd::sendFloat("pitchWrite3",0);
        lpd::sendFloat("pitchWrite4",0);
        break;

    case 1:
      if (!arcade::edit) {
         controller::setArcade(button,0);
      }
      break;
    case 2:
      if (!arcade::edit) {
         controller::setArcade(button,0);
      }
      break;
    case 3:
      if (!arcade::edit) {
         controller::setArcade(button,0);
      }
      break;
    case 4:
      if (!arcade::edit) {
         controller::setArcade(button,0);
      }
      break;

    case 31: //ch1
    case 32: //ch2
    case 33: //ch3
    case 34: //ch4
    case 35: //rec
    case 5:  //oct5
    case 11: //oct4
    case 17: //oct3
    case 23: //oct3
    case 29: //oct1
    case 24: //hold
    case 12: //arp
    case 18: //repeat
      break;

  }
}
void MC::keyPadUp(int key) {
  lastkey=32;
}
void MC::keyPadDown(int key) {
  switch (key) {
    case key_drums:
      MC::source=source_drums;
      lpd::sendFloat("source",source_drums);
      break;
    case key_synth:
      MC:source=source_synth;
      lpd::sendFloat("source",source_synth);
      break;
    case key_Up:
      setKeyRepeat(key);
      if (MC::mode==mode_File) {
        gui::dirUp(1);
        std::cout << "Keypad Up pressed" << std::endl;
      }
      break;

    case key_Down:
      setKeyRepeat(key);
      if (MC::mode==mode_File) {
      gui::dirDown(1);
      }
      std::cout << "Keypad Down pressed" << std::endl;
      break;
    
    case key_Left:
      if (MC::mode==mode_File) {
        gui::dirStart();
      }
      std::cout << "Keypad Left pressed" << std::endl;
      break;

    case key_Right:
      if (MC::mode==mode_File) {
        gui::dirEnd();
      }
      std::cout << "Keypad Right pressed" << std::endl;
      break;

    case key_Mode_Loop:
      MC::mode=mode_Loop;
      break;

    case key_Mode_File:
      MC::mode=mode_File;
      break;

    case key_Mode_FX:
      MC::mode=mode_FX;
      break;

    case key_Mode_Test2:
      MC::mode=mode_Test2;
      break;

    case key_Enter:
      if (MC::mode==mode_File) {
        if (fs::directoryContents[fs::dirIndex].isDir) {
          std::cout << "load directory:" << fs::directoryContents[fs::dirIndex].filename << std::endl;
          std::string dirNow = fs::directoryCurrent;
          if (fs::directoryContents[fs::dirIndex].filename == "..") {
            std::size_t found =  fs::directoryCurrent.find_last_of("/\\");
            fs::directoryCurrent =  fs::directoryCurrent.substr(0,found);
          } else {
            fs::directoryCurrent += "/" + fs::directoryContents[fs::dirIndex].filename;
          }
          if (fs::getDirectory() != 0) {
            std::cout << "error opening directory: " <<  fs::directoryCurrent << std::endl;
            fs::directoryCurrent = dirNow;
          } else {
            fs::dirIndex=0;
          }
        } else {
          std::cout << "load file:" << fs::directoryContents[fs::dirIndex].filename << std::endl;
        }
      }
      break;
    
    case key_Load_Patch:
      //audio::loadPatch();
      break;

  }

  if (MC::debug) std::cout << "Keypad " << key << " pressed" << std::endl;
}
void MC::PitchBend(int value) {
  if (MC::mode==mode_File) {
     if (value > 128) {
       pbscroll+=(value-128) / 600.0;
     }
     if (value < 128) {
       pbscroll+=(value-128) / 600.0;
     }
     if (value == 128) {
       //pbscroll=0;
     }
     if (pbscroll > 1) {
       gui::dirDown(1);
       pbscroll=0;
     }
     if (pbscroll < -1) {
       gui::dirUp(1);
       pbscroll=0;
     }
  }
}
void MC::processControllerInput () {
     //parse the control buffer and send to PD if changed
  for (int m = 0; m < 3; m++) {
    for (int n = 0; n < 6; n++) {
      int mask = 1 << n;
      int masked_n = MC::controlBuffer[m * 2 + 1] & mask;
      bool newvalue = masked_n > 0;
      if (newvalue != inputArcadeButtons[m*12+n]) {
        //send to pd
        if (MC::debug) std::cout << "arcadeButton " << m*12+n << ": " << newvalue << std::endl;
        inputArcadeButtons[m*12+n]=newvalue;
        if (newvalue==1) {
          MC::ArcadeDown(m*12+n);
        } else {
          MC::ArcadeUp(m*12+n);
        }
      } 
    }
    for (int n = 0; n < 6; n++) {
      int mask = 1 << n;
      int masked_n = MC::controlBuffer[m * 2 + 2] & mask;
      bool newvalue = masked_n > 0;
      if (newvalue != inputArcadeButtons[m*12+6+n]) {
        //send to pd
        if (MC::debug) std::cout << "arcadeButton " << m*12+6+n << ": " << newvalue << std::endl;
        inputArcadeButtons[m*12+6+n] = newvalue;
        if (newvalue==1) {
          MC::ArcadeDown(m*12+6+n);
        } else {
          MC::ArcadeUp(m*12+6+n);
        }
      }
    }
    for (int n = 0; n < 4; n++) {
      int mask = 1 << n;
      int masked_n = MC::controlBuffer[m + 7] & mask;
      bool newvalue = masked_n > 0;
      int k = m*4+n;
      if (newvalue != inputEncodersButtons[k]) {
        //send to pd
        if (MC::debug) std::cout << "encoderButton " << k << ": " << newvalue << std::endl;

        if (k==0 && newvalue==1) {
          controller::setLed(Fx::Set-1,0);
          Fx::Set=1;
          MC::mode=mode_FX;
          returnTimer=returnTime;
          controller::setLed(Fx::Set-1,1);
        }
        if (k==1 && newvalue==1) { 
          controller::setLed(Fx::Set-1,0);
          Fx::Set=2;
          MC::mode=mode_FX;
          returnTimer=returnTime;
          controller::setLed(Fx::Set-1,1);
        }
        if (k==2 && newvalue==1) { 
          controller::setLed(Fx::Set-1,0);
          Fx::Set=3;
          MC::mode=mode_FX;
          returnTimer=returnTime;
          controller::setLed(Fx::Set-1,1);
        }
        if (k==3 && newvalue==1) { 
          controller::setLed(Fx::Set-1,0);
          Fx::Set=4;
          controller::setLed(Fx::Set-1,1);
        }
        if (k==4 && newvalue==1) { 
          controller::setLed(Fx::Set-1,0);
          Fx::Set=5;
          MC::mode=mode_FX;
          returnTimer=returnTime;
          controller::setLed(Fx::Set-1,1);
        }
        if (k==5 && newvalue==1) { 
          controller::setLed(Fx::Set-1,0);
          Fx::Set=6;
          MC::mode=mode_FX;
          returnTimer=returnTime;
          controller::setLed(Fx::Set-1,1);
        }
        if (k==6 && newvalue==1) { 
          controller::setLed(Fx::Set-1,0);
          Fx::Set=7;
          MC::mode=mode_FX;
          returnTimer=returnTime;
          controller::setLed(Fx::Set-1,1);
        }
        if (k==7 && newvalue==1) { 
          controller::setLed(Fx::Set-1,0);
          Fx::Set=8;
          MC::mode=mode_FX;
          returnTimer=returnTime;
          controller::setLed(Fx::Set-1,1);
        }


        if (!MC::pitchFollow[1]) {
          if (k==8 && newvalue==1) {
            lpd::sendFloat("Pshift1", 0);
            inputEncoders[k] = 0;
          }
        }
        if (!MC::pitchFollow[2]) {
          if (k==9 && newvalue==1) {
            lpd::sendFloat("Pshift2", 0);
            inputEncoders[k] = 0;
          }
        }
        if (!MC::pitchFollow[3]) {
          if (k==10 && newvalue==1) {
            lpd::sendFloat("Pshift3", 0);
            inputEncoders[k] = 0;
          }
        }
        if (!MC::pitchFollow[4]) {
          if (k==11 && newvalue==1) {
            lpd::sendFloat("Pshift4", 0);
            inputEncoders[k] = 0;
          }
        }
        if (inputMixerButtons[2]) {
            if (k==8 && newvalue==1) {
              if (MC::pitchFollow[1]) {
                MC::pitchFollow[1]=false;
                controller::setLed(k,0);
                lpd::sendFloat("L1-pitchFollow",0);
              } else {
                MC::pitchFollow[1]=true;
                controller::setLed(k,1);
                lpd::sendFloat("L1-pitchFollow",1);
              }
            }
            if (k==9 && newvalue==1) {
              if (MC::pitchFollow[2]) {
                MC::pitchFollow[2]=false;
                controller::setLed(k,0);
                lpd::sendFloat("L2-pitchFollow",0);
              } else {
                MC::pitchFollow[2]=true;
                controller::setLed(k,1);
                lpd::sendFloat("L2-pitchFollow",1);
              }
            }
            if (k==10 && newvalue==1) {
              if (MC::pitchFollow[3]) {
                MC::pitchFollow[3]=false;
                controller::setLed(k,0);
                lpd::sendFloat("L3-pitchFollow",0);
              } else {
                MC::pitchFollow[3]=true;
                controller::setLed(k,1);
                lpd::sendFloat("L3-pitchFollow",1);
              }
            }
            if (k==11 && newvalue==1) {
              if (MC::pitchFollow[4]) {
                MC::pitchFollow[4]=false;
                controller::setLed(k,0);
                lpd::sendFloat("L4-pitchFollow",0);
              } else {
                MC::pitchFollow[4]=true;
                controller::setLed(k,1);
                lpd::sendFloat("L4-pitchFollow",1);
              }
            }
          
        }
        inputEncodersButtons[k] = newvalue;
        //controller::setLed(k,newvalue);
      }
    }
  }
  for (int n = 0; n < 4; n++) {
    int mask = 1 << n;
    int masked_n = MC::controlBuffer[10] & mask;
    bool newvalue = masked_n > 0;
    if (newvalue != inputMixerButtons[n]) {
      //send to pd
      if (MC::debug) std::cout << "mixerButton " << n << ": " << newvalue << std::endl;
      inputMixerButtons[n] = newvalue;

      if (n==2 && newvalue==1) {
          if (arcade::pitch) {
            if (arcade::ch1) {
              if (MC::pitchFollow[1]) {
                MC::pitchFollow[1]=false;
                lpd::sendFloat("L1-pitchFollow",0);
                controller::setLed(8,0);
              } else {
                MC::pitchFollow[1]=true;
                lpd::sendFloat("L1-pitchFollow",1);
                controller::setLed(8,1);
              }
            }
            if (arcade::ch2) {
              if (MC::pitchFollow[2]) {
                MC::pitchFollow[2]=false;
                lpd::sendFloat("L2-pitchFollow",0);
                controller::setLed(9,0);
              } else {
                MC::pitchFollow[2]=true;
                lpd::sendFloat("L2-pitchFollow",1);
                controller::setLed(9,1);
              }
            }
            if (arcade::ch3) {
              if (MC::pitchFollow[3]) {
                MC::pitchFollow[3]=false;
                lpd::sendFloat("L3-pitchFollow",0);
                controller::setLed(10,0);
              } else {
                MC::pitchFollow[3]=true;
                lpd::sendFloat("L3-pitchFollow",1);
                controller::setLed(10,1);
              }
            }
            if (arcade::ch4) {
              if (MC::pitchFollow[4]) {
                MC::pitchFollow[4]=false;
                lpd::sendFloat("L4-pitchFollow",0);
                controller::setLed(11,0);
              } else {
                MC::pitchFollow[4]=true;
                lpd::sendFloat("L4-pitchFollow",1);
                controller::setLed(11,1);
              }
            }
          }
      }

    }
  }
  for (int n = 0; n < 12; n++) {
    int newvalue = MC::controlBuffer[11+n];
    if (newvalue != 128) {
      //send to pd
      if (MC::debug) std::cout << "encoder " << n << ": " << newvalue << std::endl;
      inputEncoders[n] += (newvalue - 128);
      if (!MC::pitchFollow[1]) {
        if (n== 8) lpd::sendFloat("Pshift1", nearbyintf(inputEncoders[n] / 5));
      }
      if (!MC::pitchFollow[2]) {
        if (n== 9) lpd::sendFloat("Pshift2", nearbyintf(inputEncoders[n] / 5));
      }
      if (!MC::pitchFollow[3]) {
        if (n==10) lpd::sendFloat("Pshift3", nearbyintf(inputEncoders[n] / 5));
      }
      if (!MC::pitchFollow[4]) {
        if (n==11) lpd::sendFloat("Pshift4", nearbyintf(inputEncoders[n] / 5));
      }

      if (n < 8) {
        MC::FxChange(n,newvalue-128);
      }

    } 
  }
  for (int n = 0; n < 4; n++) {
    int newvalue = MC::controlBuffer[54+n];
    if (newvalue != 128) {
      //send to pd
      if (MC::debug) std::cout << "encoder " << n+12 << ": " << newvalue << std::endl;
      inputEncoders[n+12] = newvalue;
      MC::MacroChange(n,newvalue-128);
    } 
  }
  for (int n = 0; n < 4; n++) {
    int newvalue = MC::controlBuffer[35+n];
    if (newvalue != inputFaders[n]) {
      //send to pd
      inputFaders[n] = newvalue;
      if (n==0) lpd::sendFloat("volume1", newvalue);
      if (n==1) lpd::sendFloat("volume2", newvalue);
      if (n==2) lpd::sendFloat("volume3", newvalue);
      if (n==3) lpd::sendFloat("volume4", newvalue);

      if (MC::debug) std::cout << "fader " << n << ":" << newvalue << std::endl;
    }
  }
  for (int n = 0; n < 12; n++) {
    int newvalue = MC::controlBuffer[23+n];
    if (newvalue != inputKnobs[n]) {
      if (n==0) lpd::sendFloat("L1-gain",newvalue);
      if (n==1) lpd::sendFloat("L2-gain",newvalue);
      if (n==2) lpd::sendFloat("L3-gain",newvalue);
      if (n==3) lpd::sendFloat("L4-gain",newvalue);

      if (n==4) lpd::sendFloat("L1-freq",newvalue);
      if (n==5) lpd::sendFloat("L2-freq",newvalue);
      if (n==6) lpd::sendFloat("L3-freq",newvalue);
      if (n==7) lpd::sendFloat("L4-freq",newvalue);

      if (n==8) lpd::sendFloat("L1-reso",newvalue);
      if (n==9) lpd::sendFloat("L2-reso",newvalue);
      if (n==10) lpd::sendFloat("L3-reso",newvalue);
      if (n==11) lpd::sendFloat("L4-reso",newvalue);

      //send to pd
      if (MC::debug) std::cout << "knob " << n << ": " << newvalue << std::endl;
      inputKnobs[n] = newvalue;
    }
  }
  for (int n = 0; n < 4; n++) {
    int newvalue = MC::controlBuffer[39+n];
    if (newvalue != inputKnobs[n+12]) {
      //send to pd
      if (n==0) lpd::sendFloat("volumeMain",newvalue);
      if (n==1) lpd::sendFloat("volumeMonitor",newvalue);
      if (n==2) lpd::sendFloat("Main-freq",newvalue);
      if (n==3) lpd::sendFloat("Main-reso",newvalue);

      if (MC::debug) std::cout << "knob " << n+12 << ": " << newvalue << std::endl;
      inputKnobs[n+12] = newvalue;
    }
  }
  
  //xfader
  int newvalue = MC::controlBuffer[43];
  if (newvalue != inputFaders[4]) {
    //send to pd
    if (MC::debug) std::cout << "Fader " << 4 << " X-Fader: " << newvalue << std::endl;
    inputFaders[4] = newvalue;
    lpd::sendFloat("X-Fader",newvalue);

  }
  for (int m = 0; m < 6; m++) {  
    for (int n = 0; n < 7; n++) {
      int mask = 1 << n;
      int masked_n = MC::controlBuffer[48 + m] & mask;
      bool newvalue = masked_n > 0;
      int index = m*7+n;
      if (index < 32) {
        if (newvalue != inputKeypad[index]) {
          //send to pd
          if (newvalue==1) {
            MC::keyPadDown(index);
          } else {
            MC::keyPadUp(index);
          }
          inputKeypad[index] = newvalue;
          
        }
      }
      if (index == 32) {
        if (newvalue != inputModulation) {
          if (MC::debug) std::cout << "Modulation " << ": " << newvalue << std::endl;
          inputModulation = newvalue;
          lpd::sendFloat("Modulation",newvalue);

        }
      }
      if ((index > 32) && (index < 37)) {
        if (newvalue != inputEncodersButtons[index-32+11]) {
          if (MC::debug) std::cout << "encoderButton " << index-32+11 << ": " << newvalue << std::endl;
          inputEncodersButtons[index-32+11] = newvalue;
          
          MC::MacroButton(index-33,newvalue);
        }
      }
      if ((index > 36) && (index < 39)) {
        if (newvalue != inputFootPedal[index-37]) {
          if (MC::debug) std::cout << "footPedal " << index-37 << ": " << newvalue << std::endl;
          inputFootPedal[index-37] = newvalue;
          if (index==37) {
            lpd::sendFloat("Foot1",newvalue);
          }
          if (index==38) {
            lpd::sendFloat("Foot2",newvalue);
          }
        }
      }
    }
  }
  for (int n = 0; n < 2; n++) {
    int newvalue = MC::controlBuffer[59-n];
    if (newvalue != inputKnobs[n+16]) {
      //send to pd
      if (MC::debug) std::cout << "knob " << n+16 << ": " << newvalue << std::endl;
      inputKnobs[n+16] = newvalue;
      if (n==0) {
//        lpd::sendFloat("oscShape",newvalue);
        lpd::sendFloat("Attack",newvalue);
      }
      if (n==1) {
        lpd::sendFloat("Decay",newvalue);
        //lpd::sendFloat("arpDecay",newvalue);
        //lpd::sendFloat("oscDetune",newvalue);
      }
    }
  }
  newvalue = MC::controlBuffer[60];
  MC::PitchBend(newvalue);
  if (newvalue != inputFaders[5]) {
    //send to pd
    if (MC::debug) std::cout << "Fader " << 5 << " pitchbend : " << newvalue << std::endl;
    inputFaders[5] = newvalue;
    lpd::sendFloat("P-Bend",newvalue);
  }
}