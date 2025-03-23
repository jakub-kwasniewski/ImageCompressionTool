#ifndef SM2024_KOMPRESJE_H_INCLUDED
#define SM2024_KOMPRESJE_H_INCLUDED

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
#include <vector>

#include "SM2024-Funkcje.h"

#define M_PI 3.14159265358979323846

void kompresjaLZW(std::ofstream &wyjscie, void** arr, tryby t, int color);
void odczytLZW(std::ifstream &wejscie, void** arr, tryby t, int n, int m, int color);

const int rozmiarBloku = 8;

struct macierz{
    double dct[rozmiarBloku][rozmiarBloku];
    Uint8 dane[rozmiarBloku][rozmiarBloku];
    Uint16 maxDCT;
};

void zapisDCT(std::ofstream &wyjscie, void** arr, tryby t, int color);
void odczytDCT(std::ifstream &wejscie, void** arr, tryby t, int n, int m, int color);
#endif
