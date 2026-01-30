#include <MIDIUSB.h>

#define NUM_SENSORS 9

// Analog inputs
const uint8_t sensorPins[NUM_SENSORS] = {
  A0, A1, A2, A3, A4, A5, A7, A8, A10
  
};

// Drums in same order as Analog inputs 
const uint8_t midiNotes[NUM_SENSORS] = {
  57, // Crash Cymbal 2
  55, // Crash Cymbal 1
  48, // Hi-Mid Tom
  42, // Closed Hi Hat
  43, // Floor Tom
  36, // Kick Drum
  38, // Snare 
  51, // Ride Cymbal
  47 // Low-Mid Tom
  
};

// Constants / Thresholds
const uint8_t midiChannel = 9; 
const int HIT_THRESHOLD   = 11; // Updated as required 
const int CONST_VELOCITY = 100;

// State tracking
bool hitDetected[NUM_SENSORS];
int value = 0;

void setup() {
  // Lower ADC Prescaler for faster ADC conversion time 
  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2));
  ADCSRA |= bit (ADPS0) | bit (ADPS2);    

  for (int i = 0; i < NUM_SENSORS; i++) {
    hitDetected[i] = false;
    
  }
  
}

void loop() {
  // ---------- 1: SCAN ALL SENSORS ----------
  bool anyHit = false;
  for (int i = 0; i < NUM_SENSORS; i++) {
    value = analogRead(sensorPins[i]);
    if (value > HIT_THRESHOLD) {
      hitDetected[i] = true;
      anyHit = true;
      
    }
    
  }

  // ---------- 2: HANDLE ALL HITS TOGETHER ----------
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (hitDetected[i]) {
      sendNoteOn(midiNotes[i],CONST_VELOCITY); 
      hitDetected[i] = false;
      
    }
    
  }

  // Flush once per loop if hits sent 
  if (anyHit) {
    MidiUSB.flush();
    
  }
  
}

// ---------- MIDI HELPERS ----------
void sendNoteOn(uint8_t note, uint8_t velocity) {
  midiEventPacket_t noteOn = {
    0x09, 0x90 | midiChannel, note, velocity
    
  };
  MidiUSB.sendMIDI(noteOn);
  
}

void sendNoteOff(uint8_t note) {
  midiEventPacket_t noteOff = {
    0x08, 0x80 | midiChannel, note, 0
    
  };
  MidiUSB.sendMIDI(noteOff);
  
}
