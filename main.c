#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VSTUP "zavodnici.txt"
#define VYSTUP "vysledkova_listina.txt"
#define VELIKOST_BUFFERU 200
#define ODDELOVACE " "

typedef struct {
    int minuty;
    int sekundy;
    int setiny;
} CAS;

typedef struct {
    int cislo;
    char prijmeni[20];
    char jmeno[15];
    CAS casKolo1;
    CAS casKolo2;
} ZAVODNICE;

/* Z načteného řetězce vyextrahuje čas v minutách, sekundách a setinách */
CAS casNaCislo(const char *text) {
    CAS cas;
    sscanf(text, "%d:%d.%d", &cas.minuty, &cas.sekundy, &cas.setiny);
    return cas;
}

/* Zjistí počet řádku, díky kterému se dynamicky vytvoří pole závodnic */
int zjistiPocetRadku(const char *soubor) {
    FILE *fr = fopen(soubor, "r");
    int pocetRadku = 0;
    char znak;
    char buffer[VELIKOST_BUFFERU];
    if (fr == NULL) {
        printf("Chyba pri otevirani souboru %s.\n", soubor);
        exit(EXIT_FAILURE);
    }
    /* Přeskočí záhlaví */
    fgets(buffer, VELIKOST_BUFFERU, fr);
    while ((znak = (char) fgetc(fr)) != EOF) {
        if (znak == '\n') {
            pocetRadku++;
        }
    }
    /* Z důvodu posledního enteru */
    pocetRadku++;
    fclose(fr);
    return pocetRadku;
}

/* Načte data ze souboru pole struktur */
void nactiData(ZAVODNICE *zavodnice, const char *soubor) {
    FILE *fr = fopen(soubor, "r");
    char buffer[VELIKOST_BUFFERU];
    int i = 0;
    char *pch;

    if (fr == NULL) {
        printf("Chyba pri otevirani souboru %s.\n", soubor);
        exit(EXIT_FAILURE);
    }
    /* Přeskočí záhlaví */
    fgets(buffer, VELIKOST_BUFFERU, fr);
    while (fgets(buffer, VELIKOST_BUFFERU, fr) != NULL) {
        pch = strtok(buffer, ODDELOVACE);
        int radek = 0;
        while (pch != NULL) {
            radek++;
            switch (radek) {
                case 1:
                    zavodnice[i].cislo = atoi(pch);
                    break;
                case 2:
                    strcpy(zavodnice[i].prijmeni, pch);
                    break;
                case 3:
                    strcpy(zavodnice[i].jmeno, pch);
                    break;
                case 4:
                    zavodnice[i].casKolo1 = casNaCislo(pch);
                    break;
                case 5:
                    zavodnice[i].casKolo2 = casNaCislo(pch);
                    break;
                default:
                    break;
            }
            pch = strtok(NULL, ODDELOVACE);
        }
        i++;
    }
    fclose(fr);
}

/* Vypíše tabulku do konzole */
void vypisDoKonzole(ZAVODNICE *zavodnice, int pocetZavodnic) {
    printf("S T A R T O V N I  L I S T I N A  -  S L A L O M  Z E N Y\n");
    printf("---------------------------------------------------------\n");
    printf("startovni cislo|  prijmeni  |  jmeno |cas 1. kola|cas 2. kola\n");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < pocetZavodnic; i++) {
        printf("             %2d|%12s|%8s|    %d:%02d.%02d|    %d:%02d.%02d\n",
               zavodnice[i].cislo, zavodnice[i].prijmeni,
               zavodnice[i].jmeno, zavodnice[i].casKolo1.minuty,
               zavodnice[i].casKolo1.sekundy, zavodnice[i].casKolo1.setiny,
               zavodnice[i].casKolo2.minuty, zavodnice[i].casKolo2.sekundy,
               zavodnice[i].casKolo2.setiny);
    }
    printf("-------------------------------------------------------------\n");
}

/* Vypočítá celkový součet času */
int celkovySoucetCasu(ZAVODNICE zavodnice) {
    return (zavodnice.casKolo1.minuty * 6000 +
            zavodnice.casKolo1.sekundy * 100 +
            zavodnice.casKolo1.setiny) +
           (zavodnice.casKolo2.minuty * 6000 +
            zavodnice.casKolo2.sekundy * 100 +
            zavodnice.casKolo2.setiny);
}
/* Vypočítá ztrátu závodnic */
int ztrata(ZAVODNICE zavodnice, int celkovyCasNejrychlejsi) {
    int celkovyCasZavodnice = celkovySoucetCasu(zavodnice);
    return celkovyCasZavodnice - celkovyCasNejrychlejsi;
}
/* Převede ztrátu na text */
void ztrataNaText(ZAVODNICE zavodnice, int celkovyCasNejrychlejsi, char *text) {
    int ztrataCas = ztrata(zavodnice, celkovyCasNejrychlejsi);
    sprintf(text, "+%02d:%02d.%02d", ztrataCas / 6000, (ztrataCas % 6000) / 100, ztrataCas % 100);
}
/* Převede celkový čas na text */
void casCelkemNaText(ZAVODNICE zavodnice, char *text) {
    int celkovyCas = celkovySoucetCasu(zavodnice);
    sprintf(text, "%02d:%02d.%02d", celkovyCas / 6000, (celkovyCas % 6000) / 100, celkovyCas % 100);
}
/* Prohodí 2 struktury závodnice */
void prohod(ZAVODNICE *zav1, ZAVODNICE *zav2) {
    ZAVODNICE zavPom = *zav1;
    *zav1 = *zav2;
    *zav2 = zavPom;
}
/* "Bubble sort" - metoda třídění pole */
void bubbleSort(ZAVODNICE *zavodnice, int pocetZavodnic) {
    for (int i = 0; i < pocetZavodnic - 1; i++) {
        for (int j = 0; j < pocetZavodnic - i - 1; j++) {
            if (celkovySoucetCasu(zavodnice[j]) > celkovySoucetCasu(zavodnice[j + 1])) {
                prohod(&zavodnice[j], &zavodnice[j + 1]);
            }
        }
    }
}
/* Vypíše tabulku do souboru */
void vypisDoSouboru(ZAVODNICE *zavodnice, const char *soubor, int pocetZavodnic) {
    bubbleSort(zavodnice, pocetZavodnic);
    FILE *fw = fopen(soubor, "w");
    int poradi = 0;
    char celkemCas[15];
    char ztrata[15];

    if (fw == NULL) {
        printf("Chyba pri otevirani souboru %s.\n", soubor);
        exit(EXIT_FAILURE);
    }
    printf("Byl vytvoren soubor %s.\n", soubor);
    fprintf(fw, "V Y S L E D K O V A  L I S T I N A\n");
    fprintf(fw, "----------------------------------\n");
    fprintf(fw, "poradi|cislo|  prijmeni  |  jmeno | 1.kolo| 2.kolo| celkem |   ztrata\n");
    fprintf(fw, "---------------------------------------------------------------------\n");

    /* Nejrychlejší závodnice */
    int celkovyCasNejrychlejsi = celkovySoucetCasu(zavodnice[0]);

    for (int i = 0; i < pocetZavodnic; i++) {
        casCelkemNaText(zavodnice[i], celkemCas);
        ztrataNaText(zavodnice[i], celkovyCasNejrychlejsi, ztrata);
        fprintf(fw, "    %2d|   %2d|%12s|%8s|%d:%02d.%02d|%d:%02d.%02d|%s|%s\n", ++poradi, zavodnice[i].cislo,
                zavodnice[i].prijmeni, zavodnice[i].jmeno,
                zavodnice[i].casKolo1.minuty, zavodnice[i].casKolo1.sekundy, zavodnice[i].casKolo1.setiny,
                zavodnice[i].casKolo2.minuty, zavodnice[i].casKolo2.sekundy, zavodnice[i].casKolo2.setiny,
                celkemCas, i == 0 ? "" : ztrata);
    }
    fclose(fw);
}

int main() {
    /* Zjistí počet závodnic (řádků) v souboru*/
    int pocetZavodnic = zjistiPocetRadku(VSTUP);
    /* Dynamicky vytvoří pole závodnic na základě proměnné `pocetZavodnic` */
    ZAVODNICE *zavodnice = malloc(pocetZavodnic * sizeof(ZAVODNICE));
    /* Načte data ze vstupního souboru*/
    nactiData(zavodnice, VSTUP);
    /* Vypíše tabulku závodnic do konzole */
    vypisDoKonzole(zavodnice, pocetZavodnic);
    /* Zapíše tabulku závodnic do souboru */
    vypisDoSouboru(zavodnice, VYSTUP, pocetZavodnic);
    /* Uvolnění dynamicky alokované paměti */
    free(zavodnice);
    zavodnice == NULL;
    return 0;
}
