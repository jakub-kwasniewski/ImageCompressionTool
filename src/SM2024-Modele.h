#ifndef SM2024_MODELE_H_INCLUDED
#define SM2024_MODELE_H_INCLUDED

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

using namespace std;

struct YCbCr{
    Uint8 y;
    Uint8 cb;
    Uint8 cr;
};

int normalizacja(int liczba, int minN = 0, int maxN = 255);
YCbCr RGBToYCbCr(SDL_Color kolor);
void setYCbCr(int xx, int yy, Uint8 y, Uint8 cb, Uint8 cr);
YCbCr getYCbCr(int xx, int yy);

#endif
