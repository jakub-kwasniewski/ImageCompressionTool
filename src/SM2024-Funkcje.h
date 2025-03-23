// podstawowe funkcje
#ifndef SM2024_FUNKCJE_H_INCLUDED
#define SM2024_FUNKCJE_H_INCLUDED

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

#include "SM2024-Modele.h"
#include "SM2024-Zmienne.h"

using namespace std;

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);

SDL_Color getPixel(int x, int y);

void czyscEkran(Uint8 R, Uint8 G, Uint8 B);

void setPixelSurface(int x, int y, Uint8 R, Uint8 G, Uint8 B);

SDL_Color getPixelSurface(int x, int y, SDL_Surface *surface);

string ToString(tryby t);

void setRGB565(int xx, int yy, Uint8 r, Uint8 g, Uint8 b);
void setRGB565(int xx, int yy, Uint16 rgb565);
SDL_Color getRGB565(int xx, int yy);
Uint16 getRGB565_(int xx, int yy);
void predykcjaTyp1(void** arr, tryby t, int color);
void predykcjaTyp1Powrot(void** arr, tryby t, int n, int m, int color = 0);
void probkowanie(YCbCr** obraz);

#endif // SM2024_FUNKCJE_H_INCLUDED
