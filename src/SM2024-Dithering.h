#ifndef SM2024_DITHERING_H_INCLUDED
#define SM2024_DITHERING_H_INCLUDED

#include <exception>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <fstream>
#include <iomanip>
#include <windows.h>

#include "SM2024-Funkcje.h"

extern int tablicaBayera4[4][4];

extern float zaktualizowanaTablicaBayera4[4][4];

void zaktualizujTabliceBayera4();
void zaktualizujTabliceBayera4G();
void zaktualizujTabliceBayera4Szarosc();
Uint16 calculateCellBayerRGB565(int xx, int yy, Uint16 kolor);
void ditheringBayer(void** arr, tryby t, Uint8 color);

#endif
