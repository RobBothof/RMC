#ifndef AUDIO_H
#define AUDIO_H

#include <jack/jack.h>
#include <string>
#include <vector>
#include "masterControl.hpp"

int audioCallback(jack_nframes_t nframes, void *args);

namespace audio {
  int init();
  int shutdown();
  int loadPatch();
}

namespace lpd {
  void receiveMessages();
  void sendBang(const std::__cxx11::string &dest);
  void sendFloat(const std::__cxx11::string &dest, float value);
  void sendList(const std::__cxx11::string &dest,std::vector<int> v);
  void sendList(const std::__cxx11::string &dest,std::vector<MC::polynote> v);

}

#endif