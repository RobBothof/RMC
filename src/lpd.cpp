/*
 * Copyright (c) 2012 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/libpd/libpd for documentation
 *
 * This file was adapted from the ofxPd openFrameworks addon example:
 * https://github.com/danomatika/ofxPd
 *
 */
#include "lpd.hpp"

#include <iostream>
#include "midiUart.hpp"

#include "masterControl.hpp"


//--------------------------------------------------------------
void PdObject::print(const std::string& message) {
	std::cout << message << std::endl;
}

//--------------------------------------------------------------		
void PdObject::receiveBang(const std::string& dest) {
	std::cout << "CPP: bang " << dest << std::endl;
}

void PdObject::receiveFloat(const std::string& dest, float num) {
	//std::cout << "CPP: float " << dest << ": " << num << std::endl;
  if (dest=="editLeds") {
    MC::editLeds=(int) num;
  }
  if (dest=="noteLeds") {
    MC::noteLeds=(int) num;
  }
  if (dest=="bpm") {
    MC::realBPM = num;
  }
  if (dest=="root") {
    MC::root = (int) num;
  }

}

void PdObject::receiveSymbol(const std::string& dest, const std::string& symbol) {
	std::cout << "CPP: symbol " << dest << ": " << symbol << std::endl;
}

void PdObject::receiveList(const std::string& dest, const pd::List& list) {
  if (dest=="cppLoopIndexer") {
    MC::loopIndex = (int) list.getFloat(0);
    MC::loop1buffer[MC::loopIndex]=(int) list.getFloat(1);
    MC::loop2buffer[MC::loopIndex]=(int) list.getFloat(2);
    MC::loop3buffer[MC::loopIndex]=(int) list.getFloat(3);
    MC::loop4buffer[MC::loopIndex]=(int) list.getFloat(4);

    MC::pitch1buffer[MC::loopIndex]=list.getFloat(5);
    MC::pitches[1]=list.getFloat(5);

    MC::pitch2buffer[MC::loopIndex]=list.getFloat(6);
    MC::pitches[2]=list.getFloat(6);

    MC::pitch3buffer[MC::loopIndex]=list.getFloat(7);
    MC::pitches[3]=list.getFloat(7);

    MC::pitch4buffer[MC::loopIndex]=list.getFloat(8);
    MC::pitches[4]=list.getFloat(8);

    //std::cout << "CPP: list " << dest << ": " << list << std::endl;
  } 

}

void PdObject::receiveMessage(const std::string& dest, const std::string& msg, const pd::List& list) {
	std::cout << "CPP: message " << dest << ": " << msg << " " << list.toString() << list.types() << std::endl;
}

//--------------------------------------------------------------

// reserved for receiving PD clock bytes
void PdObject::receiveMidiByte(const int port, const int byte) {
  if (byte == 248) { 
    midiUart::sendClock();
	  //if (MC::debug) std::cout << "CPP MIDI: midi clock" << std::endl;
  } else if (byte==250) {
    midiUart::sendClockStart();
	  //if (MC::debug) std::cout << "CPP MIDI: midi ----> start" << std::endl;
  } else {
	  std::cout << "CPP MIDI: midi byte: " << port << " " << byte << std::endl;
  }
}

//--------------------------------------------------------------
void PdObject::receiveNoteOn(const int channel, const int pitch, const int velocity) {
	std::cout << "CPP MIDI: note on: " << channel << " " << pitch << " " << velocity << std::endl;
}

void PdObject::receiveControlChange(const int channel, const int controller, const int value) {
	std::cout << "CPP MIDI: control change: " << channel << " " << controller << " " << value << std::endl;
}

void PdObject::receiveProgramChange(const int channel, const int value) {
	std::cout << "CPP MIDI: program change: " << channel << " " << value << std::endl;
}

void PdObject::receivePitchBend(const int channel, const int value) {
	std::cout << "CPP MIDI: pitch bend: " << channel << " " << value << std::endl;
}

void PdObject::receiveAftertouch(const int channel, const int value) {
	std::cout << "CPP MIDI: aftertouch: " << channel << " " << value << std::endl;
}

void PdObject::receivePolyAftertouch(const int channel, const int pitch, const int value) {
	std::cout << "CPP MIDI: poly aftertouch: " << channel << " " << pitch << " " << value << std::endl;
}

