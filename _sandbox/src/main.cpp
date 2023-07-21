// =======================================================================
// @file        main.cpp
//
// @project     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-07-21
// @modifiedby  GiorgioCC - 2023-07-21 23:49
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#include <Arduino.h>
#include <stdio.h>

#include "Button_all.h"
#include "ButtonManager.h"
#include "memPool.h"

#define  MEM_POOL_SIZE  (16*sizeof(ButtonAdv))


memPool<MEM_POOL_SIZE>     pool;
char sbuf[64];

//Button      Btn;
volatile ButtonAdv   BAdv;
volatile ButtonAna   Bana;
volatile ButtonBas   BBas;
volatile ButtonEnc   BEnc;
volatile ButtonGrp   BGrp;

ButtonManager   BtnMgr;

constexpr uint8_t S_Btn  = sizeof(Button);
constexpr uint8_t S_BAdv = sizeof(ButtonAdv);
constexpr uint8_t S_Bana = sizeof(ButtonAna);
constexpr uint8_t S_BBas = sizeof(ButtonBas);
constexpr uint8_t S_BEnc = sizeof(ButtonEnc);
constexpr uint8_t S_BGrp = sizeof(ButtonGrp);

constexpr uint8_t S_BtnMgr  = sizeof(ButtonManager);

void report(void)
{
    sprintf(sbuf, "Size of %s: %d bytes\n\r", "Button   ", S_Btn );   Serial.println(sbuf);
    sprintf(sbuf, "Size of %s: %d bytes\n\r", "ButtonAdv", S_BAdv);   Serial.println(sbuf); 
    sprintf(sbuf, "Size of %s: %d bytes\n\r", "ButtonAna", S_Bana);   Serial.println(sbuf);
    sprintf(sbuf, "Size of %s: %d bytes\n\r", "ButtonBas", S_BBas);   Serial.println(sbuf);
    sprintf(sbuf, "Size of %s: %d bytes\n\r", "ButtonEnc", S_BEnc);   Serial.println(sbuf);
    sprintf(sbuf, "Size of %s: %d bytes\n\r", "ButtonGrp", S_BGrp);   Serial.println(sbuf);
    sprintf(sbuf, "Size of %s: %d bytes\n\r", "ButtonMgr", S_BtnMgr); Serial.println(sbuf);
    Serial.println("-------------------------\n\r");
}


void setup() {

    Serial.begin(9600);
    Serial.println("Ready.\n");
    
}

void loop() {
    report();
    delay(3000);
}

