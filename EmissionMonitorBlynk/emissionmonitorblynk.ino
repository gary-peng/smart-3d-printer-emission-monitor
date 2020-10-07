// Libs
#include <SF_CCS811.h>
#include <blynk.h>
#include <neopixel.h>

// Blynk
#define BLYNK_PRINT Serial  // Set serial output for debug prints
//#define BLYNK_DEBUG       // Uncomment this to see detailed prints
BlynkTimer timer;
char auth[] = "Your Auth Token";

// CCS811
#define CCS811_ADDR 0x5A
uint16_t voc;
uint16_t eco2;
CCS811 mySensor(CCS811_ADDR);

// Neopixel
#define PIXEL_PIN D2
#define PIXEL_COUNT 12
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
uint8_t brightness = 50;

// buzzerPin
#define buzzerPin D3

//Danger Level
uint8_t state;
uint8_t prevState;

void setup() {
    Blynk.begin(auth);
    timer.setInterval(1000L, sendUptime); // Update every second
    
    // It is recommended to check return status on .begin(), but it is not required.
    CCS811Core::status returnCode = mySensor.begin();
    if (returnCode != CCS811Core::SENSOR_SUCCESS) {
        while (1); //Hang if there was a problem.
    }

    strip.begin();
    strip.show();
}

//Push data
void sendUptime() {
    // Push data
    Blynk.virtualWrite(V0, voc);
    
    // Display risk levles
    if(state == 2)
        Blynk.virtualWrite(V2, "High");
    else if(state == 1)
        Blynk.virtualWrite(V2, "Moderate");
    else
        Blynk.virtualWrite(V2, "LOW");
}

void loop() {
    // Run Blynk
    Blynk.run();
    timer.run();
    
    // Read sensor
    if (mySensor.dataAvailable()) {
        mySensor.readAlgorithmResults();
        voc = mySensor.getTVOC();
        eco2 = mySensor.getCO2();
    }
    
    // Change risk levels
    if(voc > 100)
        state = 2;
    else if(voc > 44)
        state = 1;
    else
        state = 0;
    
    // Phone notification
    if(state != prevState) { // No spamming
        if(state > prevState) {
            if(state == 1)
                Blynk.notify("Open a window");
            if(state == 2)
                Blynk.notify("Ventilate and Leave the Workspace");
        }
        prevState = state;
    }
    
    // Trigger alarm
    if(state == 2)
    tone(buzzerPin, 3100, 1000);
    
    
    // Change color
    for(int i=0; i<12; i++) {
        strip.setBrightness(brightness);
        if(state == 2)
            strip.setPixelColor(i, strip.Color(255, 0, 0));
        else if(state == 1)
            strip.setPixelColor(i, strip.Color(255, 255, 0));
        else
            strip.setPixelColor(i, strip.Color(0, 255, 0));
    }
    strip.show();
}
