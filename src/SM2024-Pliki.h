// funkcje do operacji na plikach
#ifndef SM2024_PLIKI_H_INCLUDED
#define SM2024_PLIKI_H_INCLUDED

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

using namespace std;

std::string getFileNameDialog();

void zapis(std::string path, tryby t, int color, int dither, int pred, int compress);

void odczyt(std::string path);

void ladujBMP(string nazwa, int x, int y);

void zapiszZ25();

#endif // SM2024_PLIKI_H_INCLUDED
