#include <stdio.h>
#include <stdint.h>
#include "genvidcmds.h"

// inline uint8_t genParity(uint32_t data);
// inline uint32_t genCMD(uint8_t cmd, uint8_t addr, uint8_t data);

uint8_t genParity(uint32_t data){

    data = data & 0x3FFFF;   // shift the padding 

    // get the parity bit
    uint8_t parity = 0;
    while (data != 0){
        parity ^= data & 0x1;
        data = data >> 1;
    }
    return parity;
}
// 1982
// Start    3 bits  010
// ADDR     4 bits   
// CMD      5 bits 
// DATA     8 bits
// Parity   1 bits
// End      3 bits  011
// (24 bits in total)
// Generate data with MSB first, the first few bits are filled with 1 
uint32_t genCMD(uint8_t cmd, uint8_t addr, uint8_t data){

    uint32_t result = 0;           
    result = (result | 0xff) << 3;  // Fill the first few bits with 1
    result = (result | 0x2) << 4;   // Start 010
    result = (result | addr) << 5;
    result = (result | cmd) << 8;
    result = (result | data) << 1;
    uint8_t parity = genParity(result);
    result = (result | parity) << 3;
    result = result | 0x3;          // End 011

    // Test 
    result = (result << 2) | 0x3;

    //uint32_t result = 0x40000300;
    //result = result | addr << 25 | cmd << 20 | data << 12 ;
    //result = result | parity << 11;

    return result;
} 

// set VID Command issue to all voltage regulators
uint32_t genSetVID(uint8_t vid){
    return genCMD(CMD_SETVID_S, 0xF, vid);
}

uint32_t genSetVIDVal(float voltage, uint8_t addr){
    if(voltage == 0){
        return genCMD(CMD_SETVID_F,addr, 0x00 );
    }
    else if(voltage <= 0.25){   // minimum voltage 0.25V
        return genCMD(CMD_SETVID_F,addr, 0x01 ); // 0.25V
    }else if(voltage >=1.52 ){
        return genCMD(CMD_SETVID_F,addr, 0xFF);
    }
    uint8_t vid = (uint8_t)((voltage-0.245)/0.005);
    return genCMD(CMD_SETVID_F, addr, vid ); 
  }
