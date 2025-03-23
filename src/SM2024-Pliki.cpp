// funkcje do operacji na plikach

#include "SM2024-Pliki.h"
#include "SM2024-Funkcje.h"
#include "SM2024-Zmienne.h"
#include "SM2024-Kompresje.h"
#include "SM2024-Dithering.h"
#include "SM2024-Modele.h"

std::string getFileNameDialog() {
    char filename[MAX_PATH];

    OPENFILENAMEA ofn;
    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Bitmap Files\0*.bmp\0Z25 Files\0*.z25\0Any File\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Wybierz plik, yo!";
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
        std::cout << "You chose the file \"" << filename << "\"\n";
        return filename;
    } else {
        // All this stuff below is to tell you exactly how you messed up above.
        // Once you've got that fixed, you can often (not always!) reduce it to a 'user cancelled' assumption.
        switch (CommDlgExtendedError()) {
            case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";
                break;
            case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";
                break;
            case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";
                break;
            case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";
                break;
            case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";
                break;
            case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";
                break;
            case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n";
                break;
            case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";
                break;
            case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";
                break;
            case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";
                break;
            case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";
                break;
            case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";
                break;
            case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";
                break;
            case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n";
                break;
            case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n";
                break;
            default: std::cout << "You cancelled.\n";
        }
    }
}

void zapisRGB565(ofstream& wyjscie, int color, int dither, int pred, int compress){
    Uint16** obraz;
    obraz = new Uint16*[wysokosc / 2];

    for (int y = 0; y < wysokosc / 2; y++) {
        obraz[y] = new Uint16[szerokosc / 2];

        for (int x = 0; x < szerokosc / 2; x++) {
            if (color == 0)
                obraz[y][x] = getRGB565_(x, y);
            else if (color == 1) {
                SDL_Color pixel = getPixel(x, y);
                obraz[y][x] = normalizacja((0.2126 * pixel.r + 0.7152 * pixel.g + 0.0772 * pixel.b) * 65535 / 255, 0, 65535);
            }
        }
    }

    if (dither == 1)
        ditheringBayer((void**)obraz, _RGB565, color);

    if (pred == 1)
        predykcjaTyp1((void**)obraz, _RGB565, color);

    if (compress == 0)
        kompresjaLZW(wyjscie, (void**)obraz, _RGB565, color);
    else if (compress == 1)
        zapisDCT(wyjscie, (void**)obraz, _RGB565, color);
    else if (compress == 2) {
        for (int y = 0; y < wysokosc / 2; y++) {
            for (int x = 0; x < szerokosc / 2; x++) {
                wyjscie.write((char*) &obraz[y][x], sizeof(Uint16));
            }
        }
    }

}

void odczytRGB565(ifstream& wejscie, SDL_Color** obraz, Uint16 wysokoscObrazka, Uint16 szerokoscObrazka, Uint8 color, Uint8 compress, Uint8 pred) {
    Uint16** temp;
    temp = new Uint16*[wysokoscObrazka];

    for (int y = 0; y < wysokoscObrazka; y++) {
        temp[y] = new Uint16[szerokoscObrazka];
    }

    if (compress == 0)
        odczytLZW(wejscie, (void**)temp, _RGB565, wysokoscObrazka, szerokoscObrazka, color);
    else if (compress == 1)
        odczytDCT(wejscie, (void**)temp, _RGB565, wysokoscObrazka, szerokoscObrazka, color);
    else if (compress == 2) {
        for (int y = 0; y < wysokosc / 2; y++) {
            for (int x = 0; x < szerokosc / 2; x++) {
                wejscie.read((char*) &temp[y][x], sizeof(Uint16));
            }
        }
    }

    if (pred == 1)
        predykcjaTyp1Powrot((void**)temp, _RGB565, wysokoscObrazka, szerokoscObrazka, color);

    for (int x = 0; x < szerokoscObrazka; x++) {
        for (int y = 0; y < wysokoscObrazka; y++) {
            if (color == 0) {
                obraz[y][x].r = (temp[y][x] >> 11) * 255 / 31;
                obraz[y][x].g = ((0b0000011111100000 & temp[y][x]) >> 5) * 255 / 63;
                obraz[y][x].b = (0b0000000000011111 & temp[y][x]) * 255 / 31;
            } else if (color == 1) {
                obraz[y][x].r = normalizacja(temp[y][x] * 255 / 65535, 0, 255);
                obraz[y][x].g = obraz[y][x].r;
                obraz[y][x].b = obraz[y][x].r;
            }
        }
    }
}

void zapisRGB888(ofstream& wyjscie, int color, int dither, int pred, int compress){
    SDL_Color** obraz;
    obraz = new SDL_Color*[wysokosc / 2];

    for (int y = 0; y < wysokosc / 2; y++) {
        obraz[y] = new SDL_Color[szerokosc / 2];

        for (int x = 0; x < szerokosc / 2; x++) {
            obraz[y][x] = getPixel(x, y);

            if (color == 1) {
                obraz[y][x].r = normalizacja(0.2126 * obraz[y][x].r + 0.7152 * obraz[y][x].g + 0.0772 * obraz[y][x].b, 0, 255);
            }
        }
    }

    if (dither == 1)
        ditheringBayer((void**)obraz, _RGB888, color);

    if (pred == 1)
        predykcjaTyp1((void**)obraz, _RGB888, color);

    if (compress == 0)
        kompresjaLZW(wyjscie, (void**)obraz, _RGB888, color);
    else if (compress == 1)
        zapisDCT(wyjscie, (void**)obraz, _RGB888, color);
    else if (compress == 2) {
        for (int y = 0; y < wysokosc / 2; y++) {
            for (int x = 0; x < szerokosc / 2; x++) {
                wyjscie.write((char*) &obraz[y][x].r, sizeof(Uint8));
            }
        }

        if (color == 0) {
            for (int y = 0; y < wysokosc / 2; y++) {
                for (int x = 0; x < szerokosc / 2; x++) {
                    wyjscie.write((char*) &obraz[y][x].g, sizeof(Uint8));
                }
            }

            for (int y = 0; y < wysokosc / 2; y++) {
                for (int x = 0; x < szerokosc / 2; x++) {
                    wyjscie.write((char*) &obraz[y][x].b, sizeof(Uint8));
                }
            }
        }
    }
}

void odczytRGB888(ifstream& wejscie, SDL_Color** obraz, Uint16 wysokoscObrazka, Uint16 szerokoscObrazka, Uint8 color, Uint8 compress, Uint8 pred){
    if (compress == 0)
        odczytLZW(wejscie, (void**)obraz, _RGB888, wysokoscObrazka, szerokoscObrazka, color);
    else if (compress == 1)
        odczytDCT(wejscie, (void**)obraz, _RGB888, wysokoscObrazka, szerokoscObrazka, color);
    else if (compress == 2) {
        for (int y = 0; y < wysokoscObrazka; y++) {
            for (int x = 0; x < szerokoscObrazka; x++) {
                wejscie.read((char*) &obraz[y][x].r, sizeof(Uint8));
            }
        }

        if (color == 0) {
            for (int y = 0; y < wysokoscObrazka; y++) {
                for (int x = 0; x < szerokoscObrazka; x++) {
                    wejscie.read((char*) &obraz[y][x].g, sizeof(Uint8));
                }
            }

            for (int y = 0; y < wysokoscObrazka; y++) {
                for (int x = 0; x < szerokoscObrazka; x++) {
                    wejscie.read((char*) &obraz[y][x].b, sizeof(Uint8));
                }
            }
        }
    }

    if (color == 1) {
        for(int x = 0; x  < szerokoscObrazka; x++){
            for(int y = 0; y  < wysokoscObrazka; y++){
                obraz[y][x].g = obraz[y][x].r;
                obraz[y][x].b = obraz[y][x].r;
            }
        }
    }

    if (pred == 1)
        predykcjaTyp1Powrot((void**)obraz, _RGB888, wysokoscObrazka, szerokoscObrazka);
}

void zapisYCbCr888(ofstream& wyjscie, int color, int dither, int pred, int compress, int prob){
    YCbCr** obraz;
    SDL_Color** temp;
    obraz = new YCbCr*[wysokosc / 2];
    temp = new SDL_Color*[wysokosc / 2];

    for (int y = 0; y < wysokosc / 2; y++) {
        obraz[y] = new YCbCr[szerokosc / 2];
        temp[y] = new SDL_Color[szerokosc / 2];

        for (int x = 0; x < szerokosc / 2; x++) {
            temp[y][x] = getPixel(x, y);
        }
    }

    if (dither == 1)
        ditheringBayer((void**)temp, _RGB888, 0);

    for(int y = 0; y < wysokosc/2; y++){
            for(int x = 0; x < szerokosc/2; x++){
            obraz[y][x] = RGBToYCbCr(temp[y][x]);
        }

        delete [] temp[y];
    }

    delete [] temp;

    if (prob == 1)
        probkowanie(obraz);

    if (pred == 1)
        predykcjaTyp1((void**)obraz, _YCbCr888, color);

    if (compress == 0)
        kompresjaLZW(wyjscie, (void**)obraz, _YCbCr888, color);
    else if (compress == 1)
        zapisDCT(wyjscie, (void**)obraz, _YCbCr888, color);
    else if (compress == 2) {
        for (int y = 0; y < wysokosc / 2; y++) {
            for (int x = 0; x < szerokosc / 2; x++) {
                wyjscie.write((char*) &obraz[y][x].y, sizeof(Uint8));
            }
        }

        if (color == 0) {
            for (int y = 0; y < wysokosc / 2; y++) {
                for (int x = 0; x < szerokosc / 2; x++) {
                    wyjscie.write((char*) &obraz[y][x].cb, sizeof(Uint8));
                }
            }

            for (int y = 0; y < wysokosc / 2; y++) {
                for (int x = 0; x < szerokosc / 2; x++) {
                    wyjscie.write((char*) &obraz[y][x].cr, sizeof(Uint8));
                }
            }
        }
    }
}

void odczytYCbCr888(ifstream& wejscie, SDL_Color** obraz, Uint16 wysokoscObrazka, Uint16 szerokoscObrazka, Uint8 color, Uint8 compress, Uint8 pred){
    YCbCr** tablicaYCbCr888;
    tablicaYCbCr888 = new YCbCr*[wysokoscObrazka];

    for (int y = 0; y < wysokoscObrazka; y++) {
        tablicaYCbCr888[y] = new YCbCr[szerokoscObrazka];
    }

    if (compress == 0)
        odczytLZW(wejscie, (void**)tablicaYCbCr888, _YCbCr888, wysokoscObrazka, szerokoscObrazka, color);
    else if (compress == 1)
        odczytDCT(wejscie, (void**)tablicaYCbCr888, _YCbCr888, wysokoscObrazka, szerokoscObrazka, color);
    else if (compress == 2) {
        for (int y = 0; y < wysokoscObrazka; y++) {
            for (int x = 0; x < szerokoscObrazka; x++) {
                wejscie.read((char*) &tablicaYCbCr888[y][x].y, sizeof(Uint8));
            }
        }

        if (color == 0) {
            for (int y = 0; y < wysokoscObrazka; y++) {
                for (int x = 0; x < szerokoscObrazka; x++) {
                    wejscie.read((char*) &tablicaYCbCr888[y][x].cb, sizeof(Uint8));
                }
            }

            for (int y = 0; y < wysokoscObrazka; y++) {
                for (int x = 0; x < szerokoscObrazka; x++) {
                    wejscie.read((char*) &tablicaYCbCr888[y][x].cr, sizeof(Uint8));
                }
            }
        }
    }

    if (color == 0) {
        if (pred == 1)
            predykcjaTyp1Powrot((void**)tablicaYCbCr888, _YCbCr888, wysokoscObrazka, szerokoscObrazka);

        for(int x = 0; x  < szerokoscObrazka; x++){
            for(int y = 0; y  < wysokoscObrazka; y++){
                float r, g, b;
                r = tablicaYCbCr888[y][x].y + 1.402                                                     * (tablicaYCbCr888[y][x].cr - 128);
                g = tablicaYCbCr888[y][x].y - 0.344136    * (tablicaYCbCr888[y][x].cb - 128) - 0.714136 * (tablicaYCbCr888[y][x].cr - 128);
                b = tablicaYCbCr888[y][x].y + 1.772       * (tablicaYCbCr888[y][x].cb - 128);

                obraz[y][x].r = normalizacja(r, 0, 255);
                obraz[y][x].g = normalizacja(g, 0, 255);
                obraz[y][x].b = normalizacja(b, 0, 255);
            }
        }
    } else if (color == 1) {
        for(int x = 0; x  < szerokoscObrazka; x++){
            for(int y = 0; y  < wysokoscObrazka; y++){
                obraz[y][x].r = tablicaYCbCr888[y][x].y;
                obraz[y][x].g = obraz[y][x].r;
                obraz[y][x].b = obraz[y][x].r;
            }
        }

        if (pred == 1)
            predykcjaTyp1Powrot((void**)obraz, _RGB888, wysokoscObrazka, szerokoscObrazka);
    }
}

void zapis(std::string path, tryby t, int color, int dither, int pred, int compress, int prob) {
    char identyfikator[] = "KW";
    Uint16 szerokoscObrazka = szerokosc / 2;
    Uint16 wysokoscObrazka = wysokosc / 2;
    ofstream wyjscie(path, ios::binary);

    wyjscie.write((char *) &identyfikator, sizeof(char) * 2);
    wyjscie.write((char *) &szerokoscObrazka, sizeof(Uint16));
    wyjscie.write((char *) &wysokoscObrazka, sizeof(Uint16));

    auto quikWrite = [&](Uint8 t, Uint8 col, Uint8 d, Uint8 p, Uint8 comp) {
        wyjscie.write((char *) &t, sizeof(Uint8));
        wyjscie.write((char *) &col, sizeof(Uint8));
        wyjscie.write((char *) &d, sizeof(Uint8));
        wyjscie.write((char *) &p, sizeof(Uint8));
        wyjscie.write((char *) &comp, sizeof(Uint8));
    };

    switch (t) {
    case _RGB565:
        quikWrite(t, color, dither, pred, compress);
        zapisRGB565(wyjscie, color, dither, pred, compress);
        break;
    case _RGB888:
        quikWrite(t, color, dither, pred, compress);
        zapisRGB888(wyjscie, color, dither, pred, compress);
        break;
    case _YCbCr888:
        quikWrite(t, color, dither, pred, compress);
        zapisYCbCr888(wyjscie, color, dither, pred, compress, prob);
        break;
    }

    wyjscie.close();
}

void odczyt(std::string path) {
    char identyfikator[] = "  ";
    Uint16 szerokoscObrazka = 0;
    Uint16 wysokoscObrazka = 0;
    Uint8 tryb, color, dither, pred, compress;

    ifstream wejscie(path, ios::binary);

    wejscie.read((char *) &identyfikator, sizeof(char) * 2);
    wejscie.read((char *) &szerokoscObrazka, sizeof(Uint16));
    wejscie.read((char *) &wysokoscObrazka, sizeof(Uint16));
    wejscie.read((char *) &tryb, sizeof(Uint8));
    wejscie.read((char *) &color, sizeof(Uint8));
    wejscie.read((char *) &dither, sizeof(Uint8));
    wejscie.read((char *) &pred, sizeof(Uint8));
    wejscie.read((char *) &compress, sizeof(Uint8));

    Uint8 odczytanyKolor;
    Uint8 kolor1, kolor2;
    SDL_Color wyswietlanyKolor1, wyswietlanyKolor2, odczytPaleta;

    SDL_Color** obraz;
    obraz = new SDL_Color*[wysokoscObrazka];

    for (int y = 0; y < wysokoscObrazka; y++) {
        obraz[y] = new SDL_Color[szerokoscObrazka];
    }

    switch (tryb) {
    case _RGB565:
        odczytRGB565(wejscie, obraz, wysokoscObrazka, szerokoscObrazka, color, compress, pred);
        break;
    case _RGB888:
        odczytRGB888(wejscie, obraz, wysokoscObrazka, szerokoscObrazka, color, compress, pred);
        break;
    case _YCbCr888:
        odczytYCbCr888(wejscie, obraz, wysokoscObrazka, szerokoscObrazka, color, compress, pred);
        break;
    };

    for (int x = 0; x < szerokoscObrazka; x++) {
        for (int y = 0; y < wysokoscObrazka; y++) {
            obraz[y][x].r = normalizacja(obraz[y][x].r, 0, 255);
            obraz[y][x].g = normalizacja(obraz[y][x].g, 0, 255);
            obraz[y][x].b = normalizacja(obraz[y][x].b, 0, 255);

            setPixel(x + szerokoscObrazka, y, obraz[y][x].r, obraz[y][x].g, obraz[y][x].b);
        }
    }

    SDL_UpdateWindowSurface(window);
}

void ladujBMP(string nazwa, int x, int y) {
    if (nazwa.size() == 0) return;
    if (nazwa[nazwa.size() - 1] == 'p') {
        SDL_Surface *bmp = SDL_LoadBMP(nazwa.c_str());
        if (!bmp) {
            printf("Unable to load bitmap: %s\n", SDL_GetError());
        } else {
            SDL_Color kolor;
            for (int yy = 0; yy < bmp->h; yy++) {
                for (int xx = 0; xx < bmp->w; xx++) {
                    kolor = getPixelSurface(xx, yy, bmp);
                    setPixel(xx, yy, kolor.r, kolor.g, kolor.b);
                }
            }

            SDL_FreeSurface(bmp);
            SDL_UpdateWindowSurface(window);
        }
        zapiszZ25();
    } else odczyt(nazwa);
}

void zapiszZ25() {
    std::string path;
    int mode, color, dither, pred = 0, compress, prob = 0; //compress: 0 - LZW, 1 - DCT
    std::cout << "Wybierz tryb zapisu:\n";

    for (int trybyInt = 0; trybyInt != 3; trybyInt++) {
        auto t = static_cast<tryby>(trybyInt);
        std::cout << trybyInt + 1 << ". " << ToString(t) << "\n";
    }

    std::cin >> mode;
    std::cout << "Kolor?" << std::endl;
    std::cout << "\t0. Zwykly\n\t1. Skala szarosci\nWybor: ";
    std::cin >> color;
    std::cout << "Dithering?" << std::endl;
    std::cout << "\t0. Brak\n\t1. Dithering z tablica Bayera\nWybor: ";
    std::cin >> dither;
    std::cout << "Kompresja?" << std::endl;
    std::cout << "\t0. Bezstratna\n\t1. Stratna\n\t2. Brak\nWybor: ";
    std::cin >> compress;

    if (compress != 1) {
        std::cout << "Predykcja?" << std::endl;
        std::cout << "\t0. Brak\n\t1. Predykcja typu 1\nWybor: ";
        std::cin >> pred;
    }

    if (mode == 3) {
        std::cout << "Probkowanie (4:2:0)?" << std::endl;
        std::cout << "\t0. Nie\n\t1. Tak\nWybor: ";
        std::cin >> prob;
    }

    std::cout << "Wprowadz nazwe pliku do zapisu z ewentualna sciezka:\n";
    std::cin >> path;
    path += ".z25";
    zapis(path, static_cast<tryby>(mode - 1), color, dither, pred, compress, prob);
}
