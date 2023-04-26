#include <Wire.h>
#include <Keypad.h>

// I2C address of sound amplifier chips
#define ADR_PT2322 0b01000100 // 68
#define ADR_PT2323 0b01001010 // 74

/* --FUTURE IMPLEMENTATIONS--
// arduino digital INPUTS pins
#define BTN_INIT        22
#define BTN_MENU_SEL    23
#define BTN_UP          24
#define BTN_DOWN        25
#define BTN_MUTE_BOARD  26
#define BTN_MUTE        27
#define BTN_INPUT_SEL   28
#define BTN_SPEAKER_SEL 29
#define BTN_MIX_SEL     30
*/

// arduino digital OUTPUTS pins
#define PIN_MUTE_BOARD 31

// keypad constants & pins
// #define QUANTITY_OF_ROWS    4
// #define QUANTITY_OF_COLUMNS 4
// const char KEYMAP[QUANTITY_OF_ROWS][QUANTITY_OF_COLUMNS] = {
//   {'1', '2', '3', 'A'},
//   {'4', '5', '6', 'B'},
//   {'7', '8', '9', 'C'},
//   {'*', '0', '#', 'D'}
// };
// const byte ROWS_PINS[QUANTITY_OF_ROWS] = {46, 47, 48, 49};
// const byte COLUMNS_PINS[QUANTITY_OF_COLUMNS] = {50, 51, 52, 53};

// global variables
byte dataAux;
char key;
int menu;
int function;
int input;
int speakerMode;
int channelMute;
int surround;
int mix;
int muteBoard;
int mute;
int volume;
int volume10;
int volume01;
int treble;
int mid;
int bass;
int sub;
int FL;
int FR;
int CN;
int SL;
int SR;

// keypad initialization
// Keypad keypad = Keypad(makeKeymap(KEYMAP), ROWS_PINS, COLUMNS_PINS, QUANTITY_OF_ROWS, QUANTITY_OF_COLUMNS);

void setup() {
  Wire.begin(9600);
  Serial.begin(9600);

  // built-in LED
  pinMode(LED_BUILTIN, OUTPUT);

  /* --FUTURE IMPLEMENTATIONS--
  // digital inputs
  pinMode(BTN_INIT, INPUT);
  pinMode(BTN_MENU_SEL, INPUT);
  pinMode(BTN_UP, INPUT);
  pinMode(BTN_DOWN, INPUT);
  pinMode(BTN_MUTE, INPUT);
  pinMode(BTN_INPUT_SEL, INPUT);
  pinMode(BTN_SPEAKER_SEL, INPUT);
  pinMode(BTN_MIX_SEL, INPUT);
  */

  // digital outputs
  pinMode(PIN_MUTE_BOARD, OUTPUT);
}

void loop() {
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);

  /*
  // KEYPAD input control
  key = keypad.getKey();
  if (key) {
    digitalWrite(LED_BUILTIN, HIGH);
    switch (key) {
      case '*':
        manualInitialization();
        showValues();
      case '1':
        inputSelection();
      break;
      case '2':
        speakerModeSelection();
      break;
      case '3':
        mixSelection();
      break;
      case '5':
        muteBoardSelection();
      break;
      case '4':
        volMute();
      break;
      case '0':
        showValues();
      break;
      case 'D':
        menuSelection();
      break;
      case 'A':
        menuUp();
      break;
      case 'B':
        menuDown();
      break;
    }
  }
  */

  // SERIAL input master control
  if (Serial.available()) {
    digitalWrite(LED_BUILTIN, HIGH);
    switch (Serial.read()) {
      case 48:               // '0' ASCII | initialize()
        manualInitialization();
        showValues();
      break;
      case 99:               // 'c' ASCII | inputSelection()
        inputSelection();
      break;
      case 110:              // 'n' ASCII | speakerModeSelection()
        speakerModeSelection();
      break;
      case 98:               // 'b' ASCII | mixSelection()
        mixSelection();
      break;
      case 118:              // 'v' ASCII | muteBoard()
        muteBoardSelection();
      break;
      case 109:              // 'm' ASCII | volMute()
        volMute();
      break;
      case 43:               // '+' ASCII | volUp()
        volUp();
      break;
      case 45:               // '-' ASCII | volDown()
        volDown();
      break;
      case 117:              // 'u' ASCII | subUp()
        bassUp();
      break;
      case 106:              // 'j' ASCII | subDown()
        bassDown();
      break;
      case 105:              // 'i' ASCII | midUp()
        midUp();
      break;
      case 107:              // 'k' ASCII | midDown()
        midDown();
      break;
      case 111:              // 'o' ASCII | trebleUp()
        trebleUp();
      break;
      case 108:              // 'l' ASCII | trebleDown()
        trebleDown();
      break;
      case 113:              // 'q' ASCII | subUp()
        subUp();
      break;
      case 97:               // 'a' ASCII | subDown()
        subDown();
      break;
      case 119:              // 'w' ASCII | subUp()
        FLUp();
      break;
      case 115:              // 's' ASCII | subDown()
        FLDown();
      break;
      case 101:              // 'e' ASCII | subUp()
        FRUp();
      break;
      case 100:              // 'd' ASCII | subDown()
        FRDown();
      break;
      case 114:              // 'r' ASCII | subUp()
        CNUp();
      break;
      case 102:              // 'f' ASCII | subDown()
        CNDown();
      break;
      case 116:              // 't' ASCII | subUp()
        SLUp();
      break;
      case 103:              // 'g' ASCII | subDown()
        SLDown();
      break;
      case 121:              // 'y' ASCII | subUp()
        SRUp();
      break;
      case 104:              // 'h' ASCII | subDown()
        SRDown();
      break;
      case 122:              // 'z' ASCII | showValues()
        showValues();
      break;
    }
  }
}

// ================ LOGIC METHODS ================ //

void manualInitialization() {
  
  // desactivate PT2322
  Wire.beginTransmission(ADR_PT2322);
  Wire.write(0b11111111);
  Wire.endTransmission();
  // activate PT2322
  Wire.beginTransmission(ADR_PT2322);
  Wire.write(0b11000111);
  Wire.endTransmission();

  menu = 0;        // 0->volume | 1->treble | 2->mid | 3->bass | 4->sub | 5->FL | 6->FR | 7->CN | 8->SL | 9->SR
  input = 4;       // 0->aux_1 | 1->aux_2 | 2->XX | 3->XX | 4->5.1
  volume = 50;     // 29 - 79
  muteBoard = 0;   // 0->unmuted | 1->muted
  mute = 0;        // 0->unmuted | 1->muted
  mix = 1;         // 0->0dB | 1->+6dB
  treble = 7;      // 0 - 15
  mid = 7;         // 0 - 15
  bass = 7;        // 0 - 15
  sub = 7;         // 0 - 15
  FL = 7;          // 0 - 15
  FR = 7;          // 0 - 15
  CN = 7;          // 0 - 15
  SL = 7;          // 0 - 15
  SR = 7;          // 0 - 15
  speakerMode = 0; // 0->5.1 | 1->2.1

  setInput();
  setSpeakerMode();
  setMix();
  setMuteBoard();
  setMute();
  setVolume();
  setBass();
  setMid();
  setTreble();
  setFL();
  setFR();
  setCN();
  setSL();
  setSR();
  functionPT2322(0, 1, 0);
    
  // DEBUG
  Serial.println("<< manual initialization >>");
}

void inputSelection() {
  input++;
  if (input > 4)
    input = 0;
  setInput();
  
  // DEBUG
  Serial.print("<< input selection: ");
  Serial.print(input);
  Serial.println(" >>");
}

void speakerModeSelection() {
  speakerMode++;
  setSpeakerMode();

  // DEBUG
  Serial.print("<< speaker mode: ");
  Serial.print(speakerMode);
  Serial.println(" >>");
}

void mixSelection() {
  mix = !mix;
  setMix();

  // DEBUG
  Serial.print("<< mix: ");
  Serial.print(mix);
  Serial.println(" >>");
}

void muteBoardSelection() {
  muteBoard = !muteBoard;
  setMuteBoard();

  Serial.print("<< mute board: ");
  Serial.print(muteBoard);
  Serial.println(" >>");
}

void volMute() {
  mute = !mute;
  setMute();
  
  // DEBUG
  Serial.print("<< mute: ");
  Serial.print(mute);
  Serial.println(" >>");
}

void volUp() {
  volume++;
  setVolume();
  
  // DEBUG
  Serial.print("<< volume up | volume: ");
  Serial.print(volume);
  Serial.println(" >>");
}

void volDown() {
  volume--;
  setVolume();
  
  // DEBUG
  Serial.print("<< volume down | volume: ");
  Serial.print(volume);
  Serial.println(" >>");
}

void bassUp() {
  bass++;
  setBass();

  // DEBUG
  Serial.print("<< bass up | volume: ");
  Serial.print(bass);
  Serial.println(" >>");
}

void bassDown() {
  bass--;
  setBass();

  // DEBUG
  Serial.print("<< bass down | volume: ");
  Serial.print(bass);
  Serial.println(" >>");
}

void midUp() {
  mid++;
  setMid();

  // DEBUG
  Serial.print("<< mid up | volume: ");
  Serial.print(mid);
  Serial.println(" >>");
}

void midDown() {
  mid--;
  setMid();

  // DEBUG
  Serial.print("<< mid down | volume: ");
  Serial.print(mid);
  Serial.println(" >>");
}

void trebleUp() {
  treble++;
  setTreble();

  // DEBUG
  Serial.print("<< treble up | volume: ");
  Serial.print(treble);
  Serial.println(" >>");
}

void trebleDown() {
  treble--;
  setTreble();

  // DEBUG
  Serial.print("<< treble down | volume: ");
  Serial.print(treble);
  Serial.println(" >>");
}

void subUp() {
  sub++;
  setSub();

  // DEBUG
  Serial.print("<< sub up | volume: ");
  Serial.print(sub);
  Serial.println(" >>");
}

void subDown() {
  sub--;
  setSub();

  // DEBUG
  Serial.print("<< sub down | volume: ");
  Serial.print(sub);
  Serial.println(" >>");
}

void FLUp() {
  FL++;
  setFL();

  // DEBUG
  Serial.print("<< FL up | volume: ");
  Serial.print(FL);
  Serial.println(" >>");
}

void FLDown() {
  FL--;
  setFL();

  // DEBUG
  Serial.print("<< FL down | volume: ");
  Serial.print(FL);
  Serial.println(" >>");
}

void FRUp() {
  FR++;
  setFR();

  // DEBUG
  Serial.print("<< FR up | volume: ");
  Serial.print(FR);
  Serial.println(" >>");
}

void FRDown() {
  FR--;
  setFR();

  // DEBUG
  Serial.print("<< FR down | volume: ");
  Serial.print(FR);
  Serial.println(" >>");
}

void CNUp() {
  CN++;
  setCN();

  // DEBUG
  Serial.print("<< CN up | volume: ");
  Serial.print(CN);
  Serial.println(" >>");
}

void CNDown() {
  CN--;
  setCN();

  // DEBUG
  Serial.print("<< CN down | volume: ");
  Serial.print(CN);
  Serial.println(" >>");
}

void SLUp() {
  SL++;
  setSL();

  // DEBUG
  Serial.print("<< SL up | volume: ");
  Serial.print(SL);
  Serial.println(" >>");
}

void SLDown() {
  SL--;
  setSL();

  // DEBUG
  Serial.print("<< SL down | volume: ");
  Serial.print(SL);
  Serial.println(" >>");
}

void SRUp() {
  SR++;
  setSR();

  // DEBUG
  Serial.print("<< SR up | volume: ");
  Serial.print(SR);
  Serial.println(" >>");
}

void SRDown() {
  SR--;
  setSR();

  // DEBUG
  Serial.print("<< SR down | volume: ");
  Serial.print(SR);
  Serial.println(" >>");
}

void showValues() {
  Serial.print("menu: ");
  Serial.println(menu);
  Serial.print("input: ");
  Serial.println(input);
  Serial.print("volume: ");
  Serial.println(volume);
  Serial.print("mute: ");
  Serial.println(mute);
  Serial.print("mix: ");
  Serial.println(mix);
  Serial.print("bass: ");
  Serial.println(bass);
  Serial.print("mid: ");
  Serial.println(mid);
  Serial.print("treble: ");
  Serial.println(treble);
  Serial.print("sub: ");
  Serial.println(sub);
  Serial.print("FL: ");
  Serial.println(FL);
  Serial.print("FR: ");
  Serial.println(FR);
  Serial.print("CN: ");
  Serial.println(CN);
  Serial.print("SL: ");
  Serial.println(SL);
  Serial.print("SR: ");
  Serial.println(SR);
  Serial.print("speakerMode: ");
  Serial.println(speakerMode);
  Serial.print("channelMute: ");
  Serial.println(channelMute);
  Serial.print("surround: ");
  Serial.println(surround);
}

void menuSelection() {
  menu++;
  if (menu > 9)
    menu = 0;

  // DEBUG
  Serial.print("<< menu: ");
  Serial.print(menu);
  Serial.println(" >>");
}

void menuUp() {
  switch (menu) {
    case 0:
      volUp();
    break;
    case 1:
      trebleUp();
    break;
    case 2:
      midUp();
    break;
    case 3:
      bassUp();
    break;
    case 4:
      subUp();
    break;
    case 5:
      FLUp();
    break;
    case 6:
      FRUp();
    break;
    case 7:
      CNUp();
    break;
    case 8:
      SLUp();
    break;
    case 9:
      SRUp();
    break;
  }
}

void menuDown() {
  switch (menu) {
    case 0:
      volDown();
    break;
    case 1:
      trebleDown();
    break;
    case 2:
      midDown();
    break;
    case 3:
      bassDown();
    break;
    case 4:
      subDown();
    break;
    case 5:
      FLDown();
    break;
    case 6:
      FRDown();
    break;
    case 7:
      CNDown();
    break;
    case 8:
      SLDown();
    break;
    case 9:
      SRDown();
    break;
  }
}

// ================ COMMUNICATIONS ================ //

void setInput() {
  if (input > 4)
    input = 0;
  switch (input) {
    case 0:        // 1 input
      sendToPT2323(0b11001011);
      break; 
    case 1:        // 2 input
      sendToPT2323(0b11001010);
      break;
    case 2:        // 3 input
      sendToPT2323(0b11001001);
      break;
    case 3:        // 4 input
      sendToPT2323(0b11001000);
      break;
    case 4:        // 6 channels input
      sendToPT2323(0b11000111);
      break;
  }
}

void setSurround() {
  if (surround > 1)
    surround = 0;
  switch (surround) {
    case 0:           // surround ON
      sendToPT2323(0b11010000);
      break;
    case 1:           // surround OFF
      sendToPT2323(0b11010001);
      break;
  }
}

void setSpeakerMode() {
  if (speakerMode > 1)
    speakerMode = 0;
  switch (speakerMode) {
    case 0:              // 5.1 mode
      channelMute = 0;
      surround = 0;
      break;
    case 1:              // 2.1 mode
      channelMute = 1;
      surround = 1;
      break;
  }
  setCN();
  setSL();
  setSR();
  setSurround();
}

void setMix() {
  if (mix > 1)
    mix = 0;
  switch (mix) {
    case 0:      // 0dB setup
      sendToPT2323(0b10010000);
      break;
    case 1:      // +6dB setup
      sendToPT2323(0b10010001);
      break;
  }
}

void setMuteBoard() {
  if (mute > 1)
    muteBoard = 0;
  switch (muteBoard) {
    case 0:            // board unmuted
      digitalWrite(PIN_MUTE_BOARD, LOW);
      break;
    case 1:            // board muted
      digitalWrite(PIN_MUTE_BOARD, HIGH);
      break;
  }
}

void setMute() {
  if (mute > 1)
    mute = 0;
  switch (mute) {
    case 0:       // all channels unmuted
      sendToPT2323(0b11111110);
      break;
    case 1:       // all channels muted
      sendToPT2323(0b11111111);
      break;
  }
}

void setVolume() {
  if (volume > 79)
    volume = 79;
  if (volume < 29)
    volume = 29;
  int aux = 79 - volume;
  volume10 = aux/10;
  volume01 = aux - volume10*10;
  Wire.beginTransmission(ADR_PT2322);
  Wire.write(volume10 + 0b11100000);
  Wire.write(volume01 + 0b11010000);
  Wire.endTransmission();
}

void setBass() {
  if (bass > 15)
    bass = 15;
  if (bass < 0)
    bass = 0;
  dataAux = bass; 
  if(bass > 7)
    dataAux = 23 - bass;
  sendToPT2322(0b10010000 + dataAux);
}

void setMid() {
  if (mid > 15)
    mid = 15;
  if (mid < 0)
    mid = 0;
  dataAux = mid; 
  if(mid > 7)
    dataAux = 23 - mid;
  sendToPT2322(0b10100000 + dataAux);
}

void setTreble() {
  if (treble > 15)
    treble = 15;
  if (treble < 0)
    treble = 0;
  dataAux = treble; 
  if(treble > 7)
    dataAux = 23 - treble;
  sendToPT2322(0b10110000 + dataAux);
}

void setSub() {
  if (sub > 15)
    sub = 15;
  if (sub < 0)
    sub = 0;
  sendToPT2322(0b01100000 + 15 - sub);
}

void setFL() {
  if (FL > 15)
    FL = 15;
  if (FL < 0)
    FL = 0;
  sendToPT2322(0b00010000 + 15 - FL);
}

void setFR() {
  if (FR > 15)
    FR = 15;
  if (FR < 0)
    FR = 0;
  sendToPT2322(0b00100000 + 15 - FR);
}

void setCN() {
  if (CN > 15)
    CN = 15;
  if (CN < 0)
    CN = 0;
  switch (channelMute) {
    case 0:              // CN mute disabled
      sendToPT2323(0b11110100);
      break;
    case 1:              // CN mute
      sendToPT2323(0b11110101);
      break;
  }
  sendToPT2322(0b00110000 + 15 - CN);
}

void setSL() {
  if (SL > 15)
    SL = 15;
  if (SL < 0)
    SL = 0;
  switch(channelMute) {
    case 0:             // SL mute disabled
      sendToPT2323(0b11111000);
      break;
    case 1:             // SL mute
      sendToPT2323(0b11111001);
      break;
  }
  sendToPT2322(0b01000000 + 15 - SL);
}

void setSR() {
  if (SR > 15)
    SR = 15;
  if (SR < 0)
    SR = 0;
  switch(channelMute) {
    case 0:             // SR mute disabled
      sendToPT2323(0b11111010);
      break;
    case 1:             // SR mute
      sendToPT2323(0b11111011);
      break;
  }
  sendToPT2322(0b01010000 + 15 - SR);
}

void functionPT2322(int mutePT2322, int effect, int toneControl) {
  byte muteSelection;
  byte effectSelection;
  byte toneSelection;
  switch (mutePT2322) {
    case 0:
      muteSelection = 0b00000000;
      break;
    case 1:
      muteSelection = 0b00001000;
      break;
  }
  switch (effect) {
    case 0:
      effectSelection = 0b00000000;
      break;
    case 1:
      effectSelection = 0b00000100;
      break;
  }
  switch (toneControl) {
    case 0:
      toneSelection = 0b00000000;
      break;
    case 1:
      toneSelection = 0b00000010;
      break;
  }
  sendToPT2322(0b01110000 + muteSelection + effectSelection + toneSelection);
}

// ================ PT2322 & PT2323 SEND ================ //

void inputSwitchPT2322() {
  Wire.beginTransmission(ADR_PT2322);
  Wire.write(0b11000111);
  Wire.endTransmission();
}

void sendToPT2322(char c) {
  Wire.beginTransmission(ADR_PT2322);
  Wire.write(c);
  Wire.endTransmission();
}

void sendToPT2323(char c) {
  Wire.beginTransmission(ADR_PT2323);
  Wire.write(c);
  Wire.endTransmission();
}
