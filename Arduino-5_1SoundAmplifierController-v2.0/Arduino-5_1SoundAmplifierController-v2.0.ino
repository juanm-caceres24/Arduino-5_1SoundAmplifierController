#include <Wire.h>

byte buffer[3];
byte pos = 0;
const int pinMute = 5;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(pinMute, OUTPUT);
}

// address of sound amplifier
#define PRC_ADR 0x44 // 0100 0100 - 68
#define SEL_ADR 0x4A // 0100 1010 - 74

// audio processor codes
#define SYS_RESET  0xFF // 1111 1111 - 255
#define VOL_01STEP 0xD0 // 1101 0000 - 208
#define VOL_10STEP 0xE0 // 1110 0000 - 244
#define FUNCTIONS  0x70 // 0111 0000 - 112
#define ACTIVATE   0xC7 // 1100 0111 - 199

// audio selector codes
#define ENCH_SURR 0xD0 // 1101 0000 - 208
#define MIX_CHANL 0x90 // 1001 0000 - 144
#define SEL_IN    0xC0 // 1100 0000 - 192

void process() {
  switch (buffer[0]) {
    case 1: { // initialize
      digitalWrite(pinMute, HIGH);
      Wire.beginTransmission(PRC_ADR);
      byte res = Wire.write(byte(SYS_RESET));
      Wire.endTransmission();
      if (res > 0) {
        Wire.beginTransmission(PRC_ADR);
        Wire.write(byte(ACTIVATE));
        Wire.endTransmission();
      }
      Serial.write(res);
      break;
    }
    case 2: { // volume
      Wire.beginTransmission(PRC_ADR);
      Wire.write(VOL_10STEP | buffer[1]);
      Wire.write(VOL_01STEP | buffer[2]);
      Wire.endTransmission();
      break;
    }
    case 3: { } // trim
    case 4: { // tone
      Wire.beginTransmission(PRC_ADR);
      Wire.write(buffer[1] | buffer[2]);
      Wire.endTransmission();
      break;
    }
    case 5: { // functions
      byte res = FUNCTIONS;
      res |= (buffer[1] >> 1) & 0xE;
      res ^= 0x04;
      res ^= 0x02;
      Wire.beginTransmission(PRC_ADR);
      Wire.write(res);
      Wire.endTransmission();
      Wire.beginTransmission(SEL_ADR);
      Wire.write(ENCH_SURR | (buffer[1] & 0x01));
      Wire.endTransmission();
      Wire.beginTransmission(SEL_ADR);
      Wire.write(MIX_CHANL | ((buffer[1] >> 1) & 0x01));
      Wire.endTransmission();
      break;
    }
    case 6: { // input
      Wire.beginTransmission(SEL_ADR);
      Wire.write(SEL_IN | buffer[1]);
      Wire.endTransmission();
      break;
    }
    default: { }
  }
}

void loop() {
  if (Serial.available() > 0) {
    buffer[pos] = Serial.read();
    pos++;
    if (pos >= 3) {
      process();
      pos = 0;
    }
  }
}
