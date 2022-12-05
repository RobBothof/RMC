/*
Copyright (c) 2018 Rob Bothof. All rights reserved.
Created by: Rob Bothof in july 2018
This code may not be redistributed without written permission of the creator.
*/

#ifndef MIDIUART_H
#define MIDIUART_H

namespace midiUart {
  int init();
  void sendClock();
  void sendClockStart();
  void shutdown();
}
#endif