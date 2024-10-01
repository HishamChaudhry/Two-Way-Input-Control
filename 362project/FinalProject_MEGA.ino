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

#include <LiquidCrystal.h>

int toggleBtn = 6;
int leds[4] = { 10, 9, 8, 7 };
int ledVals[4] = { 0, 0, 0, 0 };
int buzzerTones[5] = { 440, 494, 523, 587, 659 };
int buzzerTones2[5] = { 400, 1000, 1200, 400, 950 };
bool ledArePulsing[4] = { 0, 0, 0, 0 };
int policeTones[2] = {900, 400};
int policeLEDs[2] = {10, 7}; // Define LED pins array
char voiceCmds[7] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G' };
char manualCmds[7] = { '0', '1', '2', '3', '4', '5', '6' };
char inputCmds[7] = {};

int toneIndex = 0,
    toneIndex2 = 0,
    toneIndex3 = 0;
unsigned long lastTime = 0,
              lastTime2 = 0,
              lastTime3 = 0;
bool buzzerPlaying = false,
     buzzerPlaying2 = false,
     buzzerPlaying3 = false;

int buzzer = 11;

// struct LED_state {
//   int brightness,
//   	  fadeInterval;
//   unsigned long prev;
// };

// LED_state ledStates[4];

int toggleState = LOW;

unsigned long lastDebounce = 0;
unsigned long debounce = 20;

int interval = 10;
int interval2 = 50;
int interval3 = 500;

unsigned long prev = 0;

int btnState = LOW,
    prevBtnState;

int inputCommand = 100;
int pulseInterval = 5;
int currLEDIdx = 0;

// 0 = manual mode
// 1 = voice mode
bool mode = 0;

String currMode = "",
       currFunc = "",
       prevMode = "",
       prevFunc = "";

void clearScreen(int row);  // not used
void changeMode();          // not used
void ledAllOn();
void ledAllOff();
void ledPulse();
void ledPulseSingle(int ledArr[4], int ledValArr[4], int ledIdx);
void buzzerTune1();
void buzzerTune2();
void policeMode();
void ledPulseSequence();

LiquidCrystal screen(13, 12, 5, 4, 3, 2);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  screen.begin(16, 2);
  pinMode(toggleBtn, INPUT);
  pinMode(buzzer, OUTPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);
  }

  for (int i = 0; i < 4; i++) {
    ledArePulsing[i] = false;
  }
  currLEDIdx = 0;  // Start pulsing from the first LED
}

void loop() {
  char input;

  prevMode = currMode;
  prevFunc = currFunc;

  if (!mode) {
    currMode = "Manual Mode";
    for (int i = 0; i < 7; i++) {
      inputCmds[i] = manualCmds[i];
    }
  } else {
    currMode = "Voice Mode";
    for (int i = 0; i < 7; i++) {
      inputCmds[i] = voiceCmds[i];
    }
  }
  Serial.print(input);
  Serial.print("   ");
  Serial.print(inputCommand);
  Serial.print("\n");

  if (prevMode != currMode || prevFunc != currFunc) {
    screen.clear();
    screen.setCursor(0, 0);
    screen.print(currMode);
    screen.setCursor(0, 1);
    screen.print(currFunc);
  }

  if(Serial1.available() ) {
    input = Serial1.read();
  }

  else if(Serial.available()) {
    input = Serial.read();
  }

  if (input == inputCmds[0]) {
    inputCommand = 0;
    currFunc = "LEDs On";

  } else if (input == inputCmds[1]) {
    inputCommand = 1;
    currFunc = "LEDs Pulse";

  } else if (input == inputCmds[2]) {
    inputCommand = 2;
    currFunc = "LEDs Pulse Seq.";

  } else if (input == inputCmds[3]) {
    inputCommand = 3;
    currFunc = "Buzzer1";

  } else if (input == inputCmds[4]) {
    inputCommand = 4;
    currFunc = "Buzzer2";

  } else if (input == inputCmds[5]) {
    inputCommand = 5;
    currFunc = "Siren";

  } else if (input == inputCmds[6]) {
    inputCommand = 6;
    currFunc = "All Off";
  }

  // if(input == inputCmds[0]) {
  if (inputCommand == 0) {
    if (currFunc != prevFunc) {
      // clearScreen(1);
      ledAllOff();
      screen.clear();
      screen.setCursor(0, 0);
      screen.print(currMode);
      screen.setCursor(0, 1);
      screen.print(currFunc);
    }
    ledAllOn();
    // screen.setCursor(0,1);
    // screen.print(currFunc);
  }

  else if (inputCommand == 1) {

    if (currFunc != prevFunc) {
      ledAllOff();
      screen.clear();
      screen.setCursor(0, 0);
      screen.print(currMode);
      screen.setCursor(0, 1);
      screen.print(currFunc);
    }
    ledPulse();
  }

  else if (inputCommand == 2) {

    if (currFunc != prevFunc) {
      ledAllOff();
      screen.clear();
      screen.setCursor(0, 0);
      screen.print(currMode);
      screen.setCursor(0, 1);
      screen.print(currFunc);
    }
    unsigned long curr = millis();
    if (curr - prev >= interval2) {
      prev = curr;
      // Pulse the current LED
      ledPulseSingle(leds[currLEDIdx], ledVals[currLEDIdx], pulseInterval);

      // Move to the next LED in sequence
      currLEDIdx = (currLEDIdx + 1) % 4;
    }
  }

  else if (inputCommand == 3) {

    if (currFunc != prevFunc) {
      ledAllOff();
      screen.clear();
      screen.setCursor(0, 0);
      screen.print(currMode);
      screen.setCursor(0, 1);
      screen.print(currFunc);
    }
    buzzerTune1();
  }

  else if (inputCommand == 4) {
    if (currFunc != prevFunc) {
      ledAllOff();
      screen.clear();
      screen.setCursor(0, 0);
      screen.print(currMode);
      screen.setCursor(0, 1);
      screen.print(currFunc);
    }
    buzzerTune2();
  }

  else if (inputCommand == 5) {

    if (currFunc != prevFunc) {
      // clearScreen(1);
      ledAllOff();
      screen.clear();
      screen.setCursor(0, 0);
      screen.print(currMode);
      screen.setCursor(0, 1);
      screen.print(currFunc);
    }
    policeMode();
  }

  else if (inputCommand == 6) {
    if (currFunc != prevFunc) {
      // clearScreen(1);
      ledAllOff();
      screen.clear();
      screen.setCursor(0, 0);
      screen.print(currMode);
      screen.setCursor(0, 1);
      screen.print(currFunc);
    }
    ledAllOff();
  }


  if ((millis() - lastDebounce) > debounce) {
    int currState = digitalRead(toggleBtn);

    if (btnState != currState) {
      btnState = currState;

      if (btnState == LOW) {
        //Toggles from manual to vocal mode
        //and vice versa.
        ledAllOff();
        currFunc = "";
        mode = !mode;
      }
    }

    // Serial.print("\n");
    // Serial.println(currFunc);
    // Serial.println(currMode);
    // Serial.print("\n");
    
  }
}


void changeMode() {
  Serial.println("Button 2 Command Recieved!");
}

void clearScreen(int row) {
  if (row == 0) {
    screen.setCursor(0, 0);
  } else if (row == 1) {
    screen.setCursor(0, 1);
  }
  screen.print("");
}

void ledAllOn() {
  Serial.println("Button 1 Command Recieved!");
  ledAllOff();
  for (int i = 0; i < 4; i++) {
    analogWrite(leds[i], 255);
  }
}

void ledAllOff() {
  for (int i = 0; i < 4; i++) {
    analogWrite(leds[i], 0);
    ledArePulsing[i] = false;
    ledVals[i] = 0;
  }
  noTone(buzzer);
}

void ledPulse() {
  unsigned long curr = millis();
  if (curr - prev >= interval) {
    for (int i = 0; i < 4; i++) {
      prev = curr;
      // Increases Brightness of LEDs until they are at their brightess value (255)
      if (ledVals[i] <= 0) {
        pulseInterval = 5;
        // Starts dimming the LEDs when they are at their brightest
      } else if (ledVals[i] >= 255) {
        pulseInterval = -5;
      }

      ledVals[i] += pulseInterval;

      analogWrite(leds[i], ledVals[i]);
    }
  }
}

void buzzerTune1() {
  unsigned long currentTime = millis();

  if (!buzzerPlaying) {
    // Start playing the tune
    buzzerPlaying = true;
    tone(buzzer, buzzerTones[toneIndex]);

    // Turn on the corresponding LED
    if (toneIndex < 4) {
      digitalWrite(leds[toneIndex], HIGH);  // Turn on LED corresponding to tone
    } else {
      // If it's the 5th tone, turn on all LEDs
      for (int i = 0; i < 4; i++) {
        digitalWrite(leds[i], HIGH);
      }
    }
  }

  // Check if it's time to move to the next tone
  if (currentTime - lastTime >= 500) {
    lastTime = currentTime;

    // Move to the next tone
    toneIndex++;

    // Check if all tones have been played
    if (toneIndex >= 5) {
      // Reset state when finished
      toneIndex = 0;
      buzzerPlaying = false;
      ledAllOff();     // Turn off all LEDs
      noTone(buzzer);  // Stop the buzzer
    } else {
      // Play the next tone and update corresponding LEDs
      tone(buzzer, buzzerTones[toneIndex]);

      if (toneIndex < 4) {
        // Turn off previous LED and turn on the next LED
        digitalWrite(leds[toneIndex - 1], LOW);
        digitalWrite(leds[toneIndex], HIGH);
      } else {
        // Turn off all LEDs except for the 5th tone
        for (int i = 0; i < 4; i++) {
          digitalWrite(leds[i], LOW);
        }
        // Turn on all LEDs for the 5th tone
        for (int i = 0; i < 4; i++) {
          digitalWrite(leds[i], HIGH);
        }
      }
    }
  }
}


void buzzerTune2() {
  unsigned long currentTime = millis();

  if (!buzzerPlaying2) {
    // Start playing the tune
    buzzerPlaying2 = true;
    tone(buzzer, buzzerTones2[toneIndex2]);

    // Turn on the corresponding LED based on toneIndex2
    if (toneIndex2 == 0) {
      digitalWrite(leds[0], HIGH); // Turn on 1st LED for the first tone
    } else if (toneIndex2 == 1) {
      digitalWrite(leds[3], HIGH); // Turn on last LED for the second tone
    } else if (toneIndex2 == 2) {
      digitalWrite(leds[1], HIGH); // Turn on 2nd LED for the third tone
    } else if (toneIndex2 == 3) {
      digitalWrite(leds[2], HIGH); // Turn on 3rd LED for the fourth tone
    } else if (toneIndex2 == 4) {
      // Turn on all LEDs for the fifth tone
      for (int i = 0; i < 4; i++) {
        digitalWrite(leds[i], HIGH);
      }
    }
  }

  // Check if it's time to move to the next tone
  if (currentTime - lastTime2 >= 800) {
    lastTime2 = currentTime;

    // Move to the next tone
    toneIndex2++;

    // Check if all tones have been played
    if (toneIndex2 >= 5) {
      // Reset state when finished
      toneIndex2 = 0;
      buzzerPlaying2 = false;
      ledAllOff(); // Turn off all LEDs
      noTone(buzzer); // Stop the buzzer
    } else {
      // Play the next tone and update corresponding LED
      tone(buzzer, buzzerTones2[toneIndex2]);

      // Turn off previous LED and turn on the next LED
      if (toneIndex2 == 1) {
        digitalWrite(leds[0], LOW); // Turn off 1st LED
        digitalWrite(leds[3], HIGH); // Turn on last LED for the second tone
      } else if (toneIndex2 == 2) {
        digitalWrite(leds[3], LOW); // Turn off last LED
        digitalWrite(leds[1], HIGH); // Turn on 2nd LED for the third tone
      } else if (toneIndex2 == 3) {
        digitalWrite(leds[1], LOW); // Turn off 2nd LED
        digitalWrite(leds[2], HIGH); // Turn on 3rd LED for the fourth tone
      } else if (toneIndex2 == 4) {
        digitalWrite(leds[2], LOW); // Turn off 3rd LED
        // Turn on all LEDs for the fifth tone
        for (int i = 0; i < 4; i++) {
          digitalWrite(leds[i], HIGH);
        }
      }
    }
  }
}



void ledPulseSingle(int ledPin, int& brightness, int& pulseInterval) {
  if (brightness <= 0) {
    pulseInterval = 51;  // Increase brightness
  } else if (brightness >= 255) {
    pulseInterval = -51;  // Decrease brightness
  }

  // Update LED brightness
  brightness += pulseInterval;
  analogWrite(ledPin, brightness);
}
/*
int toneIndex2 = 0;
unsigned long lastTime2 = 0;
bool buzzerPlaying2 = false;
*/
void policeMode() {
  unsigned long currentTime = millis();

  if (!buzzerPlaying3) {
    // Start police mode
    buzzerPlaying3 = true;
    tone(buzzer, policeTones[toneIndex3]);

    // Turn on the corresponding LED based on policeModeIndex
    if (toneIndex3 == 0) {
      digitalWrite(policeLEDs[0], HIGH); // Turn on 1st LED for the first tone
    } else if (toneIndex3 == 1) {
      digitalWrite(policeLEDs[1], HIGH); // Turn on last LED for the second tone
    }
  }

  // Check if it's time to move to the next police mode step
  if (currentTime - lastTime3 >= 500) {
    lastTime3 = currentTime;

    // Move to the next police mode step
    toneIndex3++;

    // Check if all police mode steps have been completed
    if (toneIndex3 >= 2) {
      // Reset state when finished
      toneIndex3 = 0;
      buzzerPlaying3 = false;
      ledAllOff(); // Turn off all LEDs
      noTone(buzzer); // Stop the buzzer
    } else {
      // Play the next tone and update corresponding LED
      tone(buzzer, policeTones[toneIndex3]);

      // Turn off previous LED and turn on the next LED
      if (toneIndex3 == 1) {
        digitalWrite(policeLEDs[0], LOW); // Turn off 1st LED
        digitalWrite(policeLEDs[1], HIGH); // Turn on last LED for the second tone
      }
    }
  }
}
