#include "midiLocal.hpp"

RtMidiIn *midiin;
RtMidiOut *midiout;

std::vector<unsigned char> midiMsgIn;
std::vector<unsigned char> midiMsgOut1;
std::vector<unsigned char> midiMsgOut2;
std::vector<unsigned char> midiMsgOut3;

int midiLocal::init() {
    // RtMidiOut constructor
    try {
        midiout = new RtMidiOut();
    }
    catch ( RtMidiError &error ) {
        error.printMessage();
        exit( EXIT_FAILURE );
    }

    try {
        midiout->openVirtualPort("RMC Midi Out 1");
    }
    catch ( RtMidiError &error ) {
        error.printMessage();
        midiLocal::shutdown();
    }

    // RtMidiIn constructor
    try {
        midiin = new RtMidiIn();
    }
    catch ( RtMidiError &error ) {
        error.printMessage();
        exit( EXIT_FAILURE );
    }
    try {
        midiin->openVirtualPort("RMC Midi In 1");
    }
    catch ( RtMidiError &error ) {
        error.printMessage();
        midiLocal::shutdown();
    }

    // Don't ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes( false, false, false );

    midiMsgOut1.reserve(1);
    midiMsgOut2.reserve(2);
    midiMsgOut3.reserve(3);

    midiMsgOut1.push_back(0);

    midiMsgOut2.push_back(0);
    midiMsgOut2.push_back(0);
    
    midiMsgOut3.push_back(0);
    midiMsgOut3.push_back(0);
    midiMsgOut3.push_back(0);

    return 0;
}

int midiLocal::shutdown() {
  delete midiin;
  delete midiout;
  return 0;
}
int midiLocal::receive() {
  double stamp = midiin->getMessage(&midiMsgIn);
  int nBytes = midiMsgIn.size();
  if (nBytes > 2) {
    if ((int) midiMsgIn[0] == 176) {
      std::cout << "midi control message from rtMidi: Controlnr: " << (int)midiMsgIn[1] << " value: " << (int)midiMsgIn[2] << " timestamp: " << stamp << std::endl;
    }
  }
  return 0;
}
int midiLocal::send(uint8_t b1) {
    midiMsgOut1[0] = b1;
    midiout->sendMessage(&midiMsgOut1);
}
int midiLocal::send(uint8_t b1,uint8_t b2) {
    midiMsgOut2[0] = b1;
    midiMsgOut2[1] = b2;
    midiout->sendMessage(&midiMsgOut2);
}
int midiLocal::send(uint8_t b1,uint8_t b2, uint8_t b3) {
    midiMsgOut3[0] = b1;
    midiMsgOut3[1] = b2;
    midiMsgOut3[2] = b3;
    midiout->sendMessage(&midiMsgOut3);
}