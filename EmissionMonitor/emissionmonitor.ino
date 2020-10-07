// This #include statement was automatically added by the Particle IDE.
#include <neopixel.h>

// This #include statement was automatically added by the Particle IDE.
#include <SF_CCS811.h>

// CCS811
#define CCS811_ADDR 0x5A
int voc;
CCS811 mySensor(CCS811_ADDR);

// Neopixel
#define PIXEL_PIN D2
#define PIXEL_COUNT 12
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
int brightness = 50;

// Buzzer
#define BUZZERPIN D3

// Danger Levels
String risk;
String action;
int state;

void setup() {
    // It is recommended to check return status on .begin(), but it is not required.
    CCS811Core::status returnCode = mySensor.begin();
    if (returnCode != CCS811Core::SENSOR_SUCCESS) {
        while (1); //Hang if there was a problem.
    }
    
    strip.begin();
    strip.show();
    
    Particle.variable("voc", voc);
    Particle.variable("risk", risk);
    Particle.variable("action", action);
}


void loop() {
    // Read sensor
    if (mySensor.dataAvailable()) {
        mySensor.readAlgorithmResults();
        voc = (int) mySensor.getTVOC();
    }
    
    // Change risk levels
    if (voc > 100) {
        tone(BUZZERPIN, 3100, 1000);
        state = 2;
        risk = "HIGH";
        action = "VENTILATE AND LEAVE THE WORKSPACE";
    } else if (voc > 44) {
        state = 1;
        risk = "MODERATE";
        action = "OPEN A WINDOW";
    } else {
        state = 0;
        risk = "LOW";
        action = "Happy Printing :)";
    }
      
      
    // Change color
    for (int i=0; i<12; i++) {
        strip.setBrightness(brightness);
        if (state == 2)
            strip.setPixelColor(i, strip.Color(255, 0, 0));
        else if (state == 1)
            strip.setPixelColor(i, strip.Color(255, 255, 0));
        else
            strip.setPixelColor(i, strip.Color(0, 255, 0));
        }
    strip.show();
}
