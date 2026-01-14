#include <Adafruit_NeoPixel.h>
#include <FlashStorage.h>

#define PIN         8
#define NUMPIXELS   48
#define BUTTON_PIN  10
#define BRIGHTNESS  10

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const uint8_t NUM_MODES = 17;
uint8_t modeIndex = 0;

bool lastButtonState = HIGH;
bool currentButtonState;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
uint16_t rainbowOffset = 0;

FlashStorage(savedMode, uint8_t);

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
  return strip.Color(r, g, b);
  
}

void setup() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Load last mode from EEPROM
  modeIndex = savedMode.read();
  if (modeIndex >= NUM_MODES) modeIndex = 0;
  renderCurrentMode();
}

void loop() {
  static bool buttonWasPressed = false;
  static unsigned long buttonPressStart = 0;

  bool buttonPressed = digitalRead(BUTTON_PIN) == LOW;

  if (buttonPressed) {
    if (!buttonWasPressed) {
      // Button was just pressed
      buttonPressStart = millis();
      
    }

    // Long press: hold for 3 seconds to switch to off mode immediately
    if (millis() - buttonPressStart >= 3000 && modeIndex != 0) {
      modeIndex = 0;
      savedMode.write(modeIndex);
      renderCurrentMode();
      // Keep holding has no further effect
      
    }

    buttonWasPressed = true;
    
  } 
  else if (buttonWasPressed) {
    // Button was just released
    if (millis() - buttonPressStart < 3000  && millis() - buttonPressStart > debounceDelay) {
      // Short press: cycle to next mode
      modeIndex = (modeIndex + 1) % NUM_MODES;
      savedMode.write(modeIndex);
      renderCurrentMode();
      
    }

    buttonWasPressed = false;
    
  }

  // For animated modes, keep updating them
  if (modeIndex == 13) {
    rainbowWheel(5);
    
  } 
  else if (modeIndex == 15) {
    redWhiteBlueWave(150);
    
  }
  else if (modeIndex == 16){
    waveCycle(strip.Color(255, 0, 0), strip.Color(255, 255, 255), 3);
    
  }
  
}



void setStaticMode(uint8_t index) {
  uint32_t color = 0;

  switch (index) {
    case 0: color = rgb(0, 0, 0); break;                    // Off
    case 1: color = rgb(255, 255, 255); break;              // White
    case 2: color = rgb(255, 0, 0); break;                  // Red
    case 3: color = rgb(200, 50, 0); break;                 // Dark Orange
    case 4: color = rgb(255, 128, 0); break;                // Light Orange
    case 5: color = rgb(255, 255, 0); break;                // Yellow
    case 6: color = rgb(85, 255, 0); break;              // Light Green
    case 7: color = rgb(0, 128, 0); break;                  // Dark Green
    case 8: color = rgb(63, 180, 230); break;              // Light Blue
    case 9: color = rgb(0, 0, 255); break;                  // Blue
    case 10: color = rgb(251, 74, 255); break;             // Pink
    case 11: color = rgb(128, 0, 128); break;               // Purple
    case 12: staticRainbow(); return;                      // Static rainbow
    
  }

  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, color);
    
  }
  strip.show();
  
}

void staticRainbow() {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, colorWheel(i * 256 / NUMPIXELS));
    
  }
  strip.show();
  
}

void rainbowWheel(uint8_t wait) {
  static uint16_t j = 0;
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, colorWheel((i * 256 / strip.numPixels() + j) & 255));
    
  }
  strip.show();
  j++;
  delay(wait);
  
}

void redWhiteBlueStatic() {
  for (int i = 0; i < NUMPIXELS; i++) {
    if (i % 3 == 0)
    strip.setPixelColor(i, rgb(255, 0, 0));     // Red
    else if (i % 3 == 1)
    strip.setPixelColor(i, rgb(255, 255, 255)); // White
    else
    strip.setPixelColor(i, rgb(0, 0, 255));     // Blue
    
  }
  strip.show();
  
}

void redWhiteBlueWave(uint8_t wait) {
  static uint16_t offset = 0;
  for (int i = 0; i < NUMPIXELS; i++) {
    uint8_t pos = (i + offset) % 3;
    uint32_t color;
    switch (pos) {
      case 0: color = rgb(255, 0, 0); break;     // Red
      case 1: color = rgb(255, 255, 255); break; // White
      case 2: color = rgb(0, 0, 255); break;     // Blue
      
    }
    strip.setPixelColor(i, color);
    
  }
  strip.show();
  offset = (offset + 1) % 3;
  delay(wait);
  
}

uint32_t colorWheel(byte pos) {
  if (pos < 85) {
    return strip.Color(pos * 3, 255 - pos * 3, 0);
    
  } else if (pos < 170) {
    pos -= 85;
    return strip.Color(255 - pos * 3, 0, pos * 3);
    
  } else {
    pos -= 170;
    return strip.Color(0, pos * 3, 255 - pos * 3);
    
  }
  
}

void threeSections(){
  for (int i = 0; i < NUMPIXELS; i++) {
    if (i < 4 || i > 33)
    strip.setPixelColor(i, rgb(255, 0, 0));     // Red
    else if (i < 9 || i > 28)
    strip.setPixelColor(i, rgb(255, 255, 255)); // White
    else
    strip.setPixelColor(i, rgb(0, 0, 255));     // Blue
    
  }
  strip.show();
  
}

void waveCycle(uint32_t color1, uint32_t color2, uint8_t waveLength) {
  for (int i = 0; i < NUMPIXELS; i++) {
    int wavePos = (i + rainbowOffset) % (waveLength * 2);
    if (wavePos < waveLength) {
      strip.setPixelColor(i, color1);
      
    } else {
      strip.setPixelColor(i, color2);
      
    }
    
  }
  strip.show();
  rainbowOffset++;
  
}

void renderCurrentMode() {
  switch (modeIndex) {
    case 0: strip.clear(); strip.show(); break;
    case 1: setStaticMode(1); break;
    case 2: setStaticMode(2); break;
    case 3: setStaticMode(3); break;
    case 4: setStaticMode(4); break;
    case 5: setStaticMode(5); break;
    case 6: setStaticMode(6); break;
    case 7: setStaticMode(7); break;
    case 8: setStaticMode(8); break;
    case 9: setStaticMode(9); break;
    case 10: setStaticMode(10); break;
    case 11: setStaticMode(11); break;
    case 12: staticRainbow(); break;
    case 13: rainbowWheel(5); break;
    case 14: redWhiteBlueStatic(); break;
    case 15: redWhiteBlueWave(150); break;
    case 16: waveCycle(strip.Color(255, 0, 0), strip.Color(255, 255, 255), 3); break;
    
  }
  
}
