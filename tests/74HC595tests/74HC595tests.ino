#include <Arduino.h>

// Pin out data
int latchPin = 2;
int clockPin = 4;
int dataPin = 3;

int FWD_LED = 0;
int Node_LED = 1;
int TGT_pro_LED = 2;
int DOWN_LED = 3;
int TGT_retro_LED = 4;
int RIGHT_LED = 5;
int LEFT_LED = 6;
int BWD_LED = 7;
int RCS_LED = 8;
int Mode_RCS_LED = 9;
int Mode_SAS_LED = 10;
int SAS_LED = 11;
int UP_LED = 12;
int Ref_surface = 13;
int Ref_target = 14;
int Ref_orbit = 15;

int FWD_button = 7;
int RCS_button = 8;
int Mode_button = 9;
int UP_button = 10;
int Referential_button = 11;
int SAS_button = 12;
int RIGHT_button = 22;
int TGT_retro_button = 24;
int DOWN_button = 26;
int TGT_pro_button = 28;
int LEFT_button = 30;
int BWD_button = 32;
int Node_button = 34;
int MAN_SAS_button = 36;

// Global variables
int numOfRegisters = 2;
byte* registerState;

void setup() {

  // Serial initialization
  Serial.begin(9600);
  
  //Initialize LED registers array
  registerState = new byte[numOfRegisters];
  for (size_t i = 0; i < numOfRegisters; i++) {
    registerState[i] = 0;
  }

  // LEDs - shift registers pin configuration
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // Buttons pin configuration
  pinMode(FWD_button, INPUT);
  pinMode(RCS_button, INPUT);
  pinMode(Mode_button, INPUT);
  pinMode(UP_button, INPUT);
  pinMode(Referential_button, INPUT);
  pinMode(SAS_button, INPUT);
  pinMode(RIGHT_button, INPUT);
  pinMode(TGT_retro_button, INPUT);
  pinMode(DOWN_button, INPUT);
  pinMode(TGT_pro_button, INPUT);
  pinMode(LEFT_button, INPUT);
  pinMode(BWD_button, INPUT);
  pinMode(Node_button, INPUT);
  pinMode(MAN_SAS_button, INPUT);
}

void regWrite(int pin, bool state){
    //Determines register
    int reg = pin / 8;
    //Determines pin for actual register
    int actualPin = pin - (8 * reg);

    //Begin session
    digitalWrite(latchPin, LOW);

    for (int i = 0; i < numOfRegisters; i++){
        //Get actual states for register
        byte* states = &registerState[i];

        //Update state
        if (i == reg){
            bitWrite(*states, actualPin, state);
        }

        //Write
        shiftOut(dataPin, clockPin, MSBFIRST, *states);
    }

    //End session
    digitalWrite(latchPin, HIGH);
}

void loop() {
  bool SAS = digitalRead(SAS_button);
  bool RCS = digitalRead(RCS_button);
  bool Ref = digitalRead(Referential_button);
  bool Mode = digitalRead(Mode_button);
  bool FWD = digitalRead(FWD_button);
  bool BWD = digitalRead(BWD_button);
  bool UP = digitalRead(UP_button);
  bool DOWN = digitalRead(DOWN_button);
  bool LEFT = digitalRead(LEFT_button);
  bool RIGHT = digitalRead(RIGHT_button);
  bool MAN_SAS = digitalRead(MAN_SAS_button);
  bool Node = digitalRead(Node_button);
  bool TGT_pro = digitalRead(TGT_pro_button);
  bool TGT_retro = digitalRead(TGT_retro_button);
  
  if (SAS) {
    regWrite(SAS_LED, HIGH);
  } else {
    regWrite(SAS_LED, LOW);
  }

  if (RCS) {
    regWrite(RCS_LED, HIGH);
  } else {
    regWrite(RCS_LED, LOW);
  }

  if(Mode){
    regWrite(Mode_RCS_LED, HIGH);
    regWrite(Mode_SAS_LED, HIGH);
  } else {
    regWrite(Mode_RCS_LED, LOW);
    regWrite(Mode_SAS_LED, LOW);
  }
  
  if(Ref){
    regWrite(Ref_surface, HIGH);
    regWrite(Ref_target, HIGH);
    regWrite(Ref_orbit, HIGH);
  } else {
    regWrite(Ref_surface, LOW);
    regWrite(Ref_target, LOW);
    regWrite(Ref_orbit, LOW);
  }

    if (FWD) {
        regWrite(FWD_LED, HIGH);
    } else {
        regWrite(FWD_LED, LOW);
    }

    if (BWD) {
        regWrite(BWD_LED, HIGH);
    } else {
        regWrite(BWD_LED, LOW);
    }

    if (UP) {
        regWrite(UP_LED, HIGH);
    } else {
        regWrite(UP_LED, LOW);
    }

    if (DOWN) {
        regWrite(DOWN_LED, HIGH);
    } else {
        regWrite(DOWN_LED, LOW);
    }

    if (RIGHT) {
        regWrite(RIGHT_LED, HIGH);
    } else {
        regWrite(RIGHT_LED, LOW);
    }

    if (LEFT) {
        regWrite(LEFT_LED, HIGH);
    } else {
        regWrite(LEFT_LED, LOW);
    }

    if (MAN_SAS) {
        // todo
    }

    if (Node) {
        regWrite(Node_LED, HIGH);
    } else {
        regWrite(Node_LED, LOW);
    }
    if (TGT_pro) {
        regWrite(TGT_pro_LED, HIGH);
    } else {
        regWrite(TGT_pro_LED, LOW);
    }

    if (TGT_retro) {
        regWrite(TGT_retro_LED, HIGH);
    } else {
        regWrite(TGT_retro_LED, LOW);
    }
}

void testLEDs() {
  // Test LEDs one by one
  int testDelay = 500; // ms
  for (int i = 0; i < 8 * numOfRegisters; i++) { // for each LED one by one
    for (int j = 0; j < 8 * numOfRegisters; j++) {
      if (i == j) {
        regWrite(i, HIGH);
      } else {
        regWrite(j, LOW);
      }
    }
    delay(testDelay);
  }
}
