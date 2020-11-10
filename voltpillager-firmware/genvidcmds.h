#ifndef GENVIDCMDS_H
#define GENVIDCMDS_H

#define CMD_EXTENDED 0x00
#define CMD_SETVID_F 0x01
#define CMD_SETVID_S 0x02
#define CMD_SETVID_D 0x03
#define CMD_SETPS    0x04
#define CMD_SETREGADR  0x05
#define CMD_SETREGDAT  0x06


#define VB640  0x80          // 640mV
#define VB320  0x40          // 320mV 
#define VB160  0x20          // 160mV
#define VB80  0x10           // 80mv
#define VB40 0x08            // 40mV
#define VB20 0x04            // 20mV
#define VB10 0x02            // 10mV
#define VB5 0x01             // 5mV
uint32_t genSetVIDVal(float voltage, uint8_t addr);
uint32_t genCMD(uint8_t cmd, uint8_t addr, uint8_t data);
uint32_t genSetVID(uint8_t vid);
#endif /*GENVIDCMDS_H */
