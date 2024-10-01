/*
  Project Name: Two-Way Input Control

  Members:
    Daniel Martinez
    Hisham Chaudhry

  NetIds:
    Dmart87
    Hchau6

  Group#: 25

  Abstract:
  This project brings together three Arduino boards UNO, MEGA, 
  and NANO to craft a versatile control system that responds 
  to both button presses and voice commands. The UNO handles 
  inputs from several buttons, each programmed to perform 
  specific actions like turning on LEDs or sounding a buzzer. 
  These commands are sent to the MEGA through serial communication. 
  Meanwhile, the NANO is tuned to recognize voice commands to activate 
  the same functions. The MEGA manages outputs to LEDs, a buzzer, and 
  an LCD screen that displays whether it's in manual or voice mode, 
  along with the current action. A switch on the NANO lets users toggle 
  between these input modes, illustrating the system’s adaptability and 
  interactive capabilities.

*/



// The purpose of this Arduino is to send
// serial messages to the Arduino MEGA to
// indicate which function to execute based on 
// button press
int btns[7] = {7,6,5,4,3,2,8};

int toggleStates[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW};


unsigned long lastDebounce = 0;
unsigned long debounce = 200;

void setup() {
  Serial.begin(9600);
  for(int i = 0; i < 7; i++) {
    pinMode(btns[i], INPUT);
  }
  // put your setup code here, to run once:

}

void loop() {
  // reads in current states for all buttons
  if( (millis() - lastDebounce) > debounce) {
    for(int i = 0; i < 7; i++) {
      int currState = digitalRead(btns[i]);

      if(toggleStates[i] != currState) {
        toggleStates[i] = currState;

        if(toggleStates[i] == LOW) {
         switch(i) {
           case 0:
           Serial.write('0');
           break;
           
           case 1:
           Serial.write('1');
           break;
           
           case 2:
           Serial.write('2');
           break;
           
           case 3:
           Serial.write('3');
           break;
           
           case 4:
           Serial.write('4');
           break;
           
           case 5:
           Serial.write('5');
           break;

           case 6:
           Serial.write('6');
           break;

           default:
           break;
         }
        }
      }
    }
  }
}
