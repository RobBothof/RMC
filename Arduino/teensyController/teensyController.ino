// The code reads all buttons, faders and encoders and packs it into a compact 64 byte packet
// this way we make efficient use of usb and can update the the entire controller state at once and with
// very low latency


#define THRESHOLD 2

const int ADDR_A = 5;
const int ADDR_B = 6;
const int ADDR_C = 7;

const int MPX_1 = 8; //8
const int MPX_2 = 9; //9
const int MPX_3 = 10; //10
const int MPX_4 = 11; //11
const int MPX_5 = 12; //12  

const int MIXA_1 = 15;
const int MIXA_2 = 16;
const int MIXA_3 = 17;

const int MIXD_1 = 18;
const int MIXD_2 = 19;
const int MIXD_3 = 20;
const int MIXD_4 = 21;
const int MIXD_5 = 22;

const int CTRLD_1 = 33;
const int CTRLD_2 = 34;
const int CTRLD_3 = 35;
const int CTRLD_4 = 36;
const int CTRLD_5 = 37;
const int CTRLD_6 = 30;

const int CTRLA_1 = 38;
const int CTRLA_2 = 39;
const int CTRLA_3 = 40;

const int ledPin = 13;
byte serialBuf[63];

byte bounceArcadeButtons[36];
byte arcadeButtons[36];

byte bounceMixButtons[16];
byte mixButtons[16];

byte mixRawEncoders[24];
byte bounceMixRawEncoders[24];
byte mixOldEncoders[24];
byte mixEncoders[12];
byte mixSmoothEncoders[12];

byte mixEncodersSum[12];
byte mixEncodersSumOld[12];

int mixFaders[24];
int filteredMixFaders[24];

byte bounceCTRLButtons[42];
byte CTRLButtons[42];

byte CTRLRawEncoders[8];
byte bounceCTRLRawEncoders[8];
byte CTRLOldEncoders[8];
byte CTRLEncoders[4];
byte CTRLSmoothEncoders[4];
byte CTRLEncodersSum[4];
byte CTRLEncodersSumOld[4];

int CTRLFaders[4];
int filteredCTRLFaders[4];

int count = 0;
int muxdelay=35;
int delta;
  
elapsedMicros timecheck;

void setup() {
  analogReadAveraging(32);

  pinMode(ledPin, OUTPUT);

  pinMode(ADDR_A, OUTPUT);
  pinMode(ADDR_B, OUTPUT);
  pinMode(ADDR_C, OUTPUT);

  digitalWrite(ADDR_A, LOW);    // set the ADDR_A off
  digitalWrite(ADDR_B, LOW);    // set the ADDR_A off
  digitalWrite(ADDR_C, LOW);    // set the ADDR_A off

  pinMode(MPX_1, INPUT_PULLUP);
  pinMode(MPX_2, INPUT_PULLUP);
  pinMode(MPX_3, INPUT_PULLUP);
  pinMode(MPX_4, INPUT_PULLUP);
  pinMode(MPX_5, INPUT_PULLUP);

  pinMode(MIXD_1, INPUT_PULLUP);
  pinMode(MIXD_2, INPUT_PULLUP);
  pinMode(MIXD_3, INPUT_PULLUP);
  pinMode(MIXD_4, INPUT_PULLUP);
  pinMode(MIXD_5, INPUT_PULLUP);

  pinMode(CTRLD_1, INPUT_PULLUP);
  pinMode(CTRLD_2, INPUT_PULLUP);
  pinMode(CTRLD_3, INPUT_PULLUP);
  pinMode(CTRLD_4, INPUT_PULLUP);
  pinMode(CTRLD_5, INPUT_PULLUP);
  pinMode(CTRLD_6, INPUT_PULLUP);
  
  Serial.begin(2000000);
  delay(10);

  for (int i = 0; i < 36; i++) {
    bounceArcadeButtons[i]=0;
    arcadeButtons[i]=0;
  }
  for (int i = 0; i < 24; i++) {
    mixFaders[i]=0;
    filteredMixFaders[i]=127;

    mixRawEncoders[i]=0;
    bounceMixRawEncoders[i]=0;
    mixOldEncoders[i]=0;
  }
  for (int i = 0; i < 16; i++) {
    mixButtons[i]=0;
    bounceMixButtons[i]=0;
  }
  for (int i = 0; i < 12; i++) {
    mixEncoders[i]=128;
    mixEncodersSum[i]=0;
    mixEncodersSumOld[i]=0;
  }
  for (int i = 0; i < 8; i++) {
    CTRLRawEncoders[i]=0;
    bounceCTRLRawEncoders[i]=0;
    CTRLOldEncoders[i]=0;
  }
  for (int i = 0; i < 4; i++) {
    CTRLEncoders[i]=128;
    CTRLEncodersSum[i]=0;
    CTRLEncodersSumOld[i]=0;
  }
  for (int i = 0; i < 42; i++) {
    bounceCTRLButtons[i]=0;
    CTRLButtons[i]=0;
  }
  for (int i = 0; i < 3; i++) {
    CTRLFaders[i]=0;
    filteredCTRLFaders[i]=127;   
  }

  for (int i = 0; i < 63; i++) {
    serialBuf[i]=0;
  }
//  analogWrite(A21,1);

}
  
void getControls() {
  int mixButtonID=0;
  int mixEncoderID=0;
  int CTRLButtonID=0;
  int CTRLEncoderID=0;

  int A1=0;
  int A2=0;
  int A3=0;


  for (int i = 0; i < 36; i++) {
    bounceArcadeButtons[i] <<=1;
  }
  for (int i = 0; i < 16; i++) {
    bounceMixButtons[i] <<=1;
  }
  for (int i = 0; i < 24; i++) {
    bounceMixRawEncoders[i] <<=4;
  }
  for (int i = 0; i < 42; i++) {
    bounceCTRLButtons[i] <<=1;
  }
  for (int i = 0; i < 8; i++) {
   bounceCTRLRawEncoders[i] <<=4;
  }

  delayMicroseconds(5);
  int AR19 = analogRead(A19);
  CTRLFaders[0] += AR19;

  delayMicroseconds(5);
  int AR20 = analogRead(A20);
  CTRLFaders[1] += AR20;

  delayMicroseconds(5);
  int AR21 = analogRead(A21);
  CTRLFaders[2] += AR21;
  
  delayMicroseconds(5);

  digitalWriteFast(ADDR_A, 0);    // set the ADDR_A off
  digitalWriteFast(ADDR_B, 0);    // set the ADDR_A off
  digitalWriteFast(ADDR_C, 0);    // set the ADDR_A off
  delayMicroseconds(muxdelay);

  if (digitalReadFast(MPX_1) == 0) bounceArcadeButtons[24] |= 1;
  if (bounceArcadeButtons[24] == 0) arcadeButtons[24] = 0;
  if (bounceArcadeButtons[24] > 0)  arcadeButtons[24] = 1;

  if (digitalReadFast(MPX_2) == 0) bounceArcadeButtons[14] |= 1;
  if (bounceArcadeButtons[14] == 0) arcadeButtons[14] = 0;
  if (bounceArcadeButtons[14] > 0)  arcadeButtons[14] = 1;

  if (digitalReadFast(MPX_3) == 0) bounceArcadeButtons[4] |= 1;
  if (bounceArcadeButtons[4] == 0) arcadeButtons[4] = 0;
  if (bounceArcadeButtons[4] > 0)  arcadeButtons[4] = 1;

  if (digitalReadFast(MPX_4) == 0) bounceArcadeButtons[21] |= 1;
  if (bounceArcadeButtons[21] == 0) arcadeButtons[21] = 0;
  if (bounceArcadeButtons[21] > 0)  arcadeButtons[21] = 1;

  if (digitalReadFast(MPX_5) == 0) bounceArcadeButtons[35] |= 1;
  if (bounceArcadeButtons[35] == 0) arcadeButtons[35] = 0;
  if (bounceArcadeButtons[35] > 0)  arcadeButtons[35] = 1;

  mixButtonID=14;
  if (digitalReadFast(MIXD_5  ) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

  mixEncoderID=4;
  if (digitalReadFast(MIXD_1) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=6;
  if (digitalReadFast(MIXD_4) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=10;
  if (digitalReadFast(MIXD_3) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=21;
  if (digitalReadFast(MIXD_2) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  CTRLButtonID=16;
  if (digitalReadFast(CTRLD_1) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=10;
  if (digitalReadFast(CTRLD_2) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;
  
  CTRLButtonID=15;
  if (digitalReadFast(CTRLD_3) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=37;
  if (digitalReadFast(CTRLD_4) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=32;
  // if (digitalReadFast(CTRLD_5) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  // if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  // if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  //
  CTRLEncoderID=4;
  if (digitalReadFast(CTRLD_6) == 0) bounceCTRLRawEncoders[CTRLEncoderID] |= 1;
  if (bounceCTRLRawEncoders[CTRLEncoderID] == 0) CTRLRawEncoders[CTRLEncoderID] = 0;
  if (bounceCTRLRawEncoders[CTRLEncoderID] > 0)  CTRLRawEncoders[CTRLEncoderID] = 1;

  A1 = analogRead(1);
  mixFaders[14] += A1;

  A2 = analogRead(2);
  mixFaders[0] += A2;;

  A3 = analogRead(3);
  mixFaders[3] += A3;
      
  digitalWriteFast(ADDR_A, 1);    // set the ADDR_A off
  digitalWriteFast(ADDR_B, 0);    // set the ADDR_A off
  digitalWriteFast(ADDR_C, 0);    // set the ADDR_A off
  delayMicroseconds(muxdelay);

  if (digitalReadFast(MPX_2) == 0) bounceArcadeButtons[26] |= 1;
  if (bounceArcadeButtons[26] == 0) arcadeButtons[26] = 0;
  if (bounceArcadeButtons[26] > 0)  arcadeButtons[26] = 1;

  if (digitalReadFast(MPX_3) == 0) bounceArcadeButtons[7] |= 1;
  if (bounceArcadeButtons[7] == 0) arcadeButtons[7] = 0;
  if (bounceArcadeButtons[7] > 0)  arcadeButtons[7] = 1;

  if (digitalReadFast(MPX_4) == 0) bounceArcadeButtons[10] |= 1;
  if (bounceArcadeButtons[10] == 0) arcadeButtons[10] = 0;
  if (bounceArcadeButtons[10] > 0)  arcadeButtons[10] = 1;

  if (digitalReadFast(MPX_5) == 0) bounceArcadeButtons[23] |= 1;
  if (bounceArcadeButtons[23] == 0) arcadeButtons[23] = 0;
  if (bounceArcadeButtons[23] > 0)  arcadeButtons[23] = 1;

  mixButtonID=8;
  if (digitalReadFast(MIXD_3) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

  mixEncoderID=1;
  if (digitalReadFast(MIXD_2) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=3;
  if (digitalReadFast(MIXD_5) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=17;
  if (digitalReadFast(MIXD_1) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=19;
  if (digitalReadFast(MIXD_4) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  CTRLButtonID=12;
  if (digitalReadFast(CTRLD_1) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=17;
  if (digitalReadFast(CTRLD_2) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;
  
  CTRLButtonID=11;
  if (digitalReadFast(CTRLD_3) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=36;
  if (digitalReadFast(CTRLD_4) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=35;
  if (digitalReadFast(CTRLD_5) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLEncoderID=3;
  if (digitalReadFast(CTRLD_6) == 0) bounceCTRLRawEncoders[CTRLEncoderID] |= 1;
  if (bounceCTRLRawEncoders[CTRLEncoderID] == 0) CTRLRawEncoders[CTRLEncoderID] = 0;
  if (bounceCTRLRawEncoders[CTRLEncoderID] > 0)  CTRLRawEncoders[CTRLEncoderID] = 1;

  A1 = analogRead(1);
  mixFaders[16] += A1;

  A2 = analogRead(2);
  mixFaders[8] += A2;

  A3 = analogRead(3);
  mixFaders[10] += A3;
      
  digitalWriteFast(ADDR_A, 0);    // set the ADDR_A off
  digitalWriteFast(ADDR_B, 1);    // set the ADDR_A off
  digitalWriteFast(ADDR_C, 0);    // set the ADDR_A off
  delayMicroseconds(muxdelay);

  if (digitalReadFast(MPX_1) == 0) bounceArcadeButtons[31] |= 1;
  if (bounceArcadeButtons[31] == 0) arcadeButtons[31] = 0;
  if (bounceArcadeButtons[31] > 0)  arcadeButtons[31] = 1;

  if (digitalReadFast(MPX_2) == 0) bounceArcadeButtons[12] |= 1;
  if (bounceArcadeButtons[12] == 0) arcadeButtons[12] = 0;
  if (bounceArcadeButtons[12] > 0)  arcadeButtons[12] = 1;

  if (digitalReadFast(MPX_3) == 0) bounceArcadeButtons[2] |= 1;
  if (bounceArcadeButtons[2] == 0) arcadeButtons[2] = 0;
  if (bounceArcadeButtons[2] > 0)  arcadeButtons[2] = 1;

  if (digitalReadFast(MPX_4) == 0) bounceArcadeButtons[16] |= 1;
  if (bounceArcadeButtons[16] == 0) arcadeButtons[16] = 0;
  if (bounceArcadeButtons[16] > 0)  arcadeButtons[16] = 1;

  if (digitalReadFast(MPX_5) == 0) bounceArcadeButtons[33] |= 1;
  if (bounceArcadeButtons[33] == 0) arcadeButtons[33] = 0;
  if (bounceArcadeButtons[33] > 0)  arcadeButtons[33] = 1;

  /////
  mixButtonID=5;
  if (digitalReadFast(MIXD_1) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

  mixButtonID=1;
  if (digitalReadFast(MIXD_2) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

  mixButtonID=3;
  if (digitalReadFast(MIXD_5) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

  mixButtonID=7;
  if (digitalReadFast(MIXD_4) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

  mixButtonID=10;
  if (digitalReadFast(MIXD_3) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

 CTRLButtonID=8;
  if (digitalReadFast(CTRLD_1) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=13;
  if (digitalReadFast(CTRLD_2) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;
  
  CTRLButtonID=18;
  if (digitalReadFast(CTRLD_3) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=34;
  if (digitalReadFast(CTRLD_4) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=33;
  if (digitalReadFast(CTRLD_5) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  //
  CTRLEncoderID=5;
  if (digitalReadFast(CTRLD_6) == 0) bounceCTRLRawEncoders[CTRLEncoderID] |= 1;
  if (bounceCTRLRawEncoders[CTRLEncoderID] == 0) CTRLRawEncoders[CTRLEncoderID] = 0;
  if (bounceCTRLRawEncoders[CTRLEncoderID] > 0)  CTRLRawEncoders[CTRLEncoderID] = 1;

  A1 = analogRead(1);
  mixFaders[12] += A1;

  A2 = analogRead(2);
  mixFaders[5] += A2;

  A3 = analogRead(3);
  mixFaders[6] += A3;

  digitalWriteFast(ADDR_A, 0);    // set the ADDR_A off
  digitalWriteFast(ADDR_B, 0);    // set the ADDR_A off
  digitalWriteFast(ADDR_C, 1);    // set the ADDR_A off
  delayMicroseconds(muxdelay);

  if (digitalReadFast(MPX_1) == 0) bounceArcadeButtons[32] |= 1;
  if (bounceArcadeButtons[32] == 0) arcadeButtons[32] = 0;
  if (bounceArcadeButtons[32] > 0)  arcadeButtons[32] = 1;

  if (digitalReadFast(MPX_2) == 0) bounceArcadeButtons[13] |= 1;
  if (bounceArcadeButtons[13] == 0) arcadeButtons[13] = 0;
  if (bounceArcadeButtons[13] > 0)  arcadeButtons[13] = 1;

  if (digitalReadFast(MPX_3) == 0) bounceArcadeButtons[3] |= 1;
  if (bounceArcadeButtons[3] == 0) arcadeButtons[3] = 0;
  if (bounceArcadeButtons[3] > 0)  arcadeButtons[3] = 1;

  if (digitalReadFast(MPX_4) == 0) bounceArcadeButtons[17] |= 1;
  if (bounceArcadeButtons[17] == 0) arcadeButtons[17] = 0;
  if (bounceArcadeButtons[17] > 0)  arcadeButtons[17] = 1;

  if (digitalReadFast(MPX_5) == 0) bounceArcadeButtons[34] |= 1;
  if (bounceArcadeButtons[34] == 0) arcadeButtons[34] = 0;
  if (bounceArcadeButtons[34] > 0)  arcadeButtons[34] = 1;

  mixButtonID=4;
  if (digitalReadFast(MIXD_1) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

  mixButtonID=6;
  if (digitalReadFast(MIXD_4) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

  mixEncoderID=13;
  if (digitalReadFast(MIXD_2) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=15;
  if (digitalReadFast(MIXD_5) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=22;
  if (digitalReadFast(MIXD_3) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

 CTRLButtonID=20;
  if (digitalReadFast(CTRLD_1) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=25;
  if (digitalReadFast(CTRLD_2) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;
  
  CTRLButtonID=30;
  if (digitalReadFast(CTRLD_3) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=2;
  if (digitalReadFast(CTRLD_4) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=0;
  if (digitalReadFast(CTRLD_5) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  //
  CTRLEncoderID=1;
  if (digitalReadFast(CTRLD_6) == 0) bounceCTRLRawEncoders[CTRLEncoderID] |= 1;
  if (bounceCTRLRawEncoders[CTRLEncoderID] == 0) CTRLRawEncoders[CTRLEncoderID] = 0;
  if (bounceCTRLRawEncoders[CTRLEncoderID] > 0)  CTRLRawEncoders[CTRLEncoderID] = 1;

  A1 = analogRead(1);
  mixFaders[13] += A1;;

  A2 = analogRead(2);
  mixFaders[1] += A2;

  A3 = analogRead(3);
  mixFaders[2] += A3;

  digitalWriteFast(ADDR_A, 1);    // set the ADDR_A off
  digitalWriteFast(ADDR_B, 1);    // set the ADDR_A off
  digitalWriteFast(ADDR_C, 0);    // set the ADDR_A off
  delayMicroseconds(muxdelay);

  if (digitalReadFast(MPX_2) == 0) bounceArcadeButtons[25] |= 1;
  if (bounceArcadeButtons[25] == 0) arcadeButtons[25] = 0;
  if (bounceArcadeButtons[25] > 0)  arcadeButtons[25] = 1;

  if (digitalReadFast(MPX_3) == 0) bounceArcadeButtons[6] |= 1;
  if (bounceArcadeButtons[6] == 0) arcadeButtons[6] = 0;
  if (bounceArcadeButtons[6] > 0)  arcadeButtons[6] = 1;

  if (digitalReadFast(MPX_4) == 0) bounceArcadeButtons[9] |= 1;
  if (bounceArcadeButtons[9] == 0) arcadeButtons[9] = 0;
  if (bounceArcadeButtons[9] > 0)  arcadeButtons[9] = 1;

  if (digitalReadFast(MPX_5) == 0) bounceArcadeButtons[22] |= 1;
  if (bounceArcadeButtons[22] == 0) arcadeButtons[22] = 0;
  if (bounceArcadeButtons[22] > 0)  arcadeButtons[22] = 1;

  mixEncoderID=5;
  if (digitalReadFast(MIXD_1) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=7;
  if (digitalReadFast(MIXD_4) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=12;
  if (digitalReadFast(MIXD_2) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=14;
  if (digitalReadFast(MIXD_5) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=20;
  if (digitalReadFast(MIXD_3) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

 CTRLButtonID=9;
  if (digitalReadFast(CTRLD_1) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=14;
  if (digitalReadFast(CTRLD_2) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;
  
  CTRLButtonID=19;
  if (digitalReadFast(CTRLD_3) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=38;
  if (digitalReadFast(CTRLD_4) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=32;
  if (digitalReadFast(CTRLD_5) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLEncoderID=7;
  if (digitalReadFast(CTRLD_6) == 0) bounceCTRLRawEncoders[CTRLEncoderID] |= 1;
  if (bounceCTRLRawEncoders[CTRLEncoderID] == 0) CTRLRawEncoders[CTRLEncoderID] = 0;
  if (bounceCTRLRawEncoders[CTRLEncoderID] > 0)  CTRLRawEncoders[CTRLEncoderID] = 1;

  A1 = analogRead(1);
  mixFaders[19] += A1;

  A2 = analogRead(2);
  mixFaders[9] += A2;

  A3 = analogRead(3);
  mixFaders[11] += A3;

  digitalWriteFast(ADDR_A, 1);    // set the ADDR_A off
  digitalWriteFast(ADDR_B, 0);    // set the ADDR_A off
  digitalWriteFast(ADDR_C, 1);    // set the ADDR_A off
  delayMicroseconds(muxdelay);

  if (digitalReadFast(MPX_2) == 0) bounceArcadeButtons[19] |= 1;
  if (bounceArcadeButtons[19] == 0) arcadeButtons[19] = 0;
  if (bounceArcadeButtons[19] > 0)  arcadeButtons[19] = 1;

  if (digitalReadFast(MPX_3) == 0) bounceArcadeButtons[0] |= 1;
  if (bounceArcadeButtons[0] == 0) arcadeButtons[0] = 0;
  if (bounceArcadeButtons[0] > 0) arcadeButtons[0] = 1;

  if (digitalReadFast(MPX_4) == 0) bounceArcadeButtons[5] |= 1;
  if (bounceArcadeButtons[5] == 0) arcadeButtons[5] = 0;
  if (bounceArcadeButtons[5] > 0)  arcadeButtons[5] = 1;

  if (digitalReadFast(MPX_5) == 0) bounceArcadeButtons[28] |= 1;
  if (bounceArcadeButtons[28] == 0) arcadeButtons[28] = 0;
  if (bounceArcadeButtons[28] > 0)  arcadeButtons[28] = 1;

  /////
  mixButtonID=0;
  if (digitalReadFast(MIXD_2) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

  mixButtonID=2;
  if (digitalReadFast(MIXD_5) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

  mixButtonID=9;
  if (digitalReadFast(MIXD_3) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

  mixButtonID=13;
  if (digitalReadFast(MIXD_1) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

  mixEncoderID=11;
  if (digitalReadFast(MIXD_4) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

 CTRLButtonID=21;
  if (digitalReadFast(CTRLD_1) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=26;
  if (digitalReadFast(CTRLD_2) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;
  
  CTRLButtonID=23;
  if (digitalReadFast(CTRLD_3) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

   CTRLButtonID=3;
   if (digitalReadFast(CTRLD_4) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
   if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
   if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=5;
  if (digitalReadFast(CTRLD_5) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  //
  CTRLEncoderID=6;
  if (digitalReadFast(CTRLD_6) == 0) bounceCTRLRawEncoders[CTRLEncoderID] |= 1;
  if (bounceCTRLRawEncoders[CTRLEncoderID] == 0) CTRLRawEncoders[CTRLEncoderID] = 0;
  if (bounceCTRLRawEncoders[CTRLEncoderID] > 0)  CTRLRawEncoders[CTRLEncoderID] = 1;

  A1 = analogRead(1);
  mixFaders[17] += A1;

  A2 = analogRead(2);
  mixFaders[20] += A2;;

  A3 = analogRead(3);
  mixFaders[21] += A3;
      
  digitalWriteFast(ADDR_A, 0);    // set the ADDR_A off
  digitalWriteFast(ADDR_B, 1);    // set the ADDR_A off
  digitalWriteFast(ADDR_C, 1);    // set the ADDR_A off
  delayMicroseconds(muxdelay);

  if (digitalReadFast(MPX_1) == 0) bounceArcadeButtons[30] |= 1;
  if (bounceArcadeButtons[30] == 0) arcadeButtons[30] = 0;
  if (bounceArcadeButtons[30] > 0)  arcadeButtons[30] = 1;

  if (digitalReadFast(MPX_2) == 0) bounceArcadeButtons[20] |= 1;
  if (bounceArcadeButtons[20] == 0) arcadeButtons[20] = 0;
  if (bounceArcadeButtons[20] > 0)  arcadeButtons[20] = 1;

  if (digitalReadFast(MPX_3) == 0) bounceArcadeButtons[1] |= 1;
  if (bounceArcadeButtons[1] == 0) arcadeButtons[1] = 0;
  if (bounceArcadeButtons[1] > 0)  arcadeButtons[1] = 1;

  if (digitalReadFast(MPX_4) == 0) bounceArcadeButtons[15] |= 1;
  if (bounceArcadeButtons[15] == 0) arcadeButtons[15] = 0;
  if (bounceArcadeButtons[15] > 0)  arcadeButtons[15] = 1;

  if (digitalReadFast(MPX_5) == 0) bounceArcadeButtons[29] |= 1;
  if (bounceArcadeButtons[29] == 0) arcadeButtons[29] = 0;
  if (bounceArcadeButtons[29] > 0)  arcadeButtons[29] = 1;

  mixButtonID=12;
  if (digitalReadFast(MIXD_5) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

  mixEncoderID=9;
  if (digitalReadFast(MIXD_2) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=16;
  if (digitalReadFast(MIXD_1) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=18;
  if (digitalReadFast(MIXD_4) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=23;
  if (digitalReadFast(MIXD_3) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

 CTRLButtonID=24;
  if (digitalReadFast(CTRLD_1) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=29;
  if (digitalReadFast(CTRLD_2) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;
  
  CTRLButtonID=27;
  if (digitalReadFast(CTRLD_3) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=6;
  if (digitalReadFast(CTRLD_4) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=4;
  if (digitalReadFast(CTRLD_5) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  //
  CTRLEncoderID=0;
  if (digitalReadFast(CTRLD_6) == 0) bounceCTRLRawEncoders[CTRLEncoderID] |= 1;
  if (bounceCTRLRawEncoders[CTRLEncoderID] == 0) CTRLRawEncoders[CTRLEncoderID] = 0;
  if (bounceCTRLRawEncoders[CTRLEncoderID] > 0)  CTRLRawEncoders[CTRLEncoderID] = 1;

  A1 = analogRead(1);
  mixFaders[15] += A1;

  A2 = analogRead(2);
  mixFaders[4] += A2;

  A3 = analogRead(3);
  mixFaders[7] += A3;

  digitalWriteFast(ADDR_A, 1);    // set the ADDR_A off
  digitalWriteFast(ADDR_B, 1);    // set the ADDR_A off
  digitalWriteFast(ADDR_C, 1);    // set the ADDR_A off
  delayMicroseconds(muxdelay);

  if (digitalReadFast(MPX_2) == 0) bounceArcadeButtons[18] |= 1;
  if (bounceArcadeButtons[18] == 0) arcadeButtons[18] = 0;
  if (bounceArcadeButtons[18] > 0)  arcadeButtons[18] = 1;

  if (digitalReadFast(MPX_3) == 0) bounceArcadeButtons[8] |= 1;
  if (bounceArcadeButtons[8] == 0) arcadeButtons[8] = 0;
  if (bounceArcadeButtons[8] > 0)  arcadeButtons[8] = 1;

  if (digitalReadFast(MPX_4) == 0) bounceArcadeButtons[11] |= 1;
  if (bounceArcadeButtons[11] == 0) arcadeButtons[11] = 0;
  if (bounceArcadeButtons[11] > 0)  arcadeButtons[11] = 1;

  if (digitalReadFast(MPX_5) == 0) bounceArcadeButtons[27] |= 1;
  if (bounceArcadeButtons[27] == 0) arcadeButtons[27] = 0;
  if (bounceArcadeButtons[27] > 0)  arcadeButtons[27] = 1;

  
  mixButtonID=11;
  if (digitalReadFast(MIXD_4) == 0) bounceMixButtons[mixButtonID] |= 1;
  if (bounceMixButtons[mixButtonID] == 0) mixButtons[mixButtonID] = 0;
  if (bounceMixButtons[mixButtonID] > 0)  mixButtons[mixButtonID] = 1;

  /////
  
  mixEncoderID=0;
  if (digitalReadFast(MIXD_2) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=2;
  if (digitalReadFast(MIXD_5) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  mixEncoderID=8;
  if (digitalReadFast(MIXD_3) == 0) bounceMixRawEncoders[mixEncoderID] |= 1;
  if (bounceMixRawEncoders[mixEncoderID] == 0) mixRawEncoders[mixEncoderID] = 0;
  if (bounceMixRawEncoders[mixEncoderID] > 0)  mixRawEncoders[mixEncoderID] = 1;

  CTRLButtonID=28;
  if (digitalReadFast(CTRLD_1) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=22;
  if (digitalReadFast(CTRLD_2) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;
  
  CTRLButtonID=31;
  if (digitalReadFast(CTRLD_3) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=7;
  if (digitalReadFast(CTRLD_4) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1;

  CTRLButtonID=1;
  if (digitalReadFast(CTRLD_5) == 0) bounceCTRLButtons[CTRLButtonID] |= 1;
  if (bounceCTRLButtons[CTRLButtonID] == 0) CTRLButtons[CTRLButtonID] = 0;
  if (bounceCTRLButtons[CTRLButtonID] > 0)  CTRLButtons[CTRLButtonID] = 1; 

  //
  CTRLEncoderID=2;
  if (digitalReadFast(CTRLD_6) == 0) bounceCTRLRawEncoders[CTRLEncoderID] |= 1;
  if (bounceCTRLRawEncoders[CTRLEncoderID] == 0) CTRLRawEncoders[CTRLEncoderID] = 0;
  if (bounceCTRLRawEncoders[CTRLEncoderID] > 0)  CTRLRawEncoders[CTRLEncoderID] = 1;

  A1 = analogRead(1);
  mixFaders[18] += A1;

  A2 = analogRead(2);
  mixFaders[22] += A2;

  A3 = analogRead(3);
  mixFaders[23] += A3;
 
}

void updateEncoders() {
  for (int i = 0; i < 12; i++) {
      mixEncodersSum[i] = mixRawEncoders[i] + mixRawEncoders[i+12]*2;
      if (mixEncodersSumOld[i]==1) {
        if (mixEncodersSum[i]==3) {
          mixEncoders[i]++;
        }
      }
      if (mixEncodersSumOld[i]==2) {
        if (mixEncodersSum[i]==0) {
          mixEncoders[i]++;
        }
      }
      if (mixEncodersSumOld[i]==3) {
        if (mixEncodersSum[i]==2) {
          mixEncoders[i]++;
        }
      }
      if (mixEncodersSumOld[i]==0) {
        if (mixEncodersSum[i]==1) {
          mixEncoders[i]++;
        }
      }

      if (mixEncodersSumOld[i]==3) {
        if (mixEncodersSum[i]==1) {
          mixEncoders[i]--;
        }
      }
      if (mixEncodersSumOld[i]==0) {
        if (mixEncodersSum[i]==2) {
          mixEncoders[i]--;
        }
      }
      if (mixEncodersSumOld[i]==2) {
        if (mixEncodersSum[i]==3) {
          mixEncoders[i]--;
        }
      }
      if (mixEncodersSumOld[i]==1) {
        if (mixEncodersSum[i]==0) {
          mixEncoders[i]--;
        }
      }
      mixEncodersSumOld[i]=mixEncodersSum[i];
  }
  for (int i = 0; i < 4; i++) {
      CTRLEncodersSum[i] = CTRLRawEncoders[i] + CTRLRawEncoders[i+4]*2;
      if (CTRLEncodersSumOld[i]==1) {
        if (CTRLEncodersSum[i]==3) {
          CTRLEncoders[i]++;
        }
      }
      if (CTRLEncodersSumOld[i]==2) {
        if (CTRLEncodersSum[i]==0) {
          CTRLEncoders[i]++;
        }
      }
      if (CTRLEncodersSumOld[i]==3) {
        if (CTRLEncodersSum[i]==2) {
          CTRLEncoders[i]++;
        }
      }
      if (CTRLEncodersSumOld[i]==0) {
        if (CTRLEncodersSum[i]==1) {
          CTRLEncoders[i]++;
        }
      }

      if (CTRLEncodersSumOld[i]==3) {
        if (CTRLEncodersSum[i]==1) {
          CTRLEncoders[i]--;
        }
      }
      if (CTRLEncodersSumOld[i]==0) {
        if (CTRLEncodersSum[i]==2) {
          CTRLEncoders[i]--;
        }
      }
      if (CTRLEncodersSumOld[i]==2) {
        if (CTRLEncodersSum[i]==3) {
          CTRLEncoders[i]--;
        }
      }
      if (CTRLEncodersSumOld[i]==1) {
        if (CTRLEncodersSum[i]==0) {
          CTRLEncoders[i]--;
        }
      }
      CTRLEncodersSumOld[i]=CTRLEncodersSum[i];
  }
}

void resetEncoders() {
  for (int i = 0; i < 12; i++) {
    mixEncoders[i]=128;
    //mixSmoothEncoders[i]=128;
  }
  for (int i = 0; i < 4; i++) {
    CTRLEncoders[i]=128;
  }
  
}

void loop() {
  if (Serial.dtr()) { // if serial port open
    count++;
    getControls();
    
    //encoder processing
    updateEncoders();
    
    if (timecheck > 5000) {
      timecheck=0;

      //populate buffer 6 buttons per byte
      serialBuf[0]=255; // send startByte
      for (int j=0; j<6; j++) {
        serialBuf[j+1]=0;
        for (int i=5; i>=0; i--) {      
          serialBuf[j+1] = serialBuf[j+1] << 1;
          serialBuf[j+1] += (arcadeButtons[i + 6*j]);
        }
      }
      for (int j=0; j<4; j++) {
        serialBuf[j+7]=0;
        for (int i=3; i>=0; i--) {      
          serialBuf[j+7] = serialBuf[j+7] << 1;
          serialBuf[j+7] += (mixButtons[i + 4*j]);
        }
      }
      for (int j=0; j<12; j++) {
        serialBuf[j+11]=mixEncoders[j];
      }

      for (int j=0; j<24; j++) {
        int newFader = mixFaders[j]/count;
        delta = newFader - filteredMixFaders[j];
        if ( delta <= -THRESHOLD )
            filteredMixFaders[j] = newFader + THRESHOLD;
        if ( delta >= THRESHOLD )
            filteredMixFaders[j] = newFader - THRESHOLD;
        if (filteredMixFaders[j] > 512) {
          serialBuf[j+23]=(filteredMixFaders[j] >> 2)-1;
        } else {
          serialBuf[j+23]=(filteredMixFaders[j] >> 2);
        }
        mixFaders[j]=0;
      }

      for (int j=0; j<6; j++) {
        serialBuf[j+48]=0;
        for (int i=6; i>=0; i--) {      
          serialBuf[j+48] = serialBuf[j+48] << 1;
          serialBuf[j+48] += (CTRLButtons[i + 7*j]);
        }
      }
      for (int j=0; j<4; j++) {
        serialBuf[j+54]=CTRLEncoders[j];
      }

      int pbFader = CTRLFaders[2] / count;
      int ddelta = pbFader - filteredCTRLFaders[2];
      if ( ddelta <= -THRESHOLD )
          filteredCTRLFaders[2] = pbFader + THRESHOLD;
      if ( ddelta >= THRESHOLD )
          filteredCTRLFaders[2] = pbFader - THRESHOLD;
          
      int pbend = (filteredCTRLFaders[2]);
      
      if (pbend < 477) {
        pbend = 128 + ((pbend-477) / 2.3);
        if (pbend < 0) { pbend=0;}
      } else if ( pbend > 496) {
        pbend = 128 + ((pbend-496) / 2.35);
        if (pbend > 254) { pbend=254;}
      } else {
        pbend = 128;
      }
 
      
      serialBuf[60]=pbend;
      pbend=128;
      CTRLFaders[2]=0;

      for (int j=0; j<2; j++) {
        int newFader = CTRLFaders[j]/count;
        delta = newFader - filteredCTRLFaders[j];
        if ( delta <= -THRESHOLD )
            filteredCTRLFaders[j] = newFader + THRESHOLD;
        if ( delta >= THRESHOLD )
            filteredCTRLFaders[j] = newFader - THRESHOLD;
        if (filteredCTRLFaders[j] > 512) {
          serialBuf[j+58]=(filteredCTRLFaders[j] >> 2)-1;
        } else {
          serialBuf[j+58]=(filteredCTRLFaders[j] >> 2);
        }
        CTRLFaders[j]=0;
      }


      //SEND Serial usb packet

      Serial.write(serialBuf,63);
      Serial.send_now();

      count=0;
    
      //we have send data, reset encoders
      resetEncoders();
    }
  }
}

