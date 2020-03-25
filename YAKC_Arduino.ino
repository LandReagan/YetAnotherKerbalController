#include <Arduino.h>

int SAS_led = 7;
int SAS_button = 2;

bool SAS_button_pressed = false;

bool SAS_on = false;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(100);
  pinMode(SAS_led, OUTPUT);
  pinMode(SAS_button, INPUT);
  digitalWrite(SAS_led, LOW);
}

void updateGPIO() {
  if(SAS_on){
    digitalWrite(SAS_led, HIGH);
  } else {
    digitalWrite(SAS_led, LOW);
  }
}

void sendData(){
  String data = "";
  if(SAS_on){
    data += "SAS_ON";
  } else {
    data += "SAS_OFF";
  }
  Serial.println(data);
}

void processSerial(const String& message){
  if(message.indexOf("SAS_ON") > -1){
    SAS_on = true;
  } else if(message.indexOf("SAS_OFF") > -1){
    SAS_on = false;
  }

  updateGPIO();
}

void loop() {
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
}
