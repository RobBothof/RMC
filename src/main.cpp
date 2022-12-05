/*
Copyright (c) 2018 Rob Bothof. All rights reserved.
Created by: Rob Bothof in july 2018
This code may not be redistributed without written permission of the creator.
*/ 


#include <unistd.h> //usleep
#include <SDL2/SDL.h>

#include <SDL2/SDL_gpu.h>

#include "audio.hpp"
#include "gui.hpp"
#include "functions.hpp"
#include "filesystem.hpp"
#include "midiLocal.hpp"
#include "midiUart.hpp"
#include "controller.hpp"
#include "masterControl.hpp"

#include <iomanip> // setprecision
#include <sstream> // stringstream



bool done; //color palette, 16 color + 3 background colors

Uint64 start = 0;
Uint64 end1 = 0;
Uint64 end2 = 0;
double deltaTime = 0;
double frametimes[20];
bool debug = false;
int drawFrame;

int isMouseEvent(void *userdata, SDL_Event *event) {
  if (event->type == SDL_MOUSEMOTION) { return 0; } else { return 1; }
} 

int main()
{
  if (fs::getDirectory() == 1) {
    std::cout << "error opening directory" << std::endl;
  }

  Uint32 lasttime;

  controller::init();
  
  //init midi
  if (midiUart::init()==1) {
     return 1;
  }
  midiLocal::init();

  audio::init();
  gui::init();
  MC::init();

  SDL_Event ev; //event handler
  SDL_SetEventFilter(isMouseEvent,NULL);

  done = false;
  drawFrame=0;
  while (!done)
  {
    start=SDL_GetPerformanceCounter();
    //HANDLE KEY/MOUSE EVENTS
    while (SDL_PollEvent(&ev))
    {
      if (ev.type == SDL_QUIT)
      {
        done = true;
      }
      // else if (ev.type == SDL_FINGERDOWN) {
      //     std::cout << "touchDown @ X: " << ev.tfinger.x << " Y: " << ev.tfinger.x << " PRESS: " << ev.tfinger.pressure << " touchID: " << ev.tfinger.fingerId << " deviceID: : " << ev.tfinger.touchId << std::endl;
      // }
      // else if (ev.type == SDL_FINGERUP) {
      //     std::cout << "touchUp @ X: " << ev.tfinger.x << " Y: " << ev.tfinger.x << " PRESS: " << ev.tfinger.pressure << " touchID: " << ev.tfinger.fingerId << " deviceID: : " << ev.tfinger.touchId << std::endl;
      // }
      // else if (ev.type == SDL_FINGERMOTION) {
      //     std::cout << "touchMove @ X: " << ev.tfinger.x << " Y: " << ev.tfinger.x << " PRESS: " << ev.tfinger.pressure << " touchID: " << ev.tfinger.fingerId << " deviceID: : " << ev.tfinger.touchId << std::endl;
      // }
      else if (ev.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX;
        int mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        std::cout << "click @ X: " << mouseX << " Y: " << mouseY << std::endl;
      }
      else if (ev.type == SDL_MOUSEBUTTONUP)
      {
      }
      else if (ev.type == SDL_USEREVENT)
      {
      }
      else if (ev.type == SDL_KEYDOWN)
      {
        switch (ev.key.keysym.sym)
        {
        case SDLK_ESCAPE:
          done = true;
          break;
        case SDLK_d:
          if (debug) {
            debug=false;
          } else {
            debug=true;
          }
          break;
        }
      } else if (ev.type == SDL_USEREVENT) {
      
      }
    }
  

    if (SDL_GetTicks() - lasttime > 10000) {
      controller::debugSerial();
      //std::cout << "temperature: " << getTemp() << std::endl;
      //debugArcade();
      lasttime = SDL_GetTicks();
    }
    else {
      if (SDL_GetTicks() < lasttime) {
        lasttime = SDL_GetTicks();
      }
    }
    drawFrame++;
    if (drawFrame == 8) { drawFrame = 0; }

    //get input
    
    lpd::receiveMessages();
    controller::getControls(); ///stored in controller::buffers
    midiLocal::receive();
    
    ////////////////////// let MasterControl do its thing
    MC::update(drawFrame,deltaTime);


    //////////////////////// mastercontrol has updated everything and communicated

    //set output leds + gui
    controller::update(drawFrame);
    gui::update(drawFrame);
    
    if (drawFrame==3) {
        if (debug) {
            for (int i=0;i<8;i++) {
              std::stringstream stream;
              stream << std::fixed << std::setprecision(1) << frametimes[i];
              gui::renderTextSmall(1,i+2, "\xFB" + stream.str());
            }
        }

    }    

    //frame timing

    end1=SDL_GetPerformanceCounter();
    double frametime = (end1 - start)*1000 / (double) SDL_GetPerformanceFrequency();
    if (frametime < 4.8) {
      usleep(4800-frametime*1000);
    }
    end2=SDL_GetPerformanceCounter();
    frametimes[drawFrame] = (frametimes[drawFrame]*0.8 + ((end1 - start)*1000 / (double) SDL_GetPerformanceFrequency() )*0.2);
    deltaTime = (end2 - start) * 1000 / (double) SDL_GetPerformanceFrequency();
  }

cleanup:
  MC::shutdown();
  gui::shutdown();
  audio::shutdown();
  controller::shutdown();
  midiLocal::shutdown();
  midiUart::shutdown();

  return 0;
}

