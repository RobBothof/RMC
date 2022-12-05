/*
Copyright (c) 2018 Rob Bothof. All rights reserved.
Created by: Rob Bothof in july 2018
This code may not be redistributed without written permission of the creator.
*/

#include "midiUart.hpp"
#include <iostream>

// stuff for serial
#include <unistd.h>     //serial read/close
#include <fcntl.h>
//uart settings
#include <sys/ioctl.h>
#include "/usr/include/asm-generic/termbits.h"
#include "/usr/include/asm-generic/ioctls.h"

char mididata[3] = {};
int uartPort;

int midiUart::init() {
  bool serialPortFound = false;
  if (!serialPortFound) {

    uartPort = open("/dev/ttyS1", O_WRONLY | O_NOCTTY | O_NONBLOCK);
    if (uartPort < 0) {
      std::cout << "Unable to open /dev/ttyS1" << std::endl;
    } else {
      serialPortFound = true;
      std::cout << "Opened port: /dev/ttyS1" << std::endl;
    }
  }
  if (!serialPortFound) {
    return 1;
  } else {
    struct termios2 tio;
    ioctl(uartPort, TCGETS2, &tio);
    tio.c_cflag &= ~CBAUD;
    tio.c_cflag |= BOTHER;
    tio.c_ispeed = 31250;
    tio.c_ospeed = 31250;
    int r = ioctl(uartPort, TCSETS2, &tio);

    if (r == 0) {
        std::cout << "Uart set to 31250 Baud successfully." << std::endl;
        return 0;
    } else {
        std::cout << "Unable to set Uart baud rate for /dev/ttyS1" << std::endl;
        return 1;
    }
    return 0;
  }  
}
void midiUart::sendClock() {
  mididata[0] = 0xF8;
  write(uartPort,mididata,1);
}
void midiUart::sendClockStart() {
  //rhythm wolf timecode 0
  mididata[0]= 0xF2;
  mididata[1]= 0x00;
  mididata[2]= 0x00;
  write(uartPort,mididata,3);
  mididata[0]= 0xFA;
  write(uartPort,mididata,1);
}
void midiUart::shutdown() {
    close(uartPort);
}