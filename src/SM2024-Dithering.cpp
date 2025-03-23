#include "SM2024-Dithering.h"
#include "SM2024-Funkcje.h"
#include "SM2024-Zmienne.h"
#include "SM2024-Pliki.h"
#include "SM2024-Kompresje.h"
#include "SM2024-Modele.h"

int tablicaBayera4[4][4] = { {6,14,8,16},
                             {10,2,12,4},
                             {7,15,5,13},
                             {11,3,9,1} };

Uint16 policzKomorkeBayerRGB565(int xx, int yy, Uint16 kolor) {
    Uint8 r, g, b;

    r = kolor >> 11;
    g = (0b0000011111100000 & kolor) >> 5;
    b = 0b0000000000011111 & kolor;

    int tablicaRB = tablicaBayera4[yy % 4][xx % 4] * 31 / 16;
    int tablicaG = tablicaBayera4[yy % 4][xx % 4] * 63 / 16;

    r = r > tablicaRB ? 31 : 0;
    g = g > tablicaG ? 63 : 0;
    b = b > tablicaRB ? 31 : 0;

    return (r << 11) | (g << 5) | b;
}

Uint16 policzKomorkeBayerRGB565Szarosc(int xx, int yy, Uint16 kolor) {
    int tablica = tablicaBayera4[yy % 4][xx % 4] * 65535 / 16;

    return (kolor > tablica) ? 65535 : 0;
}

SDL_Color policzKomorkeBayerRGB888(int xx, int yy, SDL_Color kolor) {
    int r, g, b;

    r = kolor.r;
    g = kolor.g;
    b = kolor.b;

    int tablica = tablicaBayera4[yy % 4][xx % 4] * 255 / 16;

    r = r > tablica ? 255 : 0;
    g = g > tablica ? 255 : 0;
    b = b > tablica ? 255 : 0;

    return { r, g, b };
}

Uint8 policzKomorkeBayerRGB888Szarosc(int xx, int yy, SDL_Color kolor) {
    int tablica = tablicaBayera4[yy % 4][xx % 4] * 255 / 16;

    return (kolor.r > tablica) ? 255 : 0;
}

void ditheringBayer(void** arr, tryby t, Uint8 color) {
    switch (t) {
    case _RGB565: {
        Uint16** obraz = reinterpret_cast<Uint16**>(arr);

        for (int y = 0; y < wysokosc / 2; y++) {
            for (int x = 0; x < szerokosc / 2; x++) {
                if (color == 0)
                    obraz[y][x] = policzKomorkeBayerRGB565(x, y, obraz[y][x]);
                else if (color == 1)
                    obraz[y][x] = policzKomorkeBayerRGB565Szarosc(x, y, obraz[y][x]);
            }
        }

        break;
    }
    case _RGB888:
        SDL_Color** obraz = reinterpret_cast<SDL_Color**>(arr);

        for (int y = 0; y < wysokosc / 2; y++) {
            for (int x = 0; x < szerokosc / 2; x++) {
                if (color == 0)
                    obraz[y][x] = policzKomorkeBayerRGB888(x, y, obraz[y][x]);
                else if (color == 1) {
                    obraz[y][x].r = policzKomorkeBayerRGB888Szarosc(x, y, obraz[y][x]);
                    obraz[y][x].g = obraz[y][x].r;
                    obraz[y][x].b = obraz[y][x].r;
                }
            }
        }

        break;
    };
}
