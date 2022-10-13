/// conversions.h

#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#define times10(n)  ((n<<1)+(n<<3))

#include <Arduino.h>
void     BCDtoASC(uint16_t src, char *dst, uint8_t src_frac=0xFF);
uint16_t BCDtouint(uint16_t src, uint8_t src_frac=0xFF);
uint16_t ASCtoBCD(char *src, uint8_t *dst_frac=NULL);
uint16_t uinttoBCD(uint16_t src, uint8_t *dst_frac=NULL);

void     uinttoHex(uint32_t src, char *dst, uint8_t len=4);
uint16_t Hextouint16(char *src, uint8_t len=4);

//uint16_t ADFTrimInt(uint16_t f, uint8_t halfK=1); // Trim ADF freq to closer 0.5/1 kHz step
void     BCD_ADFtoASC5(uint16_t srcmain, uint16_t srcext, char *dst);
uint16_t BCD_ADFtouint(uint16_t srcmain, uint16_t srcext);
void     uinttoBCD_ADF(uint16_t src, uint16_t *dstmain, uint16_t *dstext);

#endif
