#include "SM2024-Kompresje.h"
#include "SM2024-Funkcje.h"
#include "SM2024-Zmienne.h"
#include "SM2024-Pliki.h"
#include "SM2024-Modele.h"

slowo noweSlowo() {
    slowo noweSlowo;
    noweSlowo.kod = 0;
    noweSlowo.dlugosc = 0;
    noweSlowo.wSlowniku = false;
    return noweSlowo;
}

slowo noweSlowo(Uint8 znak) {
    slowo noweSlowo;
    noweSlowo.kod = 0;
    noweSlowo.dlugosc = 1;
    noweSlowo.element[0] = znak;
    noweSlowo.wSlowniku = false;
    return noweSlowo;
}

slowo polaczSlowo(slowo aktualneSlowo, Uint8 znak) {
    slowo noweSlowo;

    if(aktualneSlowo.dlugosc < 4096) {
        noweSlowo.kod = 0;
        noweSlowo.dlugosc = aktualneSlowo.dlugosc + 1;
        noweSlowo.wSlowniku = false;
        copy(begin(aktualneSlowo.element), end(aktualneSlowo.element), begin(noweSlowo.element));
        noweSlowo.element[aktualneSlowo.dlugosc] = znak;
        return noweSlowo;
    } else {
        noweSlowo.kod = 0;
        noweSlowo.dlugosc = 0;
        noweSlowo.wSlowniku = false;
        noweSlowo.element[0] = znak;
        return noweSlowo;
    }
}

bool porownajSlowa(slowo slowo1, slowo slowo2) {
    if(slowo1.dlugosc != slowo2.dlugosc){
        return false;
    }

    for(int s = 0; s < slowo1.dlugosc; s++){
        if(slowo1.element[s] != slowo2.element[s]){
            return false;
        }
    }

    return true;
}

int znajdzWSlowniku(slowo szukany) {
    for(int nr = 0; nr < rozmiarSlownika; nr++){
        if(porownajSlowa(slownik[nr], szukany)){
            return nr;
        }
    }
    return -1;
}

void wyswietlSlowo(slowo aktualneSlowo){
    if(aktualneSlowo.wSlowniku){
        cout << "[" << aktualneSlowo.kod << "] ";
    } else {
        cout << "[X] ";
    }

    for(int s = 0; s < aktualneSlowo.dlugosc; s++){
        cout << (int)aktualneSlowo.element[s];
        if(s < aktualneSlowo.dlugosc - 1){
            cout << ", ";
        }
    }
    cout << endl;
}

int dodajDoSlownika(slowo nowy, bool czyWyswietlac=false){
    if (rozmiarSlownika <= 65535) {
        Uint16 nr = rozmiarSlownika;
        slownik[nr].kod = nr;
        slownik[nr].dlugosc = nowy.dlugosc;
        copy(begin(nowy.element), end(nowy.element), begin(slownik[nr].element));
        slownik[nr].wSlowniku = true;

        if(czyWyswietlac){
            wyswietlSlowo(slownik[nr]);
        }

        rozmiarSlownika++;
        return nr;
    }

    return -1;
}

void wyswietlSlownik() {
    for(int i = 0; i < rozmiarSlownika; i++){
        cout << "[" << slownik[i].kod << "] ";

        for (int j = 0; j < slownik[i].dlugosc; j++){
            cout << (int)slownik[i].element[j];
            if(j < slownik[i].dlugosc - 1){
                cout << ", ";
            }
        }
        cout << endl;
    }
}

void LZWinicjalizacja(int zakres){
    rozmiarSlownika = 0;

    for (int s = 0; s < 65536; s++){
        slownik[s].kod = 0;
        slownik[s].dlugosc = 0;
        slownik[s].wSlowniku = false;
        memset(slownik[s].element, 0, sizeof(slownik[s].element));
    }

    slowo noweSlowo;

    for(int s = 0; s < zakres; s++){
        noweSlowo.dlugosc = 1;
        noweSlowo.element[0] = s;
        noweSlowo.kod = dodajDoSlownika(noweSlowo);
    }
}

void LZWZapis(std::ofstream &wyjscie, Uint8** arr, int zakres) {
    LZWinicjalizacja(zakres);

    slowo aktualneSlowo = noweSlowo();
    slowo slowoZnak;
    Uint8 znak;
    int kod;
    int x = 0, y = 0;

    std::vector<Uint16> out;

    for(int y = 0; y < wysokosc/2; y++){
        for(int x = 0; x < szerokosc/2; x++){
            znak = arr[y][x];
            slowoZnak = polaczSlowo(aktualneSlowo, znak);
            kod = znajdzWSlowniku(slowoZnak);

            if(kod < 0){
                dodajDoSlownika(slowoZnak, false);
                out.push_back(aktualneSlowo.kod);

                if(znajdzWSlowniku(slowoZnak) > 0){
                    slowoZnak.kod = znajdzWSlowniku(slowoZnak);;
                    //wyswietlSlowo(slownik[slowoZnak.kod]);
                }

                aktualneSlowo = noweSlowo(znak);
                aktualneSlowo.kod = znajdzWSlowniku(aktualneSlowo);
                aktualneSlowo.wSlowniku = true;

            } else {
                aktualneSlowo = slowoZnak;
                aktualneSlowo.kod = znajdzWSlowniku(aktualneSlowo);
                aktualneSlowo.wSlowniku = true;
            }
        }
    }

    out.push_back(aktualneSlowo.kod);

    Uint16 vectorSize = out.size();
    wyjscie.write((char*) &vectorSize, sizeof(Uint16));

    for (int i = 0; i < out.size(); i++) {
        wyjscie.write((char *) &out[i], sizeof(Uint16));
    }
}

void kompresjaLZW(std::ofstream &wyjscie, void** arr, tryby t, int color) {
    switch (t) {
        case _RGB565: {
            Uint16** obraz = reinterpret_cast<Uint16**>(arr);

            if (color == 0) {
                Uint8** r = new Uint8*[wysokosc / 2];
                Uint8** g = new Uint8*[wysokosc / 2];
                Uint8** b = new Uint8*[wysokosc / 2];

                for (int y = 0; y < wysokosc / 2; y++) {
                    r[y] = new Uint8[szerokosc / 2];
                    g[y] = new Uint8[szerokosc / 2];
                    b[y] = new Uint8[szerokosc / 2];

                    for (int x = 0; x < szerokosc / 2; x++) {
                        r[y][x] = obraz[y][x] >> 11;
                        g[y][x] = (obraz[y][x] & 0b0000011111100000) >> 5;
                        b[y][x] = obraz[y][x] & 0b0000000000011111;
                    }
                }

                LZWZapis(wyjscie, r, 32);
                LZWZapis(wyjscie, g, 64);
                LZWZapis(wyjscie, b, 32);
            } else if (color == 1) {
                Uint8** szary = new Uint8*[wysokosc / 2];

                for (int y = 0; y < wysokosc / 2; y++) {
                    szary[y] = new Uint8[szerokosc / 2];

                    for (int x = 0; x < szerokosc / 2; x++) {
                        szary[y][x] = obraz[y][x] % 255;
                    }
                }

                LZWZapis(wyjscie, szary, 256);
            }

            break;
        }
        case _RGB888: {
            SDL_Color** obraz = reinterpret_cast<SDL_Color**>(arr);

            if (color == 0) {
                Uint8** r = new Uint8*[wysokosc / 2];
                Uint8** g = new Uint8*[wysokosc / 2];
                Uint8** b = new Uint8*[wysokosc / 2];

                for (int y = 0; y < wysokosc / 2; y++) {
                    r[y] = new Uint8[szerokosc / 2];
                    g[y] = new Uint8[szerokosc / 2];
                    b[y] = new Uint8[szerokosc / 2];

                    for (int x = 0; x < szerokosc / 2; x++) {
                        r[y][x] = obraz[y][x].r;
                        g[y][x] = obraz[y][x].g;
                        b[y][x] = obraz[y][x].b;
                    }
                }

                LZWZapis(wyjscie, r, 256);
                LZWZapis(wyjscie, g, 256);
                LZWZapis(wyjscie, b, 256);
            } else if (color == 1) {
                Uint8** szary = new Uint8*[wysokosc / 2];

                for (int y = 0; y < wysokosc / 2; y++) {
                    szary[y] = new Uint8[szerokosc / 2];

                    for (int x = 0; x < szerokosc / 2; x++) {
                        szary[y][x] = obraz[y][x].r;
                    }
                }

                LZWZapis(wyjscie, szary, 256);
            }

            break;
        }
        case _YCbCr888: {
            YCbCr** obraz = reinterpret_cast<YCbCr**>(arr);

            if (color == 0) {
                Uint8** y = new Uint8*[wysokosc / 2];
                Uint8** cb = new Uint8*[wysokosc / 2];
                Uint8** cr = new Uint8*[wysokosc / 2];

                for (int yy = 0; yy < wysokosc / 2; yy++) {
                    y[yy] = new Uint8[szerokosc / 2];
                    cb[yy] = new Uint8[szerokosc / 2];
                    cr[yy] = new Uint8[szerokosc / 2];

                    for (int x = 0; x < szerokosc / 2; x++) {
                        y[yy][x] = obraz[yy][x].y;
                        cb[yy][x] = obraz[yy][x].cb;
                        cr[yy][x] = obraz[yy][x].cr;
                    }
                }

                LZWZapis(wyjscie, y, 256);
                LZWZapis(wyjscie, cb, 256);
                LZWZapis(wyjscie, cr, 256);
            } else if (color == 1) {
                Uint8** szary = new Uint8*[wysokosc / 2];

                for (int y = 0; y < wysokosc / 2; y++) {
                    szary[y] = new Uint8[szerokosc / 2];

                    for (int x = 0; x < szerokosc / 2; x++) {
                        szary[y][x] = obraz[y][x].y;
                    }
                }

                LZWZapis(wyjscie, szary, 256);
            }
        }
    }
}

vector<Uint8> dekompresjaLZW(std::ifstream& wejscie, int zakres) {
    LZWinicjalizacja(zakres);

    vector<Uint8> wynik;
    slowo aktualneSlowo, poprzednieSlowo;
    Uint16 kod;
    Uint16 dlugosc;

    wejscie.read((char*) &dlugosc, sizeof(Uint16));
    wejscie.read((char*) &kod, sizeof(Uint16));

    aktualneSlowo = slownik[kod];
    wynik.insert(wynik.end(), aktualneSlowo.element, aktualneSlowo.element + aktualneSlowo.dlugosc);
    poprzednieSlowo = aktualneSlowo;

    for(int i = 1; i < dlugosc; i++) {
        wejscie.read((char*) &kod, sizeof(Uint16));

        if(kod < rozmiarSlownika && slownik[kod].wSlowniku) {
            aktualneSlowo = slownik[kod];
            wynik.insert(wynik.end(), aktualneSlowo.element, aktualneSlowo.element + aktualneSlowo.dlugosc);

            slowo noweSlowo = polaczSlowo(poprzednieSlowo, aktualneSlowo.element[0]);
            dodajDoSlownika(noweSlowo, false);
        }
        else if(kod == rozmiarSlownika) {
            slowo noweSlowo = polaczSlowo(poprzednieSlowo, poprzednieSlowo.element[0]);
            aktualneSlowo = noweSlowo;
            wynik.insert(wynik.end(), aktualneSlowo.element, aktualneSlowo.element + aktualneSlowo.dlugosc);
            dodajDoSlownika(noweSlowo, false);
        }

        poprzednieSlowo = aktualneSlowo;
    }

    for (int i = wynik.size(); i < 64000; i++) {
        wynik.push_back(0);
    }

    return wynik;
}

void odczytLZW(std::ifstream &wejscie, void** arr, tryby t, int n, int m, int color) {
    switch (t) {
        case _RGB565: {
            Uint16** obraz = reinterpret_cast<Uint16**>(arr);

            if (color == 0) {
                Uint8** r = new Uint8*[n];
                Uint8** g = new Uint8*[n];
                Uint8** b = new Uint8*[n];

                for (int y = 0; y < n; y++) {
                    r[y] = new Uint8[m];
                    g[y] = new Uint8[m];
                    b[y] = new Uint8[m];
                }

                vector<Uint8> wynik;
                wynik = dekompresjaLZW(wejscie, 32);

                for (int y = 0; y < n; y++) {
                    for (int x = 0; x < m; x++) {
                        r[y][x] = wynik[y * m + x];
                    }
                }

                wynik = dekompresjaLZW(wejscie, 64);

                for (int y = 0; y < n; y++) {
                    for (int x = 0; x < m; x++) {
                        g[y][x] = wynik[y * m + x];
                    }
                }

                wynik = dekompresjaLZW(wejscie, 32);

                for (int y = 0; y < n; y++) {
                    for (int x = 0; x < m; x++) {
                        b[y][x] = wynik[y * m + x];
                    }
                }

                for (int y = 0; y < n; y++) {
                    for (int x = 0; x < m; x++) {
                        obraz[y][x] = (r[y][x] << 11) | (g[y][x] << 5) | b[y][x];
                    }
                }
            } else if (color == 1) {
                vector<Uint8> wynik;
                wynik = dekompresjaLZW(wejscie, 256);

                for (int y = 0; y < n; y++) {
                    for (int x = 0; x < m; x++) {
                        obraz[y][x] = wynik[y * m + x];
                    }
                }
            }

            break;
        }
        case _RGB888: {
            SDL_Color** obraz = reinterpret_cast<SDL_Color**>(arr);

            if (color == 0) {
                vector<Uint8> wynik;
                wynik = dekompresjaLZW(wejscie, 256);

                for (int y = 0; y < n; y++) {
                    for (int x = 0; x < m; x++) {
                        obraz[y][x].r = wynik[y * m + x];
                    }
                }

                wynik = dekompresjaLZW(wejscie, 256);

                for (int y = 0; y < n; y++) {
                    for (int x = 0; x < m; x++) {
                        obraz[y][x].g = wynik[y * m + x];
                    }
                }

                wynik = dekompresjaLZW(wejscie, 256);

                for (int y = 0; y < n; y++) {
                    for (int x = 0; x < m; x++) {
                        obraz[y][x].b = wynik[y * m + x];
                    }
                }
            } else if (color == 1) {
                vector<Uint8> wynik;
                wynik = dekompresjaLZW(wejscie, 256);

                for (int y = 0; y < n; y++) {
                    for (int x = 0; x < m; x++) {
                        obraz[y][x].r = wynik[y * m + x];
                    }
                }
            }

            break;
        }
        case _YCbCr888: {
            YCbCr** obraz = reinterpret_cast<YCbCr**>(arr);

            if (color == 1) {
                vector<Uint8> wynik;
                wynik = dekompresjaLZW(wejscie, 256);

                for (int y = 0; y < n; y++) {
                    for (int x = 0; x < m; x++) {
                        obraz[y][x].y = wynik[y * m + x];
                    }
                }

                wynik = dekompresjaLZW(wejscie, 256);

                for (int y = 0; y < n; y++) {
                    for (int x = 0; x < m; x++) {
                        obraz[y][x].cb = wynik[y * m + x];
                    }
                }

                wynik = dekompresjaLZW(wejscie, 256);

                for (int y = 0; y < n; y++) {
                    for (int x = 0; x < m; x++) {
                        obraz[y][x].cr = wynik[y * m + x];
                    }
                }
            } else if (color == 1) {
                vector<Uint8> wynik;
                wynik = dekompresjaLZW(wejscie, 256);

                for (int y = 0; y < n; y++) {
                    for (int x = 0; x < m; x++) {
                        obraz[y][x].y = wynik[y * m + x];
                    }
                }
            }
        }
    }
}

void wyswietlDane(macierz blok) {
    cout << "Dane pikselowe w macierzy" << endl;
    for (int y = 0; y < rozmiarBloku; y++){
        for (int x = 0; x < rozmiarBloku; x++){
            cout << setw(4) << (int)blok.dane[x][y] << " ";
        }
        cout << endl;
    }
}

void wyswietlDCT(macierz blok) {
    cout << "Wspolczynniki transformaty w macierzy:" << endl;
    for (int y = 0; y < rozmiarBloku; y++){
        for (int x = 0; x < rozmiarBloku; x++){
            cout << fixed << setw(6) << setprecision(2) << blok.dct[x][y] << " ";
        }
        cout << endl;
    }
}

void DCTKompresja(macierz* blok) {
    for (int u = 0; u < rozmiarBloku; u++){
        for (int v = 0; v < rozmiarBloku; v++){
            const double cu = (u == 0) ? 1.0 / sqrt(2) : 1.0;
            const double cv = (v == 0) ? 1.0 / sqrt(2) : 1.0;
            double wspolczynnikDCT = 0;

            for (int y = 0; y < rozmiarBloku; y++){
                for (int x = 0; x< rozmiarBloku; x++){
                    double uCosFactor = cos((double)(2 * x + 1) * M_PI * (double) u / (2 * (double) rozmiarBloku));
                    double vCosFactor = cos((double)(2 * y + 1) * M_PI * (double) v / (2 * (double) rozmiarBloku));
                    double pixel = (double) blok->dane[y][x];
                    wspolczynnikDCT += pixel * uCosFactor * vCosFactor;
                }
            }
            wspolczynnikDCT *= (2.0 / (double) rozmiarBloku) * cu * cv;
            blok->dct[u][v] = wspolczynnikDCT;
        }
    }
}

Uint16 DCT(macierz* blok) {
    DCTKompresja(blok);

    int maximum = 0;

    for(int y = 0; y < rozmiarBloku; y++){
        for(int x = 0; x < rozmiarBloku; x++){
            if (x==0 && y==0) continue;
            int num = blok->dct[y][x];
            if(abs(num) > maximum){
                maximum = abs(num);
            }
        }
    }

    float divider = 127.0/(maximum * 1.0);

    if (maximum == 0)
        divider = 1;

    for(int y = 0; y < rozmiarBloku; y++){
        for(int x = 0; x < rozmiarBloku; x++){
            if (x==0 && y==0) continue;
            blok->dct[y][x] = blok->dct[y][x]/divider + 127;
        }
    }

    return maximum;
}

void zapisDCT(std::ofstream &wyjscie, void** arr, tryby t, int color) {
    switch (t) {
    case _RGB565: {
        Uint16** obraz = reinterpret_cast<Uint16**>(arr);

        if (color == 0) {
            macierz blokiR[wysokosc / (2 * rozmiarBloku)][szerokosc / (2 * rozmiarBloku)];
            macierz blokiG[wysokosc / (2 * rozmiarBloku)][szerokosc / (2 * rozmiarBloku)];
            macierz blokiB[wysokosc / (2 * rozmiarBloku)][szerokosc / (2 * rozmiarBloku)];

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            blokiR[i][j].dane[y][x] = ((obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)] >> 11) & 0b11111) * 255 / 31;
                            blokiG[i][j].dane[y][x] = ((obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)] >> 5) & 0b111111) * 255 / 63;
                            blokiB[i][j].dane[y][x] = (obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)] & 0b11111) * 255 / 31;
                        }
                    }

                    blokiR[i][j].maxDCT = DCT(&blokiR[i][j]);
                    blokiG[i][j].maxDCT = DCT(&blokiG[i][j]);
                    blokiB[i][j].maxDCT = DCT(&blokiB[i][j]);
                }
            }

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    wyjscie.write((char*) &blokiR[i][j].maxDCT, sizeof(Uint16));
                    wyjscie.write((char*) &blokiG[i][j].maxDCT, sizeof(Uint16));
                    wyjscie.write((char*) &blokiB[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 coeffDCT = normalizacja((Uint16)blokiR[i][j].dct[0][0], 0, 65535);
                                wyjscie.write((char*) &coeffDCT, sizeof(Uint16));
                                coeffDCT = normalizacja((Uint16)blokiG[i][j].dct[0][0], 0, 65535);
                                wyjscie.write((char*) &coeffDCT, sizeof(Uint16));
                                coeffDCT = normalizacja((Uint16)blokiB[i][j].dct[0][0], 0, 65535);
                                wyjscie.write((char*) &coeffDCT, sizeof(Uint16));
                                continue;
                            }

                            Uint8 r, g, b;
                            Uint16 dct;

                            r = (Uint8)blokiR[i][j].dct[y][x] * 31 / 255;
                            g = (Uint8)blokiG[i][j].dct[y][x] * 63 / 255;
                            b = (Uint8)blokiB[i][j].dct[y][x] * 31 / 255;

                            dct = (r << 11) | (g << 5) | b;

                            wyjscie.write((char*) &dct, sizeof(Uint16));
                        }
                    }
                }
            }
        } else if (color == 1) {
            macierz bloki[wysokosc / (2 * rozmiarBloku)][szerokosc / (2 * rozmiarBloku)];

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            bloki[i][j].dane[y][x] = obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)] * 255 / 65535;
                        }
                    }

                    bloki[i][j].maxDCT = DCT(&bloki[i][j]);
                }
            }

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    wyjscie.write((char*) &bloki[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 coeffDCT = normalizacja((Uint16)bloki[i][j].dct[0][0], 0, 65535);
                                wyjscie.write((char*) &coeffDCT, sizeof(Uint16));
                                continue;
                            }

                            Uint8 dct = normalizacja(bloki[i][j].dct[y][x], 0, 255);
                            wyjscie.write((char*) &dct, sizeof(Uint8));
                        }
                    }
                }
            }
        }

        break;
    }
    case _RGB888: {
        SDL_Color** obraz = reinterpret_cast<SDL_Color**>(arr);

        if (color == 0) {
            macierz blokiR[wysokosc / (2 * rozmiarBloku)][szerokosc / (2 * rozmiarBloku)];
            macierz blokiG[wysokosc / (2 * rozmiarBloku)][szerokosc / (2 * rozmiarBloku)];
            macierz blokiB[wysokosc / (2 * rozmiarBloku)][szerokosc / (2 * rozmiarBloku)];

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            blokiR[i][j].dane[y][x] = obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].r;
                            blokiG[i][j].dane[y][x] = obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].g;
                            blokiB[i][j].dane[y][x] = obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].b;
                        }
                    }

                    blokiR[i][j].maxDCT = DCT(&blokiR[i][j]);
                    blokiG[i][j].maxDCT = DCT(&blokiG[i][j]);
                    blokiB[i][j].maxDCT = DCT(&blokiB[i][j]);
                }
            }

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    wyjscie.write((char*) &blokiR[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 coeffDCT = normalizacja((Uint16)blokiR[i][j].dct[0][0], 0, 65535);
                                wyjscie.write((char*) &coeffDCT, sizeof(Uint16));
                                continue;
                            }

                            Uint8 dct = normalizacja(blokiR[i][j].dct[y][x], 0, 255);
                            wyjscie.write((char*) &dct, sizeof(Uint8));
                        }
                    }
                }
            }

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    wyjscie.write((char*) &blokiG[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 coeffDCT = normalizacja((Uint16)blokiG[i][j].dct[0][0], 0, 65535);
                                wyjscie.write((char*) &coeffDCT, sizeof(Uint16));
                                continue;
                            }

                            Uint8 dct = normalizacja(blokiG[i][j].dct[y][x], 0, 255);
                            wyjscie.write((char*) &dct, sizeof(Uint8));
                        }
                    }
                }
            }

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    wyjscie.write((char*) &blokiB[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 coeffDCT = normalizacja((Uint16)blokiB[i][j].dct[0][0], 0, 65535);
                                wyjscie.write((char*) &coeffDCT, sizeof(Uint16));
                                continue;
                            }

                            Uint8 dct = normalizacja(blokiB[i][j].dct[y][x], 0, 255);
                            wyjscie.write((char*) &dct, sizeof(Uint8));
                        }
                    }
                }
            }
        } else if (color == 1) {
            macierz bloki[wysokosc / (2 * rozmiarBloku)][szerokosc / (2 * rozmiarBloku)];

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            bloki[i][j].dane[y][x] = obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].r;
                        }
                    }

                    bloki[i][j].maxDCT = DCT(&bloki[i][j]);
                }
            }

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    wyjscie.write((char*) &bloki[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 coeffDCT = normalizacja((Uint16)bloki[i][j].dct[0][0], 0, 65535);
                                wyjscie.write((char*) &coeffDCT, sizeof(Uint16));
                                continue;
                            }

                            Uint8 dct = normalizacja(bloki[i][j].dct[y][x], 0, 255);
                            wyjscie.write((char*) &dct, sizeof(Uint8));
                        }
                    }
                }
            }
        }

        break;
    }
    case _YCbCr888: {
        YCbCr** obraz = reinterpret_cast<YCbCr**>(arr);

        if (color == 0) {
            macierz blokiY[wysokosc / (2 * rozmiarBloku)][szerokosc / (2 * rozmiarBloku)];
            macierz blokiCb[wysokosc / (2 * rozmiarBloku)][szerokosc / (2 * rozmiarBloku)];
            macierz blokiCr[wysokosc / (2 * rozmiarBloku)][szerokosc / (2 * rozmiarBloku)];

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            blokiY[i][j].dane[y][x] = obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].y;
                            blokiCb[i][j].dane[y][x] = obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].cb;
                            blokiCr[i][j].dane[y][x] = obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].cr;
                        }
                    }

                    blokiY[i][j].maxDCT = DCT(&blokiY[i][j]);
                    blokiCb[i][j].maxDCT = DCT(&blokiCb[i][j]);
                    blokiCr[i][j].maxDCT = DCT(&blokiCr[i][j]);
                }
            }

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    wyjscie.write((char*) &blokiY[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 coeffDCT = normalizacja((Uint16)blokiY[i][j].dct[0][0], 0, 65535);
                                wyjscie.write((char*) &coeffDCT, sizeof(Uint16));
                                continue;
                            }

                            Uint8 dct = normalizacja(blokiY[i][j].dct[y][x], 0, 255);
                            wyjscie.write((char*) &dct, sizeof(Uint8));
                        }
                    }
                }
            }

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    wyjscie.write((char*) &blokiCb[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 coeffDCT = normalizacja((Uint16)blokiCb[i][j].dct[0][0], 0, 65535);
                                wyjscie.write((char*) &coeffDCT, sizeof(Uint16));
                                continue;
                            }

                            Uint8 dct = normalizacja(blokiCb[i][j].dct[y][x], 0, 255);
                            wyjscie.write((char*) &dct, sizeof(Uint8));
                        }
                    }
                }
            }

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    wyjscie.write((char*) &blokiCr[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 coeffDCT = normalizacja((Uint16)blokiCr[i][j].dct[0][0], 0, 65535);
                                wyjscie.write((char*) &coeffDCT, sizeof(Uint16));
                                continue;
                            }

                            Uint8 dct = normalizacja(blokiCr[i][j].dct[y][x], 0, 255);
                            wyjscie.write((char*) &dct, sizeof(Uint8));
                        }
                    }
                }
            }
        } else if (color == 1) {
            macierz bloki[wysokosc / (2 * rozmiarBloku)][szerokosc / (2 * rozmiarBloku)];

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            bloki[i][j].dane[y][x] = obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].y;
                        }
                    }

                    bloki[i][j].maxDCT = DCT(&bloki[i][j]);
                }
            }

            for (int i = 0; i < wysokosc / (rozmiarBloku * 2); i++) {
                for (int j = 0; j < szerokosc / (rozmiarBloku * 2); j++) {
                    wyjscie.write((char*) &bloki[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 coeffDCT = normalizacja((Uint16)bloki[i][j].dct[0][0], 0, 65535);
                                wyjscie.write((char*) &coeffDCT, sizeof(Uint16));
                                continue;
                            }

                            Uint8 dct = normalizacja(bloki[i][j].dct[y][x], 0, 255);
                            wyjscie.write((char*) &dct, sizeof(Uint8));
                        }
                    }
                }
            }
        }
    }
    }
}

void DCTDekompresja(macierz* blok, Uint16 maxDCT) {
    double divider = (maxDCT == 0) ? 1 : 127.0 / maxDCT;

    for (int y = 0; y < rozmiarBloku; y++) {
        for (int x = 0; x < rozmiarBloku; x++) {
            if (x == 0 && y == 0) continue;
            blok->dct[y][x] = (blok->dct[y][x] - 127) * divider;
        }
    }

    for (int y = 0; y < rozmiarBloku; y++){
        for (int x = 0; x< rozmiarBloku; x++){
            double pixel = 0;

            for (int u = 0; u < rozmiarBloku; ++u){
                for (int v = 0; v < rozmiarBloku; ++v){
                    const double cu = (u == 0) ? 1.0 / sqrt(2) : 1.0;
                    const double cv = (v == 0) ? 1.0 / sqrt(2) : 1.0;
                    double uCosFactor = cos((double)(2 * x + 1) * M_PI * (double) u / (2 * (double) rozmiarBloku));
                    double vCosFactor = cos((double)(2 * y + 1) * M_PI * (double) v / (2 * (double) rozmiarBloku));

                    double wspolczynnikDCT = blok->dct[u][v];
                    pixel += wspolczynnikDCT * uCosFactor * cu * vCosFactor * cv;
                }
            }

            pixel *= 2.0 / (double) rozmiarBloku;

            blok->dane[y][x] = normalizacja(pixel, 0, 255);
        }
    }
}

void odczytDCT(std::ifstream &wejscie, void** arr, tryby t, int n, int m, int color) {
    switch (t) {
    case _RGB565: {
        Uint16** obraz = reinterpret_cast<Uint16**>(arr);

        if (color == 0) {
            macierz blokiR[n / rozmiarBloku][m / rozmiarBloku];
            macierz blokiG[n / rozmiarBloku][m / rozmiarBloku];
            macierz blokiB[n / rozmiarBloku][m / rozmiarBloku];

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    wejscie.read((char*) &blokiR[i][j].maxDCT, sizeof(Uint16));
                    wejscie.read((char*) &blokiG[i][j].maxDCT, sizeof(Uint16));
                    wejscie.read((char*) &blokiB[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 first;
                                wejscie.read((char*) &first, sizeof(Uint16));
                                blokiR[i][j].dct[0][0] = (float)first;
                                wejscie.read((char*) &first, sizeof(Uint16));
                                blokiG[i][j].dct[0][0] = (float)first;
                                wejscie.read((char*) &first, sizeof(Uint16));
                                blokiB[i][j].dct[0][0] = (float)first;
                                continue;
                            }

                            Uint8 r, g, b;
                            Uint16 readData;

                            wejscie.read((char*) &readData, sizeof(Uint16));

                            r = ((readData >> 11) & 0b11111) * 255 / 31;
                            g = ((readData >> 5) & 0b111111) * 255 / 63;
                            b = (readData & 0b11111) * 255 / 31;

                            blokiR[i][j].dct[y][x] = r;
                            blokiG[i][j].dct[y][x] = g;
                            blokiB[i][j].dct[y][x] = b;
                        }
                    }

                    DCTDekompresja(&blokiR[i][j], blokiR[i][j].maxDCT);
                    DCTDekompresja(&blokiG[i][j], blokiG[i][j].maxDCT);
                    DCTDekompresja(&blokiB[i][j], blokiB[i][j].maxDCT);
                }
            }

            Uint8 r, g, b;

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            r = blokiR[i][j].dane[y][x] * 31 / 255;
                            g = blokiG[i][j].dane[y][x] * 63 / 255;
                            b = blokiB[i][j].dane[y][x] * 31 / 255;

                            obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)] = (r << 11) | (g << 5) | b;
                        }
                    }
                }
            }
        } else if (color == 1) {
            macierz bloki[n / rozmiarBloku][m / rozmiarBloku];

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    wejscie.read((char*) &bloki[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 first;
                                wejscie.read((char*) &first, sizeof(Uint16));
                                bloki[i][j].dct[0][0] = (float)first;
                                continue;
                            }

                            Uint8 readData;
                            wejscie.read((char*) &readData, sizeof(Uint8));
                            bloki[i][j].dct[y][x] = readData;
                        }
                    }

                    DCTDekompresja(&bloki[i][j], bloki[i][j].maxDCT);
                }
            }

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)] = normalizacja(bloki[i][j].dane[y][x] * 65535 / 255, 0, 65535);
                        }
                    }
                }
            }
        }

        break;
    }
    case _RGB888: {
        SDL_Color** obraz = reinterpret_cast<SDL_Color**>(arr);

        if (color == 0) {
            macierz blokiR[n / rozmiarBloku][m / rozmiarBloku];
            macierz blokiG[n / rozmiarBloku][m / rozmiarBloku];
            macierz blokiB[n / rozmiarBloku][m / rozmiarBloku];

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    wejscie.read((char*) &blokiR[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 first;
                                wejscie.read((char*) &first, sizeof(Uint16));
                                blokiR[i][j].dct[0][0] = (float)first;
                                continue;
                            }

                            Uint8 readData;
                            wejscie.read((char*) &readData, sizeof(Uint8));
                            blokiR[i][j].dct[y][x] = readData;
                        }
                    }

                    DCTDekompresja(&blokiR[i][j], blokiR[i][j].maxDCT);
                }
            }

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    wejscie.read((char*) &blokiG[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 first;
                                wejscie.read((char*) &first, sizeof(Uint16));
                                blokiG[i][j].dct[0][0] = (float)first;
                                continue;
                            }

                            Uint8 readData;
                            wejscie.read((char*) &readData, sizeof(Uint8));
                            blokiG[i][j].dct[y][x] = readData;
                        }
                    }

                    DCTDekompresja(&blokiG[i][j], blokiG[i][j].maxDCT);
                }
            }

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    wejscie.read((char*) &blokiB[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 first;
                                wejscie.read((char*) &first, sizeof(Uint16));
                                blokiB[i][j].dct[0][0] = (float)first;
                                continue;
                            }

                            Uint8 readData;
                            wejscie.read((char*) &readData, sizeof(Uint8));
                            blokiB[i][j].dct[y][x] = readData;
                        }
                    }

                    DCTDekompresja(&blokiB[i][j], blokiB[i][j].maxDCT);
                }
            }

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].r = blokiR[i][j].dane[y][x];
                            obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].g = blokiG[i][j].dane[y][x];
                            obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].b = blokiB[i][j].dane[y][x];
                        }
                    }
                }
            }
        } else if (color == 1) {
            macierz bloki[n / rozmiarBloku][m / rozmiarBloku];

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    wejscie.read((char*) &bloki[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 first;
                                wejscie.read((char*) &first, sizeof(Uint16));
                                bloki[i][j].dct[0][0] = (float)first;
                                continue;
                            }

                            Uint8 readData;
                            wejscie.read((char*) &readData, sizeof(Uint8));
                            bloki[i][j].dct[y][x] = readData;
                        }
                    }

                    DCTDekompresja(&bloki[i][j], bloki[i][j].maxDCT);
                }
            }

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].r = bloki[i][j].dane[y][x];
                        }
                    }
                }
            }
        }

        break;
    }
    case _YCbCr888: {
        YCbCr** obraz = reinterpret_cast<YCbCr**>(arr);

        if (color == 0) {
            macierz blokiY[n / rozmiarBloku][m / rozmiarBloku];
            macierz blokiCb[n / rozmiarBloku][m / rozmiarBloku];
            macierz blokiCr[n / rozmiarBloku][m / rozmiarBloku];

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    wejscie.read((char*) &blokiY[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 first;
                                wejscie.read((char*) &first, sizeof(Uint16));
                                blokiY[i][j].dct[0][0] = (float)first;
                                continue;
                            }

                            Uint8 readData;
                            wejscie.read((char*) &readData, sizeof(Uint8));
                            blokiY[i][j].dct[y][x] = readData;
                        }
                    }

                    DCTDekompresja(&blokiY[i][j], blokiY[i][j].maxDCT);
                }
            }

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    wejscie.read((char*) &blokiCb[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 first;
                                wejscie.read((char*) &first, sizeof(Uint16));
                                blokiCb[i][j].dct[0][0] = (float)first;
                                continue;
                            }

                            Uint8 readData;
                            wejscie.read((char*) &readData, sizeof(Uint8));
                            blokiCb[i][j].dct[y][x] = readData;
                        }
                    }

                    DCTDekompresja(&blokiCb[i][j], blokiCb[i][j].maxDCT);
                }
            }

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    wejscie.read((char*) &blokiCr[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 first;
                                wejscie.read((char*) &first, sizeof(Uint16));
                                blokiCr[i][j].dct[0][0] = (float)first;
                                continue;
                            }

                            Uint8 readData;
                            wejscie.read((char*) &readData, sizeof(Uint8));
                            blokiCr[i][j].dct[y][x] = readData;
                        }
                    }

                    DCTDekompresja(&blokiCr[i][j], blokiCr[i][j].maxDCT);
                }
            }

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].y = blokiY[i][j].dane[y][x];
                            obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].cb = blokiCb[i][j].dane[y][x];
                            obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].cr = blokiCr[i][j].dane[y][x];
                        }
                    }
                }
            }
        } else if (color == 1) {
            macierz bloki[n / rozmiarBloku][m / rozmiarBloku];

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    wejscie.read((char*) &bloki[i][j].maxDCT, sizeof(Uint16));

                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            if (y == 0 && x == 0) {
                                Uint16 first;
                                wejscie.read((char*) &first, sizeof(Uint16));
                                bloki[i][j].dct[0][0] = (float)first;
                                continue;
                            }

                            Uint8 readData;
                            wejscie.read((char*) &readData, sizeof(Uint8));
                            bloki[i][j].dct[y][x] = readData;
                        }
                    }

                    DCTDekompresja(&bloki[i][j], bloki[i][j].maxDCT);
                }
            }

            for (int i = 0; i < n / rozmiarBloku; i++) {
                for (int j = 0; j < m / rozmiarBloku; j++) {
                    for (int y = 0; y < rozmiarBloku; y++) {
                        for (int x = 0; x < rozmiarBloku; x++) {
                            obraz[y + (i * rozmiarBloku)][x + (j * rozmiarBloku)].y = bloki[i][j].dane[y][x];
                        }
                    }
                }
            }
        }
    }
    }
}
