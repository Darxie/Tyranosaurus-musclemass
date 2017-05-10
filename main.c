#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <afxres.h>

#define MAX 30

#pragma pack(push, 1)
struct BitmapFileHeader {
    unsigned short bfType;
    unsigned long bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned long bfOffBits;
};
#pragma pack(pop)

struct BitmapInfoHeader {
    unsigned long biSize;
    long int biWidth;
    long int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned long biCompression;
    unsigned long biSizeImage;
    long int biXPelsPerMeter;
    long int biYPelsPerMeter;
    unsigned long biClrUsed;
    unsigned long biClrImportant;
};

void write_head(FILE *f, int width, int height) {
    if (width % 4 != 0 || height % 4 != 0) {
        printf("Chyba: Vyska a sirka nie su delitelne 4.\n");
        return;
    }
    struct BitmapInfoHeader bih;
    bih.biSize = sizeof(struct BitmapInfoHeader);
    bih.biWidth = width;
    bih.biHeight = height;
    bih.biSizeImage = bih.biWidth * bih.biHeight * 3;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = 0;
    bih.biXPelsPerMeter = 0;
    bih.biYPelsPerMeter = 0;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;

    struct BitmapFileHeader bfh;
    bfh.bfType = 0x4D42;
    bfh.bfSize = sizeof(struct BitmapFileHeader) + sizeof(struct BitmapInfoHeader) + bih.biSizeImage;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits = sizeof(struct BitmapFileHeader) + bih.biSize;

    fwrite(&bfh, sizeof(struct BitmapFileHeader), 1, f);
    fwrite(&bih, sizeof(struct BitmapInfoHeader), 1, f);
}

void write_pixel(FILE *f, unsigned char r, unsigned char g, unsigned char b) {
    fwrite(&r, 1, 1, f);
    fwrite(&g, 1, 1, f);
    fwrite(&b, 1, 1, f);
}

char **grid;
int width = 0, height = 0;
int color[205][205];
char map[205][205];
int rgb[255][3];


int hladaj(int r, int s, int c) {
    if (r < 0 || r >= MAX || s < 0 || s >= MAX || grid[r][s] != '.' || color[r][s]) {
        return 0;
    }
    color[r][s] = c;
    hladaj(r + 1, s, c);
    hladaj(r - 1, s, c);
    hladaj(r, s + 1, c);
    hladaj(r, s - 1, c);
    return 1;
}

char **printGrid(char **grid, int vys, int sir) {
    int i, j, q, x;
    printf("pecen\n");
    char **bip = (char **) malloc(vys * 4 * sizeof(char *));
    for (i = 0; i < vys * 4; i++) {
        bip[i] = (char *) malloc(sir * 4 * sizeof(char));
        for (j = 0; j < sir * 4; j++) {
            bip[i][j] = 0;
        }
    }


    for (i = 0; i < vys; i++) {
        for (j = 0; j < sir; j++) {
            printf("%c", grid[i][j]);
            for (q = 0; q < 4; q++)
                for (x = 0; x < 4; x++) {
                    bip[i * 4 + q][j * 4 + x] = grid[i][j];
                }
        }
        printf("\n");
    }
    return bip;
}


// ukazkove kreslenie BMP obrazku
void obrazok(char *nazov_suboru) {

    FILE *f = fopen(nazov_suboru, "wb");
//ToDo premenna na 4
    int w = 4 * width, h = 4 * height;
    int i = 0, j = 0;
    int c = color[i][j];
    write_head(f, w, h);
    for (i = 0; i < 255; i++) {
        rgb[i][0] = rand() % 256;
        rgb[i][1] = rand() % 256;
        rgb[i][2] = rand() % 256;
    }
    int k = 1;
    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++)
            if (hladaj(i, j, k))
                k++;
    }
    char **bip = printGrid(grid, width, height);
    printf("TU\n");
    int x, y;
    for (y = h - 1; y > -1; y--)
        for (x = 0; x < w; x++) {
            //write_pixel(f, 0, 0, 255);
            switch (bip[y][x]) {
                case 'T':
                    write_pixel(f, 0, 0, 255);
                    break;
                case 'X':
                    write_pixel(f, 255, 0, 0);
                    break;
                case '#':
                    write_pixel(f, 30, 30, 30);
                    break;
                case '.':
                    write_pixel(f, 255, 255, 255);
                    break;
                default:
                    if ((bip[y][x] >= 'A' && bip[y][x] <= 'Z') || (bip[y][x] >= 'a' && bip[y][x] <= 'z')) {
                        write_pixel(f, 128, 128, 128);
                    } else {
                        write_pixel(f, rgb[bip[y][x]][0], rgb[bip[y][x]][1], rgb[bip[y][x]][2]);
                    }
                    break;
            }
        }
    fclose(f);
}


//int width, height;

// vsetky metody pre node

struct Node {
    struct Node *next;
    int x, y;
    char room;
};

void printNode(struct Node n) {
    printf("x: %d,y: %d,room: %c", n.x, n.y, n.room);
}

void printNodeLenDvere(struct Node n) {
    printf("%c", n.room);
}

// vsetky metody pre queue

struct Queue {
    struct Node *first;
    int size;
};

void initq(struct Queue *q) {
    q->first = NULL;
    q->size = 0;
}

void push(struct Queue *q, int x, int y, char room) {
    struct Node *a = (struct Node *) malloc(sizeof(struct Node));
    a->next = NULL;
    a->x = x;
    a->y = y;
    a->room = room;
    if (q->size == 0) { //ak je prazdna
        q->first = a;
    } else { //musim preskakat na posledny
        struct Node *n = q->first;

        while (n->next != NULL) { //
            n = n->next;
        }
        n->next = a; //dam ho na koniec
    }
    q->size = q->size + 1; //zvysim size o 1
}

struct Node pop(struct Queue *q) {
    struct Node node; //vytiahne prvy node z
    node.x = 0;
    node.y = 0;
    node.room = 0;

    if (q->size > 0) {
        struct Node *n = q->first;
        node.x = n->x;
        node.y = n->y;
        node.room = n->room;
        q->first = n->next;
        free(n);
        q->size = q->size - 1;
    }
    return node;
}

void printQueue(struct Queue q) {
    struct Node *n = q.first;
    printf(" size: %d, ", q.size);
    while (n != NULL) {
        printNode(*n);
        printf(", ");
        n = n->next;
    }
    printf("\n");
}

void printQueueLenDvere(struct Queue q) {
    struct Node *n = q.first;
    printf(" size: %d, ", q.size);
    while (n != NULL) {
        printNodeLenDvere(*n);
        printf(", ");
        n = n->next;
    }
    printf("\n");
}

//pomocne funkcie ktore neviem ci budes potrebovat

int nacitajVstup() {
    int i = 0;
    while (i < MAX) {
        int j = scanf("%30s\n", grid[i]);
        if (j <= 0) {
            return 0;
        }
        if (strlen(grid[i]) >= width)
            width = strlen(grid[i]);
        height++;
        i++;
    }
    return 0;
}

void init() {
    grid = (char **) malloc(sizeof(char *) * MAX);
    int i, j;
    for (i = 0; i < MAX; i++) {
        grid[i] = (char *) malloc(sizeof(char) * MAX);
    }
    nacitajVstup();
}



//funkcie pre rekurzivne prehladavanie

int mozemTuByt(char **lab, int vys, int sir, int x, int y) {
    if (y < 0 || x < 0) {
        return -1;
    }

    if (x >= sir || y >= vys) {
        return -1;
    }

    char tmp = lab[y][x];
    if (tmp == '.' || (tmp >= 'A' && tmp <= 'Z')) {
        return 0;
    }

    return -1;
}

int dvere(char **lab, int vys, int sir, int x, int y, int posX, int posY) {

    if (mozemTuByt(lab, vys, sir, x + posX, y + posY) == -1) {
        return -1;
    }

    if (lab[y][x] >= 'A' && lab[y][x] <= 'Z') {
        return 0;
    } else {
        return -1;
    }
}

struct Queue najdiDvere(char **lab, int vys, int sir) {
    struct Queue q;
    int x, y;
    initq(&q);
    for (y = 0; y < vys; y++) {
        for (x = 0; x < sir; x++) {
            char tmp = lab[y][x];
            if (tmp >= 'A' && tmp <= 'Z') {
                push(&q, x, y, tmp);
            }
        }
    }
    return q;
}

//rekurzivne funkcie

int chod(char **lab, int vys, int sir, int x, int y, char room, struct Queue *q) {
    int done = -1;

    if (mozemTuByt(lab, vys, sir, x, y) == 0) {
        if (dvere(lab, vys, sir, x, y, 0, 0) == -1) {
            lab[y][x] = room; //color
        } else {
            char a = lab[y][x] - 'A';
            lab[y][x] = 'a' + a;
        }

        if (dvere(lab, vys, sir, x - 1, y, -1, 0) == 0) {
            push(q, x - 1, y, lab[y][x - 1]);
        } else {
            chod(lab, vys, sir, x - 1, y, room, q);
            done = 0; //zaznacim aby som nespravil krok spat
        }

        if (dvere(lab, vys, sir, x, y - 1, 0, -1) == 0) {
            push(q, x, y - 1, lab[y - 1][x]);
        } else {
            chod(lab, vys, sir, x, y - 1, room, q);
            done = 0;
        }

        if (dvere(lab, vys, sir, x + 1, y, 1, 0) == 0) {
            push(q, x + 1, y, lab[y][x + 1]);
        } else {
            chod(lab, vys, sir, x + 1, y, room, q);
            done = 0;
        }

        if (dvere(lab, vys, sir, x, y + 1, 0, 1) == 0) {
            push(q, x, y + 1, lab[y + 1][x]);
        } else {
            chod(lab, vys, sir, x, y + 1, room, q);
            done = 0;
        }
    }

    return done;
}

int chod2(char **lab, int vys, int sir, int x, int y, struct Queue *q) {
    if (mozemTuByt(lab, vys, sir, x, y) == 0) {
        if (dvere(lab, vys, sir, x, y, 0, 0) == 0) {
            push(q, x, y, lab[y][x]);
            //printGrid(lab,vys,sir);
        } else {
            lab[y][x] = '?'; //keby som nedal otaznik tak idem doprava a potom aj dolava a zaznacujem si ze tam nejdem
            chod2(lab, vys, sir, x - 1, y, q);
            chod2(lab, vys, sir, x, y - 1, q);
            chod2(lab, vys, sir, x + 1, y, q);
            chod2(lab, vys, sir, x, y + 1, q);
            lab[y][x] = '.'; //vraciam naspat do povodneho stavu
        }
    }
    return 0;
}

int chod3(char **lab, int vys, int sir, int x, int y, int n) {
    if (n < 0)
        return 0;
    if (mozemTuByt(lab, vys, sir, x, y) == 0) {
        int m = n; //n-ko je pocet, kolko ostava dveri cez ktore moze prejst, dam si to do m a ak su to dvere tak to zmensim,
        if (dvere(lab, vys, sir, x, y, 0, 0) == 0) {
            m--;
        } else {
            lab[y][x] = '1';
        }
        chod3(lab, vys, sir, x - 1, y, m);
        chod3(lab, vys, sir, x, y - 1, m);
        chod3(lab, vys, sir, x + 1, y, m);
        chod3(lab, vys, sir, x, y + 1, m);
        //lab[y][x] = '.';
    }
    return 0;
}

//funkcie pre zadanie
//uloha 2

void vyfarbiMiestnosti(char **lab, int vys, int sir) { //panacik vrati zoznam vsetkych dveri ktore uvidel,
    //pomocny zoznam
    struct Queue q;
    initq(&q);
    push(&q, 1, 0, 'T');

    int i = 0;
    while (q.size != 0) { //vykonavam kym sa tam nieco nachadza
        struct Node n = pop(&q); //vytiahnem jedny dvere
        if (chod(lab, vys, sir, n.x, n.y, '1' + i, &q) ==
            0) { //zavolam funkciu chod s parametrami - lab labyrintu, n.x dvere, 1 - flag, icko sa zvysuje, na konci queue keby nahodou narazil na dalsie dvere, tak nech ich ma
            i++;
        }
    }
    printGrid(lab, vys, sir); //nakresli do suboru
}

//uloha 3

void statistika(char **lab, int vys, int sir) {
    struct Queue q = najdiDvere(lab, vys, sir); //prebehne grid, zaznaci do queue a vrati ju
    while (q.size != 0) { //vyjde von a on pochodi cely priestor a zaznaci si vsetky dvere, ktore su tam
        struct Node n = pop(&q);
        struct Queue dvere;
        initq(&dvere);
        lab[n.y][n.x] = '.';
        chod2(lab, vys, sir, n.x, n.y, &dvere);
        lab[n.y][n.x] = n.room;
        printf("%c : ", n.room);
        printQueueLenDvere(dvere);
    }
}

//uloha 4

void vyfarbiNkrokov(char **lab, int vys, int sir, int n) {
    chod3(lab, vys, sir, 1, 0, n); //ak nahodou prejde cez dvere, tak zmensi
    printGrid(lab, vys, sir);
}

int main() {
    printf("\n");
    printf("PECENJENAJLEPSI\n");
    init();//toto nam vytvori vstup
    //printGrid(grid, width, height);
    //char **a = printGrid(grid, width, height);
    //printGrid(a, width*10, height*10);
    obrazok("obrazok.bmp");
    //uloha 2
    //vyfarbiMiestnosti(grid, width, height);
    //obrazok("miestnosti.bmp");
    //uloha 3
    //obrazok("obrazok.bmp");
    //init(); // premazavam to grid
    //statistika(grid, MAX, MAX);
    //3 uloha neposkodi grid takze netreba nic mazat
    vyfarbiNkrokov(grid, width, height, 2);
    obrazok("kroky.bmp");

    // v podstate jedine co mas spravit je nahradit funkciu printGrid za nakresli do subora.
    // neviem mozno bude treba aj destruktor pre queue
    return 0;
}