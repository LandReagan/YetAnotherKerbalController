#include <Arduino.h>

// Pin out data________________________________________________________________
// 74HC595 register#1, register#2 is daisy-chained to #1
int latchPin = 2;
int clockPin = 4;
int dataPin = 3;

// Virtual pinout for 74HC595 registers
int FWD_LED = 0;
int node_LED = 1;
int TGT_pro_LED = 2;
int DOWN_LED = 3;
int TGT_retro_LED = 4;
int RIGHT_LED = 5;
int LEFT_LED = 6;
int BWD_LED = 7;
int RCS_LED = 8;
int mode_RCS_LED = 9;
int mode_SAS_LED = 10;
int SAS_LED = 11;
int UP_LED = 12;
int ref_surface_LED = 15;
int ref_target_LED = 14;
int ref_orbit_LED = 13;

// Buttons pin-out (direct)
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

// Global variables____________________________________________________________
// "Button was pressed" booleans
bool FWD_button_pressed = false;
bool RCS_button_pressed = false;
bool Mode_button_pressed = false;
bool UP_button_pressed = false;
bool Referential_button_pressed = false;
bool SAS_button_pressed = false;
bool RIGHT_button_pressed = false;
bool TGT_retro_button_pressed = false;
bool DOWN_button_pressed = false;
bool TGT_pro_button_pressed = false;
bool LEFT_button_pressed = false;
bool BWD_button_pressed = false;
bool Node_button_pressed = false;
bool MAN_SAS_button_pressed = false;

// System states
bool RCS = false;
bool SAS = false;
enum ControlMode {MODE_SAS, MODE_RCS};
ControlMode control_mode = MODE_SAS;
enum SpeedMode {ORBIT, TARGET, SURFACE};
SpeedMode speed_mode = SURFACE;
enum SASMode {STABILITY_ASSIST, PROGRADE, RETROGRADE, NORMAL, ANTI_NORMAL,
    RADIAL, ANTI_RADIAL, MANEUVER, TO_TARGET, ANTI_TARGET};
SASMode sas_mode = STABILITY_ASSIST;
bool FWD = false;
bool BWD = false;
bool UP = false;
bool LEFT = false;
bool DOWN = false;
bool RIGHT = false;

// System state has changed flag
bool state_changed = false;

void process_incoming_serial() {
  while (Serial.available() > 0) {
    char data = Serial.read();

    if (data == 'A') RCS = true;
    else if (data == 'B') RCS = false;
    else if (data == 'C') SAS = true;
    else if (data == 'D') SAS = false;

    else if (data == 'E') speed_mode = ORBIT;
    else if (data == 'F') speed_mode = TARGET;
    else if (data == 'G') speed_mode = SURFACE;

    else if (data == 'H') sas_mode = PROGRADE;
    else if (data == 'I') sas_mode = RETROGRADE;
    else if (data == 'J') sas_mode = NORMAL;
    else if (data == 'K') sas_mode = ANTI_NORMAL;
    else if (data == 'L') sas_mode = RADIAL;
    else if (data == 'M') sas_mode = ANTI_RADIAL;
    else if (data == 'N') sas_mode = STABILITY_ASSIST;
    else if (data == 'O') sas_mode = MANEUVER;
    else if (data == 'P') sas_mode = TO_TARGET;
    else if (data == 'Q') sas_mode = ANTI_TARGET;    
  }
}

void update_system_state() {
  int value = 0;

  if (RCS) {
    value += pow(2, RCS_LED);
  }
  if (SAS) {
    value += pow(2, SAS_LED);
  }
  switch (control_mode) {
    case MODE_SAS:
      value += pow(2, mode_SAS_LED);
      break;
    case MODE_RCS:
      value += pow(2, mode_RCS_LED);
      break;
  }
  switch (speed_mode) {
    case ORBIT:
    value += pow(2, ref_orbit_LED);
    break;
    case TARGET:
    value += pow(2, ref_target_LED);
    break;
    case SURFACE:
    value += pow(2, ref_surface_LED);
    break;
  }

  if (control_mode == MODE_SAS) {
    switch (sas_mode) {
      case PROGRADE:
        value += pow(2, FWD_LED);
        break;
      case RETROGRADE:
        value += pow(2, BWD_LED);
        break;
      case NORMAL:
        value += pow(2, UP_LED);
        break;
      case ANTI_NORMAL:
        value += pow(2, DOWN_LED);
        break;
      case RADIAL:
        value += pow(2, LEFT_LED);
        break;
      case ANTI_RADIAL:
        value += pow(2, RIGHT_LED);
        break;
      case STABILITY_ASSIST:
        //value += pow(2, DOWN_LED);
        break;
      case MANEUVER:
        value += pow(2, node_LED);
        break;
      case TO_TARGET:
        value += pow(2, TGT_pro_LED);
        break;
      case ANTI_TARGET:
        value += pow(2, TGT_retro_LED);
        break;
    } 
  } else { // light buttons if pressed in RCS mode
    if (UP_button_pressed) {
      value += pow(2, UP_LED);
    } else if (DOWN_button_pressed) {
      value += pow(2, DOWN_LED);
    } else if (LEFT_button_pressed) {
      value += pow(2, LEFT_LED);
    } else if (RIGHT_button_pressed) {
      value += pow(2, RIGHT_LED);
    } else if (FWD_button_pressed) {
      value += pow(2, FWD_LED);
    } else if (BWD_button_pressed) {
      value += pow(2, BWD_LED);
    }
  }

  // Write the state to registers
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, value % (int)pow(2, 8));
  shiftOut(dataPin, clockPin, MSBFIRST, value / (int)pow(2, 8));
  digitalWrite(latchPin, HIGH);
}

void check_buttons_state() {
  
  bool FWD_press = digitalRead(FWD_button);
  bool RCS_press = digitalRead(RCS_button);
  bool Mode_press = digitalRead(Mode_button);
  bool UP_press = digitalRead(UP_button);
  bool Referential_press = digitalRead(Referential_button);
  bool SAS_press = digitalRead(SAS_button);
  bool RIGHT_press = digitalRead(RIGHT_button);
  bool TGT_retro_press = digitalRead(TGT_retro_button);
  bool DOWN_press = digitalRead(DOWN_button);
  bool TGT_pro_press = digitalRead(TGT_pro_button);
  bool LEFT_press = digitalRead(LEFT_button);
  bool BWD_press = digitalRead(BWD_button);
  bool Node_press = digitalRead(Node_button);
  bool MAN_SAS_press = digitalRead(MAN_SAS_button);

  if (SAS_press) {
    if (!SAS_button_pressed) {
      state_changed = true;
      SAS = !SAS;
      SAS_button_pressed = true;
    }
  } else {
    SAS_button_pressed = false;
  }
}

void send_state_to_serial() {
  
}

void setup() {
  // Serial connection setup
  Serial.begin(9600);
  Serial.setTimeout(100);

  // Pin mode setup
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

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

  update_system_state();  // Initializing LEDs
}

void loop() {
  
  process_incoming_serial();
  
  update_system_state();

  check_buttons_state();

  if (state_changed) send_state_to_serial();
  state_changed = false;

  /*
  bool SAS_press = digitalRead(SAS_button) == HIGH;
  if(SAS_press && !SAS_button_pressed) {
    SAS_button_pressed = true;
    SAS_on = !SAS_on;
    sendData();
  } else if(!SAS_press) {
    SAS_button_pressed = false;
  }
  if(Serial.available() > 0){
    processSerial(Serial.readString());
  }
  */
}