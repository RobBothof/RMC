#include "controller.hpp"
#include "masterControl.hpp"
#include "lpd.hpp"

// stuff for serial
#include <termios.h>    //termios serial settings
#include <unistd.h>     //serial read/close

#include <iostream>
#include <cstring> //memset

#define SPI_PORT 0
#define I2C_BUS 0
#define I2C_ADDR 0x70
const uint8_t arcadeColors3[8][3] = {{0x00,0x00,0x00},{0x00,0x77,0xEE},{0xFF,0x12,0x00},{0x00,0xAA,0x01},{0x01,0x00,0xCC},{0x55,0x00,0xBB},{0xA0,0xAA,0xA0},{0x55,0x99,0x00}};
const uint8_t arcadeColors2[8][3] = {{0x00,0x00,0x00},{2,0,255},{0x00,0x77,0xEE},{254,127,45},{187,254,67},{107,72,254},{24,74,254},{155,145,254}};
const uint8_t arcadeColors[10][3] = {{0x00,0x00,0x00},{2,0,255},{0x00,0x77,0xEE},{254,127,45},{0,160,50},{107,72,254},{24,74,254},{155,145,254},{187,254,67},{0x55,0x00,0xBB}};


uint8_t spiBuf[144];
uint8_t spiBufPattern[144];
uint8_t spiBufStartEnd[] = {0x00, 0x00, 0x00, 0x00};

uint8_t ledBuffer[8];
const uint8_t ledsOn[4] = {0b00100000,0b00010000,0b00000010,0b00000100};
const uint8_t ledsOff[4] = {0b11011111,0b11101111,0b11111101,0b11111011};



int serialPort;
struct termios serialSettings;
int serialBytesRead;
uint8_t serialBuffer[128];
int serialSuccesCount = 0;
int serialDropCount = 0;
int serialAvgSucces = 0;

int controlBuffercount = 0;

mraa::I2c i2c(I2C_BUS);
mraa::Spi spi(SPI_PORT);

int controller::init() {
    std::cout << "**** connecting hardware\n" << std::endl;

    //init teensy
    if (initSerial()==1) {
        return 1;
    }
    mraa_init();

    //init SPIBUF
    memset(spiBuf, 0x00, sizeof(spiBuf));
    memset(spiBufPattern, 0x00, sizeof(spiBufPattern));

    for (int m = 0; m < 36; m++)
    {
        spiBuf[m * 4] = 0xE2; //set brightness
        spiBufPattern[m * 4] = 0xE2; //set brightness
    }

    //init i2c
    i2c.frequency(mraa::I2C_HIGH);
    i2c.address(I2C_ADDR);
    i2c.writeByte(0x21); //system setup, osc on
    i2c.address(I2C_ADDR);
    i2c.writeByte(0x81); // display on, no blink
    i2c.address(I2C_ADDR);
    i2c.writeByte(0xEF); //max brightness
    memset(ledBuffer, 0x00, sizeof(ledBuffer));

    controller::clearLeds();
}

void controller::update(int drawFrame) {
    //controller::getControls();

    //controller::setArcadeLeds();
    


    // update Arcade Leds
    spi.write(spiBufStartEnd, sizeof(spiBufStartEnd));
    if (arcade::edit) {
    spi.write(spiBufPattern, sizeof(spiBufPattern));
    } else {
    spi.write(spiBuf, sizeof(spiBuf));
    }
    spi.write(spiBufStartEnd, sizeof(spiBufStartEnd));
    
    //update Mixer Leds
    uint8_t rx_tx_buf[2];
    //for (int i=0;i<8;i++) {
    int i = drawFrame % 4;
        i2c.address(I2C_ADDR);
        rx_tx_buf[0] = i; //row adress
        rx_tx_buf[1] = ledBuffer[i];
        i2c.write(rx_tx_buf, 2);
        i2c.address(I2C_ADDR);
        rx_tx_buf[0] = i+4; //row adress
        rx_tx_buf[1] = ledBuffer[i+4];
        i2c.write(rx_tx_buf, 2);
    //}
    
    
}

int controller::shutdown() {
  mraa_deinit();
  close(serialPort);
}


int controller::initSerial() {
  bool serialPortFound = false;
  if (!serialPortFound) {
    serialPort = open("/dev/ttyACM0", O_RDONLY | O_NOCTTY | O_NONBLOCK);
    if (serialPort < 0) {
      std::cout << "Unable to open /dev/ttyACM0" << std::endl;
    } else {
      serialPortFound = true;
       std::cout << "Opened port: /dev/ttyACM0" << std::endl;
    }
  }
  if (!serialPortFound) {
    serialPort = open("/dev/ttyACM1", O_RDONLY | O_NOCTTY | O_NONBLOCK);
    if (serialPort < 0) {
       std::cout <<  "Unable to open /dev/ttyACM1" << std::endl;
    } else {
      serialPortFound = true;
       std::cout << "Opened port: /dev/ttyACM1" << std::endl;
    }
  }
  if (!serialPortFound) {
    serialPort = open("/dev/ttyACM2", O_RDONLY | O_NOCTTY | O_NONBLOCK);
    if (serialPort < 0) {
       std::cout << "Unable to open /dev/ttyACM2" << std::endl;
    } else {
      serialPortFound = true;
       std::cout << "Opened port: /dev/ttyACM2" << std::endl;
    }
  }
  if (!serialPortFound) {
    return 1;
  } else {
    //config serialPort
    tcgetattr(serialPort, &serialSettings);
    cfmakeraw(&serialSettings);
    tcsetattr(serialPort, TCSANOW, &serialSettings);
  }
  return 0;
}

void controller::debugSerial() {
  if (MC::debug) std::cout << "== drops:" << serialDropCount << " Total:" << serialSuccesCount  << std::endl;
  serialSuccesCount = 0;
  serialDropCount = 0;
}

void controller::getControls() {
  //GET CONTROLS
  serialBytesRead = read(serialPort, serialBuffer, sizeof(serialBuffer));
  if (serialBytesRead > 0) {
    for (int i = 0; i < serialBytesRead; i++) {
      if (serialBuffer[i] == 255) {
        controlBuffercount = 0;
      }
      if (controlBuffercount < 63) {
        MC::controlBuffer[controlBuffercount] = serialBuffer[i];
      }
      controlBuffercount++;
    }
    if (serialBytesRead > 63) {
      serialDropCount++;
    }
    serialSuccesCount++;
  }
}

// void controller::setArcadeLeds() {
//     int arcadecol=3;
//     for (int m = 0; m < 3; m++) {
//       for (int n = 0; n < 6; n++) {
//         int mask = 1 << n;
//         int masked_n = MC::controlBuffer[m * 2 + 1] & mask;
//         int thebit = masked_n >> n;
//         spiBuf[m * 48 + n * 4 + 1] = arcadeColors[arcadecol][0] * thebit;
//         spiBuf[m * 48 + n * 4 + 2] = arcadeColors[arcadecol][1] * thebit;
//         spiBuf[m * 48 + n * 4 + 3] = arcadeColors[arcadecol][2] * thebit;
//       }
//       for (int n = 0; n < 6; n++) {
//         int mask = 1 << (5 - n);
//         int masked_n = MC::controlBuffer[m * 2 + 2] & mask;
//         int thebit = masked_n >> (5 - n);
//         spiBuf[m * 48 + 24 + n * 4 + 1] = arcadeColors[arcadecol][0] * thebit;
//         spiBuf[m * 48 + 24 + n * 4 + 2] = arcadeColors[arcadecol][1] * thebit;
//         spiBuf[m * 48 + 24 + n * 4 + 3] = arcadeColors[arcadecol][2] * thebit;
//       }
//     }
// }
void controller::setArcade(int led, int arcadecol) {
     if (led%12 < 6) {
      spiBuf[led * 4 + 1] = arcadeColors[arcadecol][0];
      spiBuf[led * 4 + 2] = arcadeColors[arcadecol][1];
      spiBuf[led * 4 + 3] = arcadeColors[arcadecol][2];
     } else {
      spiBuf[((led/12)*12+11-(led%12)+6) * 4 + 1] = arcadeColors[arcadecol][0];
      spiBuf[((led/12)*12+11-(led%12)+6) * 4 + 2] = arcadeColors[arcadecol][1];
      spiBuf[((led/12)*12+11-(led%12)+6) * 4 + 3] = arcadeColors[arcadecol][2];
     }
}
void controller::setArcadePattern(int led, int arcadecol) {
     if (led%12 < 6) {
      spiBufPattern[led * 4 + 1] = arcadeColors[arcadecol][0];
      spiBufPattern[led * 4 + 2] = arcadeColors[arcadecol][1];
      spiBufPattern[led * 4 + 3] = arcadeColors[arcadecol][2];
     } else {
      spiBufPattern[((led/12)*12+11-(led%12)+6) * 4 + 1] = arcadeColors[arcadecol][0];
      spiBufPattern[((led/12)*12+11-(led%12)+6) * 4 + 2] = arcadeColors[arcadecol][1];
      spiBufPattern[((led/12)*12+11-(led%12)+6) * 4 + 3] = arcadeColors[arcadecol][2];
     }
}

void controller::setArcadeRGB(int led, int value, int R, int G, int B) {
     if (led%12 < 6) {
      spiBuf[led * 4 + 1] = B * value;
      spiBuf[led * 4 + 2] = G * value;
      spiBuf[led * 4 + 3] = R * value;
     } else {
      spiBuf[((led/12)*12+11-(led%12)+6) * 4 + 1] = B * value;
      spiBuf[((led/12)*12+11-(led%12)+6) * 4 + 2] = G * value;
      spiBuf[((led/12)*12+11-(led%12)+6) * 4 + 3] = R * value;
     }
}

void controller::setArcadeBrightness(int led, int value) {
  if (value > 31) {
    value=31;
  }
    if (led%12 < 6) {
    spiBuf[led * 4] = value + 0xE0;
    spiBufPattern[led * 4] = value + 0xE0;
    } else {
    spiBuf[((led/12)*12+11-(led%12)+6) * 4] = value + 0xE0;
    spiBufPattern[((led/12)*12+11-(led%12)+6) * 4] = value + 0xE0;
    }
}

void controller::setLed(int led, int value) {
  if (value > 0) {
    if (led < 8) {
      ledBuffer[(3-(led%4))*2+1] |= ledsOn[led/4];
    } else {
      ledBuffer[((3-led%4))*2] |= ledsOn[led/4];
    }
  } else {
    if (led < 8) {
      ledBuffer[(3-(led%4))*2+1] &= ledsOff[led/4];
    } else {
      ledBuffer[(3-(led%4))*2] &= ledsOff[led/4];
    }
  }
}

void controller::setVu(int vu, int value) {
  //clear vu
  ledBuffer[vu*2] &= 0b00001111;
  ledBuffer[vu*2+1] &= 0b11110000;
  if (value > 8) {
    value=8;
  }
  //set vu
  uint8_t b1 = 0b00001111;
  if (value < 5) {
    b1 = b1 >> (4-value);
  }
  b1 = b1 << 4;
  ledBuffer[vu*2] |= b1;

  uint8_t b2 = 0b00001111;
  if (value > 4) {
    value -= 4;
    b2 = b2 >> (4-value);
    b2 &= 0b00001111;
    ledBuffer[vu*2+1] |= b2;
  }
}

void controller::clearLeds() {
  for (int i=0;i<8;i++) {
    ledBuffer[i] = 0x00;
  }
}

void controller::setAllLeds() {
  uint8_t rx_tx_buf[2];
  for (int i=0;i<8;i++) {
    ledBuffer[i] = 0xFF;
  }
}
