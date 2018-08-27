/*******************************************************************************
   main: main for voltmeter with vga
   2015 Brno University of Technology,
                      Faculty of Information Technology
   Author: Michal Gabonay (xgabon00)           
*******************************************************************************/

#include <fitkitlib.h>
#include <lcd/display.h>
#include <stdio.h>;
#include "adc.h"
#include <stdlib.h>
#include <string.h>

// Bazova Adresa komponenty DISPLAY
#define BASE_ADDR_VRAM 0x8000

//#define SPI_FPGA_ENABLE_WRITE 0x01
#define SPI_FPGA_ENABLE_READ  0x02

//sloupcu
#define VGA_COLUMNS 80
//radku
#define VGA_ROWS    60
//pocet znaku na radek
#define VGA_STRIDE  128

int paused = 0;
int interval = 50;

int x=0;

unsigned short vga_pos = 0;

/*******************************************************************************
 * Vypis uzivatelske napovedy (funkce se vola pri vykonavani prikazu "help")
*******************************************************************************/
void print_user_help(void)
{
  term_send_str_crlf(" STOP Pozastavi sledovani napeti");
  term_send_str_crlf(" START Obnovi sledovani napeti");
  term_send_str_crlf(" SLOW Zpomali sledovani");
  term_send_str_crlf(" FAST Zrychli sledovani");
}

//Vymazani video RAM
void VGA_Clear(void) {
  unsigned short i, j;
  unsigned char c;        
  vga_pos = 0;
  for (i=0;i<VGA_ROWS;i++) {
     for (j = 0; j<VGA_COLUMNS; j++) {
        c = 0;
        // Odeslani dat do FPGA (2 byty adresy, 1 byte dat)
        FPGA_SPI_RW_AN_DN(SPI_FPGA_ENABLE_WRITE, BASE_ADDR_VRAM/*base*/ + i*VGA_STRIDE + j, &c, 2, 1);
     }
  }
}

//Zapsani hodnoty na souradnici X,Y
void VGA_SetPixelXY(unsigned char x, unsigned char y, char val) {
  unsigned short vga_pos = y*VGA_STRIDE + x;
  FPGA_SPI_RW_AN_DN(SPI_FPGA_ENABLE_WRITE, BASE_ADDR_VRAM /*base*/ + vga_pos, &val, 2, 1);
}

//Inicializace VRAM
void VRAM_Init(void) {
  short i;
  VGA_Clear();
  
  //bily ramecek
  for (i=0;i<VGA_COLUMNS;i++) {
      VGA_SetPixelXY(i,0, 1);
      VGA_SetPixelXY(i,VGA_ROWS-1, 1);
  }
  for (i=0;i<VGA_ROWS;i++) {
      VGA_SetPixelXY(0,i, 1);
      VGA_SetPixelXY(VGA_COLUMNS-1,i, 1);
  }
}

/*******************************************************************************
 * Dekodovani a vykonani uzivatelskych prikazu
*******************************************************************************/
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
  if (strcmp5(cmd_ucase, "STOP"))
  {
    paused = 1;
    return (USER_COMMAND);
  }
  else if (strcmp5(cmd_ucase, "START"))
  {
    paused = 0;
    return (USER_COMMAND);
  }
  else if (strcmp4(cmd_ucase, "SLOW"))
  {
    interval += 30;
    if(interval > 500){
      interval = 0;
      term_send_str_crlf("Toto je najmensia mozna rychlost!");
    }
    return (USER_COMMAND);
  }
  else if (strcmp4(cmd_ucase, "FAST"))
  {
    interval -= 30;
    if(interval <= 0){
      interval = 0;
      term_send_str_crlf("Toto je najvacsia mozna rychlost!");
    }
    return (USER_COMMAND);
  }
  else
  {
    return (CMD_UNKNOWN);
  }
}

/*******************************************************************************
 * Inicializace periferii/komponent po naprogramovani FPGA
*******************************************************************************/

void fpga_initialized()
{
  LCD_init();

  VRAM_Init();
}

/*******************************************************************************
 * Hlavni funkce
*******************************************************************************/
int main(void)
{
  unsigned int cnt = 0;
  char string[100];
  unsigned int value = 0;

  initialize_hardware();

  while (1)
  {
    delay_ms(1);
    cnt++;
    if (cnt >= interval)
    {
      cnt = 0;

      if(!paused){
        adcInit();
        value = adcValue();

        sprintf(string, "Voltage: %u mV", value);
        LCD_clear();
        LCD_append_string(string);

        x++;
        if(x>=79){
          x=1;
          VRAM_Init();
        } 
        VGA_SetPixelXY(x, 56-(value/45)+1, 3);
      }
      terminal_idle();              
    } 
  } 

}
