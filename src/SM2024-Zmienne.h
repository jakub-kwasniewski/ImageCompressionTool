// zmienne globalne
#ifndef SM2024_ZMIENNE_H_INCLUDED
#define SM2024_ZMIENNE_H_INCLUDED

#include <SDL2/SDL.h>
#include <vector>

#define szerokosc 640
#define wysokosc 400

#define tytul "SM2024 - Projekt - Zespol 25"

extern SDL_Window *window;
extern SDL_Surface *screen;

enum tryby {
    _RGB565,
    _RGB888,
    _YCbCr888
};

struct slowo {
    Uint16 kod = 0;
    Uint8 dlugosc = 0;
    Uint8 element[4096];
    bool wSlowniku = false;
};

extern int rozmiarSlownika;
extern slowo slownik[65535];

#endif // SM2024_ZMIENNE_H_INCLUDED
