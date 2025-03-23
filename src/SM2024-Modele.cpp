#include "SM2024-Modele.h"
#include "SM2024-Zmienne.h"
#include "SM2024-Pliki.h"
#include "SM2024-Kompresje.h"
#include "SM2024-Funkcje.h"

int normalizacja(int liczba, int minN, int maxN) {
    if(liczba < minN){
        return minN;
    }else if(liczba > maxN){
        return maxN;
    }
    return liczba;
}

YCbCr RGBToYCbCr(SDL_Color kolor) {
    float y, cb, cr;
    y  =        (0.299 * kolor.r)        + (0.587 * kolor.g)      + (0.114 * kolor.b);
    cb = 128    - (0.168736 * kolor.r)   - (0.331264 * kolor.g)   + (0.5 * kolor.b);
    cr = 128    + (0.5 * kolor.r)        - (0.418688 * kolor.g)   - (0.081312 * kolor.b);

    return { y, cb, cr };
}

void setYCbCr(int xx, int yy, Uint8 y, Uint8 cb, Uint8 cr){
    float r, g, b;
    r = y + 1.402                               * (cr - 128);
    g = y - 0.344136    * (cb - 128) - 0.714136 * (cr - 128);
    b = y + 1.772       * (cb - 128);

    setPixel(xx, yy, normalizacja(r), normalizacja(g), normalizacja(b));
}

YCbCr getYCbCr(int xx, int yy){
    SDL_Color temp = getPixel(xx, yy);

    float y, cb, cr;
    y  =        (0.299 * temp.r)        + (0.587 * temp.g)      + (0.114 * temp.b);
    cb = 128    - (0.168736 * temp.r)   - (0.331264 * temp.g)   + (0.5 * temp.b);
    cr = 128    + (0.5 * temp.r)        - (0.418688 * temp.g)   - (0.081312 * temp.b);

    return {y, cb, cr};
}
