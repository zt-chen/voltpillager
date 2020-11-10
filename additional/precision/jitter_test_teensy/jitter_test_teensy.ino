// include the SPI library:
// Include timer library
#include "TeensyTimerTool.h"

using namespace TeensyTimerTool;
OneShotTimer t1(TMR1);
float pscValue = 1 << (PSC_1 & 0b0111);
uint32_t pscBits = 0b1000 | (PSC_1 & 0b0111);
IMXRT_TMR_CH_t *regs = &(IMXRT_TMR1.CH[0]);
uint16_t reload;
uint16_t timer_ctl = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_ONCE | TMR_CTRL_LENGTH;

// Hardware Connections 
#define TRIGGER_OSCI  17
#define TRIGGER_IN    4
#define PIN_MOSI      11
#define PIN_SCK       13

#define JITTER_TEST

// Glitch Configuration
int iterations, pre_delay, rst_delay, delay1, delay2;
float pre_voltage, dst_voltage, rst_voltage;

String inString = "";

uint32_t data = 0x0;
uint32_t rst_data = 0x0;
uint32_t pre_data = 0x0;
uint32_t cmd_setps = 0x0;
bool armed = true;
uint32_t delay_us = 0;

// Function to do the glitch
FASTRUN void glitch() {
  //SPI.beginTransaction(sett25mhz);
  //SPI.setFrameSize(96);
  //tmr_ch->trigger(5.0);/*
  if (armed) {
    regs->CTRL = 0x0000;
    regs->LOAD = 0x0000;
    regs->COMP1 = reload;     // the delay value used by the timer
    regs->CMPLD1 = reload;
    regs->CNTR = 0x0000;

    regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
    regs->CSCTRL |= TMR_CSCTRL_TCF1EN;

    regs->CTRL = timer_ctl;

    //t1.trigger(delay_us);
  }

}

FASTRUN void INT_timer() {
  for (int i = 0; i < iterations; i++) {
    digitalWriteFast(TRIGGER_OSCI, HIGH);
    delayMicroseconds(1);
    digitalWriteFast(TRIGGER_OSCI, LOW);
  }
  Serial.println("Triggered!");
}

void setup() {
  // Configure pins
  pinMode(TRIGGER_OSCI, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(TRIGGER_IN, INPUT);

  // Keep SPI pins high
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(11, HIGH);
  digitalWrite(13, HIGH);


  delay(10);
  digitalWrite(TRIGGER_OSCI, LOW);

  // Configure Serial
  Serial.begin(115200);
  delay(1000);

  // for timer
  float t = delay_us * (150.0f / pscValue);
  reload = t > 0xFFFF ? 0xFFFF : (uint16_t) t;

  // Delays are counted by us, voltage need to be in step of 0.005V
  // Delay2 can be negative value, as the delay for voltage slew will be added
  Serial.println("Help: <times> <v1> <delay1> <v2> <delay2> <v3>");

  // Configure interrupt (Trigger from physical pin)
  attachInterrupt(digitalPinToInterrupt(TRIGGER_IN), glitch, FALLING);

  // Configure timer
  t1.begin(INT_timer);
  //tmr_ch = (TMRChannel*)t1.getChannel();

  iterations = 1;
  armed = true;

}

// Parse the string received from serial connection
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void loop() {
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    // if armed, wait for the trigger (Trigger form Teensy 4.0 USB)
    if (armed) {
      // This part of code need to be optimised
      if (inChar == '\n') {
        //delay(10);
        for (int i = 0; i < iterations; i++) {
          digitalWriteFast(TRIGGER_OSCI, HIGH);
          delayMicroseconds(1);
          digitalWriteFast(TRIGGER_OSCI, LOW);

        }
        Serial.println("Triggered!");
      }

    } else {
      // Wait for configuration and cmd from serial, no need to optimise
      Serial.println("cmd hadling code\n");
    }
  }


}
