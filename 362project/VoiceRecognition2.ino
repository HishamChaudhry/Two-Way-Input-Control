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


/*
    VoiceRecognition.ino
        Demonstrate the voice recognition technology.

    This sketch supports the following board types:
        Arduino Nano RP2040 Connect
            https://store.arduino.cc/products/arduino-nano-rp2040-connect
        Arduino Nano 33 BLE Sense
            https://store.arduino.cc/products/arduino-nano-33-ble-sense
        Arduino Portenta H7
            https://store.arduino.cc/products/portenta-h7

    To run the free demo with a pre-defined keyword set:
        1. Verify & upload the GetSerialNumber.ino sketch to get the board's serial number and print it on the console output.
        2. Go to the DSpotter Free Demo License Page:
            https://tool.cyberon.com.tw/ArduinoDSpotterAuth/FDMain.php
        3. Enter the board's serial number and click Submit. The license data of the board will show on the webpage.
        4. Copy & paste the license data into the CybLicense.h file under your sketch folder.
        5. Verify & upload the sketch to the board.
        6. The voice recognition is ready to go.

    To try out a custom keyword set:
        Users can create their own custom keyword sets through the DSpotter Model Configuration Page:
            https://tool.cyberon.com.tw/ArduinoDSpotterAuth/CTMain.php
        and obtain the Custom Trial Model & the corresponding Custom Trial License to test on the board.
            1. Go to the DSpotter Model Configuration Page.
            2. Select the board type of your board, enter your email address and enter the board's serial number.
            3. Read & agree to the end-user license agreement, then click Next.
            4. You can choose to create a new model set or import model from an existing .dsproj file.
            5. To create a new model, select the desired language for the model and click Create.
            6. You can set the trigger & command keyword simply by text input.
                    The keyword ID will be used in the sketch to identify the speech recognition results.
            7. After completing trigger keyword configuration, click Next and proceed to command keyword configuration.
            8. If all the configurations in the review project page are correct, click Confirm.
                    The Custom Trial Model, the corresponding Custom Trial License, and the .dsproj file will be sent to your email.
            9. For example, you would receive files like:
                    Custom Trial Model     Model_xxxxxxxxxxx.h
                    Custom Trial License   CybLicense_xxxxxxxxxxx.h
                    DSpotter Project File  Model_xxxxxxxxx_Arduino_[board type].dsproj
               xxxxxxxxxxx is a random tamp number use to differ each download.
               Copy & paste Model_xxxxxxxxxxx.h and CybLicense_xxxxxxxxxxx.h to your sketch folder.
            10. Modify the following lines in the sketch:
                    #include "CybLicense.h" ----> #include "CybLicense_xxxxxxxxxxx.h"
                    #include "Model_L1.h" ----> #include "Model_xxxxxxxxxxx.h"
            11. Verify & upload the sketch to the board.
            12. The voice recognition with your custom keyword set is ready to go.
        Please note that the Custom Trial Model and the Custom Trial License are free to use with some limitations:
            1. Recognize 50 times each reboot. Once the number of recognitions is reached, the model will stop until the next reboot.
            2. There is a 20-second delay between entering the trigger mode and starting to recognize the trigger mode keywords.

    To unlock the limitations of the Custom Trial version:
        Users can remove the limitations of the Custom Trial version by upgrading to the Custom Formal version
        through the DSpotter License Activation Page:
            https://tool.cyberon.com.tw/ArduinoDSpotterAuth/CFMain.php
        to obtain the Custom Formal Model & the corresponding Custom Formal License for the final product.
            1. Purchase a valid voucher code from Arduino Online Store:
                    https://store.arduino.cc/speech-recognition-engine
            2. Go to the DSpotter License Activation Page.
            3. Select the board type of your board, then enter your email address, the board's serial number, and the voucher code you just purchased.
            4. Import the .dsproj file you received with your tested Custom Trial Model.
            5. Read & agree to the end-user license agreement, then click Next.
            6. If all the configurations in the review project page are correct, click Confirm.
                    The Custom Formal Model, the corresponding Custom Formal License will be sent to your email.
            7. For example, you would receive files like:
                    Custom Formal Model     Model_xxxxxxxxxxx.h
                    Custom Formal License   CybLicense_xxxxxxxxxxx.h
               xxxxxxxxxxx is a random tamp number use to differ each download.
               Please keep these data properly.
               Copy & paste Model_xxxxxxxxxxx.h and CybLicense_xxxxxxxxxxx.h to your sketch folder.
            8. Modify the following lines in the sketch:
                    #include "CybLicense.h" ----> #include "CybLicense_xxxxxxxxxxx.h"
                    #include "Model_L1.h" ----> #include "Model_xxxxxxxxxxx.h"
            9. Verify & upload the sketch to the board.
            10.The voice recognition for the custom keyword set is ready and without any limitation.

    16 Feb 2023 by Cyberon Corporation.
    https://www.cyberon.com.tw/index.php?lang=en
*/
#include <Arduino.h>
#include <DSpotterSDK_MakerHL.h>
#include <LED_Control.h>

// The DSpotter License Data.
#include "CybLicense_1713640979.h"
#define DSPOTTER_LICENSE g_lpdwLicense

// The DSpotter Keyword Model Data.
#if defined(TARGET_ARDUINO_NANO33BLE) || defined(TARGET_PORTENTA_H7) || defined(TARGET_NICLA_VISION)
// For ARDUINO_NANO33BLE and PORTENTA_H7
#include "Model_L1.h"             // The packed level one model file.
// For NANO_RP2040_CONNECT
#elif defined(TARGET_NANO_RP2040_CONNECT)
#include "Model_1713640979.h"             // The packed level zero model file.
#endif
#define DSPOTTER_MODEL g_lpdwModel

// The VR engine object. Only can exist one, otherwise not worked.
static DSpotterSDKHL g_oDSpotterSDKHL;

void VRCallback(int nFlag, int nID, int nScore, int nSG, int nEnergy)
{
    if (nFlag == DSpotterSDKHL::GetResult)
    {
        // Handle the recognized command based on its ID
        switch (nID)
        {
            case 100: // Hello Bob
                Serial.println("Hello Bob!");
                LED_RGB_Off();
                LED_BUILTIN_Off();
                break;

            case 10001: // LED On
                Serial.println("Lights On");
                Serial1.write('A');
                break;

            case 10002: // LED Off
                Serial.println("Lights Off");
                Serial1.write('G');
                break;

            case 10003: // Toggle LED
                Serial.println("Toggle Lights");
                // Serial1.write('A');
                break;

            case 10004: // LED Pulse
                Serial.println("LED Pulse");
                Serial1.write('B');
                // Implement LED pulsing logic
                break;

            case 10005: // LED Pulse two
                Serial.println("LED Pulse Two");
                Serial1.write('C');
                // Implement another LED pulsing logic
                break;

            case 10006: // Buzzer Tone One
                Serial.println("Buzzer Tone One");
                Serial1.write('D');
                // Trigger buzzer tone 1
                break;

            case 10007: // Buzzer Tone Two
                Serial.println("Buzzer Tone Two");
                Serial1.write('E');
                break;

            case 10008: // Buzzer Tone Two
                Serial.println("Buzzer Tone Three");
                Serial1.write('F');
                break;

            default:
                break;
        }
    }
}

void setup()
{
  // Init LED control
  LED_Init_All();

  // Init Serial output for show debug info
  Serial.begin(9600);
  Serial1.begin(9600);
  // Serial2.begin(9600);
  while(!Serial);
  DSpotterSDKHL::ShowDebugInfo(true);

  // Init VR engine & Audio
  if (g_oDSpotterSDKHL.Init(DSPOTTER_LICENSE, sizeof(DSPOTTER_LICENSE), DSPOTTER_MODEL, VRCallback) != DSpotterSDKHL::Success)
    return;
}

void loop()
{
  // Do VR
  g_oDSpotterSDKHL.DoVR();
}

