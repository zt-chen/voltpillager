// include the SPI library:
#include "SPI.h"
#include "genvidcmds.c"

// Include timer library
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;
OneShotTimer t1 (TMR1);
float pscValue = 1 << (PSC_1 & 0b0111);
uint32_t pscBits = 0b1000 | (PSC_1 & 0b0111);
IMXRT_TMR_CH_t* regs = &(IMXRT_TMR1.CH[0]);
uint16_t reload;
uint16_t timer_ctl = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_ONCE | TMR_CTRL_LENGTH;

// Hardware Connections 
#define TRIGGER_OSCI  17
#define TRIGGER_IN    4
#define PIN_MOSI      11
#define PIN_SCK       13

// Glitch Configuration
int iterations, pre_delay, rst_delay, delay1, delay2;
float pre_voltage, dst_voltage, rst_voltage;

String inString = "";

uint32_t data = 0x0;
uint32_t rst_data = 0x0;
uint32_t pre_data = 0x0;
uint32_t cmd_setps = 0x0;
SPISettings sett25mhz;
bool armed = false;
uint32_t delay_us = 0;

// Function to do the glitch
FASTRUN void glitch(){
  //SPI.beginTransaction(sett25mhz);
  //SPI.setFrameSize(96);
  //tmr_ch->trigger(5.0);/*
  if(armed){
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

FASTRUN void INT_timer(){
      //delayMicroseconds(delay_us);

      for( int i = 0; i < iterations; i++){
                    SPI.transfer32(pre_data);
                    delayMicroseconds(delay1); 
                    SPI.transfer32(data);
                    delayMicroseconds(delay2);

                    SPI.transfer32(rst_data);
                    delayMicroseconds(2); // Send 2 times just in case
                    SPI.transfer32(rst_data);
                    
                    digitalWriteFast(TRIGGER_OSCI, HIGH);
                    delayMicroseconds(1);
                    digitalWriteFast(TRIGGER_OSCI, LOW);

                    delay(50);

      }
      armed = false;
      Serial.println("Triggered!");
}
void setup() {
    // Configure pins
    pinMode(TRIGGER_OSCI, OUTPUT);
    pinMode(18, OUTPUT);
    pinMode(TRIGGER_IN, INPUT);

    delay(10);
    digitalWrite(TRIGGER_OSCI, LOW);
    
    // Configure SPI
    SPI.begin();
    sett25mhz = SPISettings(25000000, MSBFIRST, SPI_MODE2); // period 40ns
    delay(1000);
    SPI.beginTransaction(sett25mhz);
    delay(1000);
    SPI.setFrameSize(96);           // Large frame size

    // 96*40 = 3840ns
    // 96 bit frame size
    // 40bit pre clock + 24bit data + 32bit last frame
    // 1600ns + 960ns + 1280ns
    
    // Configure Serial
    Serial.begin(115200);
    delay(1000);

    // Delays are counted by us, voltage need to be in step of 0.005V
    // Delay2 can be negative value, as the delay for voltage slew will be added
    Serial.println("Help: <times> <v1> <delay1> <v2> <delay2> <v3>" );

    // Configure interrupt (Trigger from physical pin)
    attachInterrupt(digitalPinToInterrupt(TRIGGER_IN), glitch, FALLING);

    // Configure timer
    t1.begin(INT_timer);
      //tmr_ch = (TMRChannel*)t1.getChannel();

}

// Parse the string received from serial connection
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length()-1;

    for(int i=0; i<=maxIndex && found<=index; i++){
        if(data.charAt(i)==separator || i==maxIndex){
            found++;
            strIndex[0] = strIndex[1]+1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }

    return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void loop() {
    
    uint8_t counter = 0x65;

    while (Serial.available() > 0) {
        int inChar = Serial.read();
        // if armed, wait for the trigger (Trigger form Teensy 4.0 USB)
        if(armed){
          // This part of code need to be optimised 
            if(inChar == '\n'){
              SPI.beginTransaction(sett25mhz);
              //delay(10);
              SPI.setFrameSize(96);

              delayMicroseconds(delay_us);
        
              for( int i = 0; i < iterations; i++){
                    digitalWriteFast(TRIGGER_OSCI, HIGH);


                    SPI.transfer32(pre_data);
                    delayMicroseconds(delay1); 
                    SPI.transfer32(data);
                    delayMicroseconds(delay2);

                    SPI.transfer32(rst_data);
                    delayMicroseconds(2); // Send 2 times just in case
                    SPI.transfer32(rst_data);
                    digitalWriteFast(TRIGGER_OSCI, LOW);

                    delay(50);

                }
                Serial.println("Triggered!");
                armed = false;      // unarm after the glitch
            }

        }else{          
          // Wait for configuration and cmd from serial, no need to optimise

          // continue read until reach newline
            if (inChar != '\n') {
                inString += (char)inChar;
            }
            else {
              // read complete, parse cmds
                String serial_cmd = getValue(inString, ' ', 0);
                
                
                if(serial_cmd == "arm"){
                  // If the command is arm
                  armed = true;
                  Serial.println("Armed!");
                  // Max value for delay is around 1200 us
                }else if(serial_cmd == "delay"){
                  Serial.println(serial_cmd);
                  delay_us  = (getValue(inString, ' ', 1)).toInt();
                  // for timer
                  float t = delay_us * (150.0f / pscValue);
                  reload = t > 0xFFFF ? 0xFFFF : (uint16_t)t;

                  Serial.println("Delay Set!");
                }else{
                  // configuration of glitch
                  armed = false;       // arm and wait for trigger
                  iterations  = (getValue(inString, ' ', 0)).toInt();       // uS
                  pre_voltage = (getValue(inString, ' ', 1)).toFloat();     // 0.005 step
                  pre_delay   = (getValue(inString, ' ', 2)).toInt();       // uS
                  dst_voltage = (getValue(inString, ' ', 3)).toFloat();     // 0.005 step
                  rst_delay   = (getValue(inString, ' ', 4)).toInt();       // uS
                  rst_voltage = (getValue(inString, ' ', 5)).toFloat();     // 0.005 step
  
                  // Change time 1V->1.2V is 50uS
                  Serial.println(inString);
  
                  cmd_setps  = genCMD(CMD_SETPS, 0x0, 0x3); 
                  pre_data  = genSetVIDVal(pre_voltage, 0);
                  delay1    = pre_delay;
                  data      = genSetVIDVal(dst_voltage, 0);
                  // Calculated by SR of 10mV/uS, +40 delay for cmd sending(can be smaller)
                  delay2    = abs((pre_voltage - dst_voltage)*100) + rst_delay + 40;
                  rst_data  = genSetVIDVal(rst_voltage, 0);
                  // print the actual parameters after calculation. These actual parameters are reported in paper.
                  Serial.printf("(%d times) %1.3fV -> delay %d -> %1.3fV -> delay %d -> %1.3fV \n",
                          iterations, pre_voltage, delay1, dst_voltage, delay2, rst_voltage );
                  //Serial.println("Armed!");
                }
                inString = "";

            }
        }
    }


SPI.endTransaction();

}
