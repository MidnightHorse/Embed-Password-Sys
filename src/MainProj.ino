/* Austin Haugland && ahaug004@ucr.edu
 * Discussion Section: 24
 * Assignment: Custom Lab Final Demo
 * I acknowledge all content contained herein, excluding template or example code, is my own original work.
 * Demo video Link: https://www.youtube.com/watch?v=JC2Sf3PKna4
 */
#include <LiquidCrystal.h>
#include <Keypad.h>

// Array of Output Pin variables, set to the pins being used
const int rs = 22, en = 24, d4 = 26, d5 = 28, d6 = 30, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int light_pattern_size = 2;
const char light_pattern[light_pattern_size] = {0x00, 0x0F};
const int Password_Length = 8;
const String Master = "00101010";
String Data;
 unsigned char j = 0;
 unsigned char k = 0;
unsigned char counter = 0;
bool pass_correct = false;
bool denied = false;
unsigned int time = 0;
unsigned char r = 1;


//Shift Registers
unsigned int latchPin = 3;
unsigned int clockPin= 2;
unsigned int dataPin = 4;
unsigned int resetPin = 5;
unsigned int layer[4] = {0, 0, 0, 0};
byte LEDS = 0;
byte dataArray[10];
//Buttons
const int a_size = 2;
unsigned char X = 34;
unsigned char Y = 36;
unsigned char a[a_size] = {34, 36};
unsigned char a_type[a_size] = {0, 1};
bool x_release = true;
bool y_release = true;

//const char sound_pattern_size = 2;
//const double sound_pattern[sound_pattern_size] = {261.63, 293.66}; // C4, D4

const int buzzer = 3;

//Keypad
const int ROW_NUM = 4;
const int COLUMN_NUM = 4;

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {6, 7, 8, 9};
byte pin_column[COLUMN_NUM] = {10, 11, 12, 13};
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
int cursorColumn = 0;

int ButtonFunction(int button){
  //works with pulldown
  if(button == 36){
    return digitalRead(button);
  }
  //Works with pullup
  else if(button == 34){
    return digitalRead(button);
  }
  else{
    return 0;
  }
}
void Setup(unsigned int val){
  layer[0] = val;
  layer[1] = val;
  layer[2] = val;
  layer[3] = val;
}
void setShiftReg(unsigned int val){
     digitalWrite(latchPin, LOW);
     shiftOut(dataPin, clockPin, MSBFIRST, val >> 8);
     shiftOut(dataPin, clockPin, MSBFIRST, val);
     digitalWrite(latchPin, HIGH);
}
void rand(int i){
  LEDS = ((i * 77 + r) % 1000);
  r = ((i * r) % 10);
  return LEDS;
}
int random (int i){
  LEDS = ((i + r) % 16);
  r = (((LEDS * 22) % 100));
  return LEDS;
}

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
    
} task;

int delay_gcd;
const unsigned short tasksNum = 3;
task tasks[tasksNum];

//Keypad state
enum SM1_States { SM1_INIT, SM1_S0, SM1_S1};
int SM1_Tick(int state){
  char key = keypad.getKey();
  //unsigned char k;
    //Read thing
    switch(state){ // State transitions
      case SM1_INIT:
         //State Transition
            state = SM1_S0;
        break;
      case SM1_S0:
      if(pass_correct == true){
        state = SM1_S1;
        j = 0;
        lcd.clear();
        lcd.print("ACCESS GRANTED");
      }
      else{
        state = SM1_S0;
      }
         //State Transition
      break;
      case SM1_S1:
      if(pass_correct == false){
          state = SM1_INIT;
        }
        else{
          j++;
          state = SM1_S1;
        }
      break;
    }
    switch(state){ // State Action
      case SM1_INIT:
      break;
      case SM1_S0:
      if(key){
        //General rule is that strings are bad for passwords but for simplicity let's use it
        j = 0;
        lcd.setCursor(cursorColumn, 0);
        lcd.print(key);
        
        k++;
        cursorColumn++;
        //Reset function
        if(key == '#'){
          lcd.clear();
          k = 0;
          Data = "";
          cursorColumn = 0;
        }
        if(key == '*'){
        lcd.clear();
        k = 0;
        if(Master == Data){
        pass_correct = true;
        Data = "";
        }
        else{
          denied = true;
        }
        }
        Data += key;
      }
      //Failure
      if(k > Password_Length || denied == true){
          if(j == 0){
            lcd.clear();
            lcd.print("ACCESS DENIED");
          }
          if(j >= 50){
            lcd.clear();
            k = 0;
            Data = "";
            cursorColumn = 0;
            denied = false;
          }
          j++;
          Serial.println(j);
        }
         //State Action
      break;
      case SM1_S1:
      if(j == 100){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Press any button");
        lcd.setCursor(0, 1);
        lcd.print("to display light show");
      }
      else if(j == 150){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Press the # key");
        lcd.setCursor(0, 1);
        lcd.print("to exit.");
      }
      else if(j == 200){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ENJOY!");
      }
      Data = "";
      if(key == '#'){
        pass_correct = false;
        k = 0;
        cursorColumn = 0;
        Data = "";
        lcd.clear();
      }
      Serial.println(j);
      break;
    }
    return state;
}
//Button to enter/clear/(LED States in final)
enum SM2_States { SM2_INIT, SM2_S0};
int SM2_Tick(int state){
    //Button
    switch(state){ // State transitions
      case SM2_INIT:
      if(pass_correct == true){
        state = SM2_S0;
        counter = 1;
      }
      else{
        state = SM2_INIT;
      }
        break;
      case SM2_S0:
      if(pass_correct != true){
        state = SM2_INIT;
      }
      else if(counter == 3){
        counter = 2;
      }
      else if(counter == 0){
        counter = 1;
      }
      else{
      state = SM2_S0;
      }
      break;
    }
    switch(state){ // State Action
      case SM2_INIT:
      counter = 0;
         //State Action
        break;
      case SM2_S0:
      if(ButtonFunction(X) && x_release){ //CLICK
            x_release = false;
      }
      if(!ButtonFunction(X) && !x_release){ //CLICK
            x_release = true;
            counter++;
      }
      if(ButtonFunction(Y) && y_release){ //CLICK
            y_release = false;
      }
      if(!ButtonFunction(Y) && !y_release){ //CLICK
            y_release = true;
            counter--;
      }
      Serial.println(counter);
      break;
    }
    return state;
}
//Shift Registers and 4 LED states
enum SM3_States { SM3_INIT, SM3_S0, SM3_S1, SM3_S2, /*SM3_S3*/};
int SM3_Tick(int state){
    //Button
    switch(state){ // State transitions
      case SM3_INIT:
      if(counter != 0){ //PASSWORD STATE WAITING
        state = SM3_S0;
      }
      else{
        LEDS = 0;
        state = SM3_INIT;
      }
        break;
      case SM3_S0: //LEDS OFF
      if(counter == 2){ //CLICK
        LEDS = 0;
        state = SM3_S1;
      }
      //Return home
      else if(counter == 0){
        LEDS = 0;
        state = SM3_INIT;
      }
      else{
        state = SM3_S0;
      }
        break;
      case SM3_S1: //LEDS FULL CUBE
      if(counter == 3){
        LEDS = 0;
        state = SM3_S2;
      }
      else if(counter == 1 || counter == 0){
        LEDS = 0;
        state = SM3_S0;
      }
      else{
        state = SM3_S1;
      }
      break;
      case SM3_S2: //LEDS RANDOM
      if(counter == 2 || counter == 0){
        LEDS = 0;
        state = SM3_S1;
      }
      else{
        state = SM3_S2;
      }
      break;
    }
    switch(state){ // State Action
      case SM3_INIT: //PASSWORD WAIT
      if(LEDS < 256){
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, LEDS);
      digitalWrite(latchPin, HIGH);
      time++;
      }
         //State Action
        break;
      case SM3_S0: //OFF
      if(LEDS < 256){
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, LEDS);
      digitalWrite(latchPin, HIGH);
      time++;
      }
        break;
      case SM3_S1: //Normal
      LEDS = 255;
      if(LEDS == 255){
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, LEDS);
      digitalWrite(latchPin, HIGH);
      time++;
      //Serial.println(LEDS);
      }
      break;
     case SM3_S2: //RANDOM
     rand(time);
     digitalWrite(latchPin, LOW);
     shiftOut(dataPin, clockPin, MSBFIRST, LEDS);
     digitalWrite(latchPin, HIGH);
     time++;
     break;
    }

    return state;
}

void setup() {
    //some set up (Default Arduino Function)
    pinMode(X, INPUT);
    pinMode(Y, INPUT_PULLUP);
    pinMode(latchPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, LOW);
    digitalWrite(resetPin, HIGH);
    // LEDs
  dataArray[0] = 0xFF;
  dataArray[1] = 0xFE;
  dataArray[2] = 0xFC;
  dataArray[4] = 0xF0;
  dataArray[5] = 0xE0;
  dataArray[6] = 0xC0;
  dataArray[7] = 0x80;
  dataArray[8] = 0x00;
  dataArray[9] = 0xE0;
   
    Serial.begin(9600);

  unsigned char i = 0;
  tasks[i].state = SM1_INIT;
  tasks[i].period = 100;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM1_Tick;
  i++;
  tasks[i].state = SM2_INIT;
  tasks[i].period = 200;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM2_Tick;
  i++;
  tasks[i].state = SM3_INIT;
  tasks[i].period = 500;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM3_Tick;
  lcd.begin(16,2);
}


void loop() {
  // put your main code here, to run repeatedly:
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) {
     if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
        tasks[i].state = tasks[i].TickFct(tasks[i].state);
        tasks[i].elapsedTime = millis(); // Last time this task was ran
     }
   }
}
//Due to time constraint of the demo, There will only be the necessary presentation of complexities and basic functionality
