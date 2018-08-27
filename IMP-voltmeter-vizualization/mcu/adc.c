/**
 * Michal Gabonay (xgabon00)
 */

#include <fitkitlib.h>
#include <msp430x16x.h>
#include <signal.h>
#include <stdbool.h>
#include "adc.h"

unsigned int value = 0; //vysledna hodnota

/**
 * Poupraveny zdroj:
 *  D169 Demo - ADC12 / Integrated Temperature Sensor (D169_adc12_01.c)
 *  http://sandiaproject.googlecode.com/svn/trunk/Docs/CDs/Utah%202008-09/MSP430/slac027a/D169_adc12_01.c
 *  Texas Instruments Inc.
 */
void adcInit(){
  
  ADC12CTL1 = SHS_1 + SHP + CONSEQ_2;  // TA trig., rpt conv.
  ADC12MCTL0 = SREF_1 + INCH_0;        // Vref+, Channel A0
  ADC12IE = 0x01;                      // Enable ADC12IFG.0
  
  ADC12CTL0 = SHT0_8 +
              REF2_5V +  // ADC12 Ref 0:1.5V / 1:2.5V
              REFON +    // ADC12 Reference on
              ADC12ON +  // ADC12 On/enable
              ENC;       // ADC12 Enable Conversion
  
  TACCTL1 = OUTMOD_4;       // Toggle on EQU1 (TAR = 0)
  TACTL = TASSEL_2 + MC_2;  // SMCLK, cont-mode
  
  _EINT(); // enable interrupt
}

interrupt (ADC12_VECTOR) adcIntHandler(){
  if(ADC12IV == 6){
    value=ADC12MEM0;
  }
}

inline bool isHigh(){
  return ADC12CTL0 & REF2_5V;
}

unsigned int adcValue(){
  
  bool high = isHigh();
  unsigned int output;
  
  if(high){
    output = value * 5 / 8; // * 2.5V / 4 (4000 => 2500mV)
  }else{
    output = value * 3 / 8; // * 1.5V / 4 (4000 => 1500mV)
  }
  
  // prepnutie na referencne napatie 2,5V
  if(!high && value >= 4095){
    ADC12CTL0 &= ~ENC; // disable ENC
    ADC12CTL0 |= REF2_5V; // enable bit
    ADC12CTL0 |= ENC; // enable ENC
    term_send_str_crlf("High ref");
  }
  
  // vypnutie referencneho napatita 2,5V (1,5V)
  if(high && value <= 2047){
    ADC12CTL0 &= ~ENC;      // disable ENC
    ADC12CTL0 &= ~REF2_5V;  // disable REF2_5V
    ADC12CTL0 |= ENC;       // enable ENC
    term_send_str_crlf("Low ref");
  }
  
  return output;
}

