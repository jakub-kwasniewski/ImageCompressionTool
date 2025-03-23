#include "SM2024-Funkcje.h"
#include "SM2024-Zmienne.h"
#include "SM2024-Kompresje.h"
#include "SM2024-Modele.h"

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B) {
    if ((x >= 0) && (x < szerokosc) && (y >= 0) && (y < wysokosc)) {
        /* Zamieniamy poszczególne składowe koloru na format koloru piksela */
        Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

        /* Pobieramy informację ile bajtów zajmuje jeden piksel */
        int bpp = screen->format->BytesPerPixel;

        /* Obliczamy adres piksela */
        Uint8* p1 = (Uint8*)screen->pixels + (y * 2) * screen->pitch + (x * 2) * bpp;
        Uint8* p2 = (Uint8*)screen->pixels + (y * 2 + 1) * screen->pitch + (x * 2) * bpp;
        Uint8* p3 = (Uint8*)screen->pixels + (y * 2) * screen->pitch + (x * 2 + 1) * bpp;
        Uint8* p4 = (Uint8*)screen->pixels + (y * 2 + 1) * screen->pitch + (x * 2 + 1) * bpp;

        /* Ustawiamy wartość piksela, w zależnoœci od formatu powierzchni*/
        switch (bpp) {
        case 1: //8-bit
            *p1 = pixel;
            *p2 = pixel;
            *p3 = pixel;
            *p4 = pixel;
            break;

        case 2: //16-bit
            *(Uint16*)p1 = pixel;
            *(Uint16*)p2 = pixel;
            *(Uint16*)p3 = pixel;
            *(Uint16*)p4 = pixel;
            break;

        case 3: //24-bit
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p1[0] = (pixel >> 16) & 0xff;
                p1[1] = (pixel >> 8) & 0xff;
                p1[2] = pixel & 0xff;
                p2[0] = (pixel >> 16) & 0xff;
                p2[1] = (pixel >> 8) & 0xff;
                p2[2] = pixel & 0xff;
                p3[0] = (pixel >> 16) & 0xff;
                p3[1] = (pixel >> 8) & 0xff;
                p3[2] = pixel & 0xff;
                p4[0] = (pixel >> 16) & 0xff;
                p4[1] = (pixel >> 8) & 0xff;
                p4[2] = pixel & 0xff;
            }
            else {
                p1[0] = pixel & 0xff;
                p1[1] = (pixel >> 8) & 0xff;
                p1[2] = (pixel >> 16) & 0xff;
                p2[0] = pixel & 0xff;
                p2[1] = (pixel >> 8) & 0xff;
                p2[2] = (pixel >> 16) & 0xff;
                p3[0] = pixel & 0xff;
                p3[1] = (pixel >> 8) & 0xff;
                p3[2] = (pixel >> 16) & 0xff;
                p4[0] = pixel & 0xff;
                p4[1] = (pixel >> 8) & 0xff;
                p4[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32*)p1 = pixel;
            *(Uint32*)p2 = pixel;
            *(Uint32*)p3 = pixel;
            *(Uint32*)p4 = pixel;
            break;

        }
    }
}

SDL_Color getPixel(int x, int y) {
    SDL_Color color;
    Uint32 col = 0;
    if ((x >= 0) && (x < szerokosc) && (y >= 0) && (y < wysokosc)) {
        //określamy pozycję
        char* pPosition = (char*)screen->pixels;

        //przesunięcie względem y
        pPosition += (screen->pitch * y * 2);

        //przesunięcie względem x
        pPosition += (screen->format->BytesPerPixel * x * 2);

        //kopiujemy dane piksela
        memcpy(&col, pPosition, screen->format->BytesPerPixel);

        //konwertujemy kolor
        SDL_GetRGB(col, screen->format, &color.r, &color.g, &color.b);
    }
    return (color);
}

void czyscEkran(Uint8 R, Uint8 G, Uint8 B) {
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, R, G, B));
    SDL_UpdateWindowSurface(window);
}

void setPixelSurface(int x, int y, Uint8 R, Uint8 G, Uint8 B) {
    if ((x >= 0) && (x < szerokosc * 2) && (y >= 0) && (y < wysokosc * 2)) {
        /* Zamieniamy poszczególne składowe koloru na format koloru piksela */
        Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

        /* Pobieramy informację ile bajtów zajmuje jeden piksel */
        int bpp = screen->format->BytesPerPixel;

        /* Obliczamy adres piksela */
        Uint8* p = (Uint8*)screen->pixels + y * screen->pitch + x * bpp;

        /* Ustawiamy wartość piksela, w zależności od formatu powierzchni*/
        switch (bpp) {
        case 1: //8-bit
            *p = pixel;
            break;

        case 2: //16-bit
            *(Uint16*)p = pixel;
            break;

        case 3: //24-bit
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32*)p = pixel;
            break;
        }
    }
}

SDL_Color getPixelSurface(int x, int y, SDL_Surface* surface) {
    SDL_Color color;
    Uint32 col = 0;
    if ((x >= 0) && (x < szerokosc) && (y >= 0) && (y < wysokosc)) {
        //określamy pozycję
        char* pPosition = (char*)surface->pixels;

        //przesunięcie względem y
        pPosition += (surface->pitch * y);

        //przesunięcie względem x
        pPosition += (surface->format->BytesPerPixel * x);

        //kopiujemy dane piksela
        memcpy(&col, pPosition, surface->format->BytesPerPixel);

        //konwertujemy kolor
        SDL_GetRGB(col, surface->format, &color.r, &color.g, &color.b);
    }
    return (color);
}

string ToString(tryby t) {
    switch (t) {
/*    case _paletaNarzucona:                  return "Paleta Narzucona";
    case _paletaNarzuconaDither:            return "Paleta Narzucona + Dithering";
    case _skalaSzarosciNarzuconej:          return "Skala Szarosci Narzuconej";
    case _skalaSzarosciNarzuconejDither:    return "Skala Szarosci Narzuconej + Dithering";
    case _skalaSzarosciDedykowanej:         return "Skala Szarosci Dedykowanej";
    case _skalaSzarosciDedykowanejDither:   return "Skala Szarosci Dedykowanej + Dithering";
    case _paletaWykryta:                    return "Paleta Wykryta";
    case _paletaDedykowana:                 return "Paleta Dedykowana";
    case _paletaDedykowanaDither:           return "Paleta Dedykowana + Dithering";
    case _RGB565Bayer:                      return "RGB565 + Dithering Bayera";
    case _SzaroscBayer:                     return "Skala Szarosci + Dithering Bayera";*/
    case _RGB565:                           return "RGB565";
    case _RGB888:                           return "RGB888";
    case _YCbCr888:                         return "YCbCr888";
    }
}

void setRGB565(int xx, int yy, Uint8 r, Uint8 g, Uint8 b){
    setPixel(xx, yy, r * 255/31, g * 255/63, b * 255/31);
}

void setRGB565(int xx, int yy, Uint16 rgb565){
    Uint8 r, g, b;
    r = (rgb565 & 0xF800) >> 11;
    g = (rgb565 & 0x7E3) >> 5;
    b = (rgb565 & 0x1F);

    r = r * 255 / 31;
    g = g * 255 / 63;
    b = b * 255 / 31;

    setPixel(xx, yy, r, g, b);
}

SDL_Color getRGB565(int xx, int yy){
    SDL_Color temp = getPixel(xx, yy);
    temp.r = temp.r * 31/255;
    temp.g = temp.g * 63/255;
    temp.b = temp.b * 31/255;

    return temp;
}

Uint16 getRGB565_(int xx, int yy){
    SDL_Color temp = getPixel(xx, yy);

    Uint8 r, g, b;
    r = temp.r * 31 / 255;
    g = temp.g * 63 / 255;
    b = temp.b * 31 / 255;

    Uint16 rgb565 = (r << 11) | (g << 5) | b;

    return rgb565;
}

Uint16 predykcjaKomorkaRGB565(Uint16 kolor, Uint16 ostatniKolor) {
    Uint8 r, g, b;
    Uint8 lastR, lastG, lastB;

    r = kolor >> 11;
    g = (0b0000011111100000 & kolor) >> 5;
    b = 0b0000000000011111 & kolor;

    lastR = ostatniKolor >> 11;
    lastG = (0b0000011111100000 & ostatniKolor) >> 5;
    lastB = 0b0000000000011111 & ostatniKolor;

    r = (r - lastR) - 32 * std::floor((double)(r - lastR) / 32);
    g = (g - lastG) - 64 * std::floor((double)(g - lastG) / 64);
    b = (b - lastB) - 32 * std::floor((double)(b - lastB) / 32);

    return (r << 11) | (g << 5) | b;
}

Uint16 predykcjaKomorkaRGB565Szarosc(Uint16 kolor, Uint16 ostatniKolor) {
    return kolor - ostatniKolor;
}

SDL_Color predykcjaKomorkaRGB888(SDL_Color kolor, SDL_Color ostatniKolor) {
    Uint8 r, g, b;
    Uint8 lastR, lastG, lastB;

    r = kolor.r;
    g = kolor.g;
    b = kolor.b;

    lastR = ostatniKolor.r;
    lastG = ostatniKolor.g;
    lastB = ostatniKolor.b;

    r = r - lastR;
    g = g - lastG;
    b = b - lastB;

    return { r, g, b };
}

Uint8 predykcjaKomorkaRGB888Szarosc(SDL_Color kolor, SDL_Color ostatniKolor) {
    return kolor.r - ostatniKolor.r;
}

YCbCr predykcjaKomorkaYCbCr888(YCbCr kolor, YCbCr ostatniKolor) {
    Uint8 y, cb, cr;
    Uint8 lastY, lastCb, lastCr;

    y = kolor.y;
    cb = kolor.cb;
    cr = kolor.cr;

    lastY = ostatniKolor.y;
    lastCb = ostatniKolor.cb;
    lastCr = ostatniKolor.cr;

    y = y - lastY;
    cb = cb - lastCb;
    cr = cr - lastCr;

    return { y, cb, cr };
}

Uint8 predykcjaKomorkaYCbCr888Szarosc(YCbCr kolor, YCbCr ostatniKolor) {
    return kolor.y - ostatniKolor.y;
}

void predykcjaTyp1(void** arr, tryby t, int color) {
    switch (t) {
    case _RGB565: {
        Uint16** obraz = reinterpret_cast<Uint16**>(arr);
        Uint16 ostatniKolor;

        for (int y = 0; y < wysokosc / 2; y++) {
            ostatniKolor = obraz[y][0];

            for (int x = 1; x < szerokosc / 2; x++) {
                Uint16 temp = obraz[y][x];

                if (color == 0)
                    obraz[y][x] = predykcjaKomorkaRGB565(obraz[y][x], ostatniKolor);
                else if (color == 1)
                    obraz[y][x] = predykcjaKomorkaRGB565Szarosc(obraz[y][x], ostatniKolor);

                ostatniKolor = temp;
            }
        }

        break;
    }
    case _RGB888: {
        SDL_Color** obraz = reinterpret_cast<SDL_Color**>(arr);
        SDL_Color ostatniKolor;

        for (int y = 0; y < wysokosc / 2; y++) {
            ostatniKolor = obraz[y][0];

            for (int x = 1; x < szerokosc / 2; x++) {
                SDL_Color temp = obraz[y][x];

                if (color == 0)
                    obraz[y][x] = predykcjaKomorkaRGB888(obraz[y][x], ostatniKolor);
                else if (color == 1)
                    obraz[y][x].r = predykcjaKomorkaRGB888Szarosc(obraz[y][x], ostatniKolor);

                ostatniKolor = temp;
            }
        }

        break;
    }
    case _YCbCr888: {
        YCbCr** obraz = reinterpret_cast<YCbCr**>(arr);
        YCbCr ostatniKolor;

        for (int y = 0; y < wysokosc / 2; y++) {
            ostatniKolor = obraz[y][0];

            for (int x = 1; x < szerokosc / 2; x++) {
                YCbCr temp = obraz[y][x];

                if (color == 0)
                    obraz[y][x] = predykcjaKomorkaYCbCr888(obraz[y][x], ostatniKolor);
                else if (color == 1)
                    obraz[y][x].y = predykcjaKomorkaYCbCr888Szarosc(obraz[y][x], ostatniKolor);

                ostatniKolor = temp;
            }
        }

        break;
    }
    };
}

Uint16 predykcjaPowrotKomorkaRGB565(Uint16 kolor, Uint16 ostatniKolor) {
    Uint8 r, g, b;
    Uint8 lastR, lastG, lastB;

    r = (kolor >> 11) & 0x1F;
    g = (kolor >> 5) & 0x3F;
    b = kolor & 0x1F;

    lastR = (ostatniKolor >> 11) & 0x1F;
    lastG = (ostatniKolor >> 5) & 0x3F;
    lastB = ostatniKolor & 0x1F;

    r = (r + lastR) % 32;
    g = (g + lastG) % 64;
    b = (b + lastB) % 32;

    return (r << 11) | (g << 5) | b;
}

Uint16 predykcjaPowrotKomorkaRGB565Szarosc(Uint16 kolor, Uint16 ostatniKolor) {
    return kolor + ostatniKolor;
}

void predykcjaTyp1Powrot(void** arr, tryby t, int n, int m, int color) {
    switch (t) {
    case _RGB565: {
        Uint16** obraz = reinterpret_cast<Uint16**>(arr);

        for (int y = 0; y < n; y++) {
            for (int x = 1; x < m; x++) {
                if (color == 0)
                    obraz[y][x] = predykcjaPowrotKomorkaRGB565(obraz[y][x], obraz[y][x - 1]);
                else if (color == 1)
                    obraz[y][x] = predykcjaPowrotKomorkaRGB565Szarosc(obraz[y][x], obraz[y][x - 1]);
            }
        }

        break;
    }
    case _RGB888: {
        SDL_Color** obraz = reinterpret_cast<SDL_Color**>(arr);

        for(int i = 0; i < n; i++){
            for (int j = 1; j < m; j++){
                obraz[i][j].r = obraz[i][j - 1].r + obraz[i][j].r;
                obraz[i][j].g = obraz[i][j - 1].g + obraz[i][j].g;
                obraz[i][j].b = obraz[i][j - 1].b + obraz[i][j].b;
            }
        }

        break;
    }
    case _YCbCr888:
        YCbCr** obraz = reinterpret_cast<YCbCr**>(arr);

        for(int i = 0; i < n; i++){
            for (int j = 1; j < m; j++){
                obraz[i][j].y = obraz[i][j - 1].y + obraz[i][j].y;
                obraz[i][j].cb = obraz[i][j - 1].cb + obraz[i][j].cb;
                obraz[i][j].cr = obraz[i][j - 1].cr + obraz[i][j].cr;
            }
        }

        break;
    };
}

void probkowanieCb(YCbCr** obraz, int xx, int yy) {
    Uint8 pixel1, pixel2, pixel3, pixel4;
    pixel1 = obraz[yy][xx].cr;
    pixel2 = obraz[yy + 1][xx].cr;
    pixel3 = obraz[yy][xx + 1].cr;
    pixel4 = obraz[yy + 1][xx + 1].cr;

    int newCr = normalizacja((pixel1 + pixel2 + pixel3 + pixel4) / 4);

    obraz[yy][xx].cr = newCr;
    obraz[yy + 1][xx].cr = newCr;
    obraz[yy][xx + 1].cr = newCr;
    obraz[yy + 1][xx + 1].cr = newCr;
}

void probkowanieCr(YCbCr** obraz, int xx, int yy) {
    Uint8 pixel1, pixel2, pixel3, pixel4;
    pixel1 = obraz[yy][xx].cr;
    pixel2 = obraz[yy + 1][xx].cr;
    pixel3 = obraz[yy][xx + 1].cr;
    pixel4 = obraz[yy + 1][xx + 1].cr;

    int newCr = normalizacja((pixel1 + pixel2 + pixel3 + pixel4) / 4);

    obraz[yy][xx].cr = newCr;
    obraz[yy + 1][xx].cr = newCr;
    obraz[yy][xx + 1].cr = newCr;
    obraz[yy + 1][xx + 1].cr = newCr;
}

void probkowanie(YCbCr** obraz) {
    for (int y = 0; y < wysokosc / 2; y += 2) {
        for (int x = 0; x < szerokosc / 2; x += 2) {
            probkowanieCb(obraz, x, y);
            probkowanieCr(obraz, x, y);
        }
    }
}
