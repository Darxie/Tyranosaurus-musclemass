#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 30
#define PREM 16

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
  bih.biSizeImage = (unsigned long) (bih.biWidth * bih.biHeight * 3);
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

//globalne premenne
char **grid;
int width = 0, height = 0;
int rgb[255][3];

//kreslenie BMP obrazku
char **printGrid(char **grid, int vys, int sir) {
  int i, j, q, x;
  char **bitmap = (char **) malloc(vys * PREM * sizeof(char *));
  for (i = 0; i < vys * PREM; i++) {
    bitmap[i] = (char *) malloc(sir * PREM * sizeof(char));
    for (j = 0; j < sir * PREM; j++) {
      bitmap[i][j] = 0;
    }
  }

  for (i = 0; i < vys; i++) {
    for (j = 0; j < sir; j++) {
      printf("%c", grid[i][j]);
      for (q = 0; q < PREM; q++)
        for (x = 0; x < PREM; x++) {
          bitmap[i * PREM + q][j * PREM + x] = grid[i][j];
        }
    }
    printf("\n");
  }
  return bitmap;
}

void print(char *nazov_suboru) {

  FILE *f = fopen(nazov_suboru, "wb");
  int w = PREM * width, h = PREM * height;
  int i = 0;
  write_head(f, w, h);
  for (i = 0; i < 255; i++) {
    rgb[i][0] = rand() % 256;
    rgb[i][1] = rand() % 256;
    rgb[i][2] = rand() % 256;
  }

  char **bitmap = printGrid(grid, width, height);
  int x, y;
  for (y = h - 1; y > -1; y--)
    for (x = 0; x < w; x++) {
      switch (bitmap[y][x]) {
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
          if ((bitmap[y][x] >= 'A' && bitmap[y][x] <= 'Z') || (bitmap[y][x] >= 'a' && bitmap[y][x] <= 'z')) {
            write_pixel(f, 128, 128, 128);
          } else {
            write_pixel(f, (unsigned char) rgb[bitmap[y][x]][0], (unsigned char) rgb[bitmap[y][x]][1],
                        (unsigned char) rgb[bitmap[y][x]][2]);
          }
          break;
      }
    }
  fclose(f);
}

// vsetky metody pre node
struct Node {
    struct Node *next;
    int x, y;
    char room;
};

void printNode(struct Node n) {
  printf("x: %d,y: %d,room: %c", n.x, n.y, n.room);
}

void printNodeOnlyDoor(struct Node n) {
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

void prientQueueOnlyDoor(struct Queue q) {
  struct Node *n = q.first;
  //(" size: %d, ", q.size);
  while (n != NULL) {
    printNodeOnlyDoor(*n);
    printf(", ");
    n = n->next;
  }
  printf("\n");
}

//pomocne funkcie vstupu
int loadStdin() {
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
  int i;
  for (i = 0; i < MAX; i++) {
    grid[i] = (char *) malloc(sizeof(char) * MAX);
  }
  loadStdin();
  print("obrazok.bmp");
}

//funkcie pre rekurzivne prehladavanie
int canMoveHere(char **lab, int height, int width, int x, int y) {
  if (y < 0 || x < 0) {
    return -1;
  }

  if (x >= width || y >= height) {
    return -1;
  }

  char tmp = lab[y][x];
  if (tmp == '.' || (tmp >= 'A' && tmp <= 'Z')) {
    return 0;
  }

  return -1;
}

int door(char **lab, int height, int width, int x, int y, int posX, int posY) {

  if (canMoveHere(lab, height, width, x + posX, y + posY) == -1) {
    return -1;
  }

  if (lab[y][x] >= 'A' && lab[y][x] <= 'Z') {
    return 0;
  } else {
    return -1;
  }
}

struct Queue findDoor(char **lab, int height, int width) {
  struct Queue q;
  int x, y;
  initq(&q);
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      char tmp = lab[y][x];
      if (tmp >= 'A' && tmp <= 'Z') {
        push(&q, x, y, tmp);
      }
    }
  }
  return q;
}

//rekurzivne funkcie
int go(char **lab, int height, int width, int x, int y, char room, struct Queue *q) {
  int done = -1;

  if (canMoveHere(lab, height, width, x, y) == 0) {
    if (door(lab, height, width, x, y, 0, 0) == -1) {
      lab[y][x] = room; //color
    } else {
      char a = (char) (lab[y][x] - 'A');
      lab[y][x] = (char) ('a' + a);
    }

    if (door(lab, height, width, x - 1, y, -1, 0) == 0) {
      push(q, x - 1, y, lab[y][x - 1]);
    } else {
      go(lab, height, width, x - 1, y, room, q);
      done = 0; //zaznacim aby som nespravil krok spat
    }

    if (door(lab, height, width, x, y - 1, 0, -1) == 0) {
      push(q, x, y - 1, lab[y - 1][x]);
    } else {
      go(lab, height, width, x, y - 1, room, q);
      done = 0;
    }

    if (door(lab, height, width, x + 1, y, 1, 0) == 0) {
      push(q, x + 1, y, lab[y][x + 1]);
    } else {
      go(lab, height, width, x + 1, y, room, q);
      done = 0;
    }

    if (door(lab, height, width, x, y + 1, 0, 1) == 0) {
      push(q, x, y + 1, lab[y + 1][x]);
    } else {
      go(lab, height, width, x, y + 1, room, q);
      done = 0;
    }
  }

  return done;
}

int go2(char **lab, int height, int width, int x, int y, struct Queue *q) {
  if (canMoveHere(lab, height, width, x, y) == 0) {
    if (door(lab, height, width, x, y, 0, 0) == 0) {
      push(q, x, y, lab[y][x]);
    } else {
      lab[y][x] = '?'; //keby som nedal otaznik tak idem doprava a potom aj dolava a zaznacujem si ze tam nejdem
      go2(lab, height, width, x - 1, y, q);
      go2(lab, height, width, x, y - 1, q);
      go2(lab, height, width, x + 1, y, q);
      go2(lab, height, width, x, y + 1, q);
      lab[y][x] = '.'; //vraciam naspat do povodneho stavu
    }
  }
  return 0;
}

int go3(char **lab, int height, int width, int x, int y, int n) {
  if (n < 0)
    return 0;
  if (canMoveHere(lab, height, width, x, y) == 0) {
    int m = n; //n-ko je pocet, kolko ostava dveri cez ktore moze prejst, dam si to do m a ak su to door tak to zmensim,
    if (door(lab, height, width, x, y, 0, 0) == 0) {
      m--;
    } else {
      lab[y][x] = '1';
    }
    go3(lab, height, width, x - 1, y, m);
    go3(lab, height, width, x, y - 1, m);
    go3(lab, height, width, x + 1, y, m);
    go3(lab, height, width, x, y + 1, m);
  }
  return 0;
}

//funkcie pre zadanie
void kresli_miestnosti(char **lab, int height, int width) { //panacik vrati zoznam vsetkych dveri ktore uvidel,
  //pomocny zoznam
  struct Queue q;
  initq(&q);
  push(&q, 1, 0, 'T');
  int i = 0;
  while (q.size != 0) { //vykonavam kym sa tam nieco nachadza
    struct Node n = pop(&q); //vytiahnem jedny door
    if (go(lab, height, width, n.x, n.y, (char) ('1' + i), &q) ==
        0) { //zavolam funkciu go s parametrami - lab labyrintu, n.x dvere, 1 - flag, icko sa zvysuje, na konci queue keby nahodou narazil na dalsie door, tak nech ich ma
      i++;
    }
  }
  print("miestnosti.bmp"); //print do suboru
}

void vypis_susednost(char **lab, int height, int width) {
  struct Queue q = findDoor(lab, height, width); //prebehne grid, zaznaci do queue a vrati ju
  while (q.size != 0) { //vyjde von a on pochodi cely priestor a zaznaci si vsetky door, ktore su tam
    struct Node n = pop(&q);
    struct Queue door;
    initq(&door);
    lab[n.y][n.x] = '.';
    go2(lab, height, width, n.x, n.y, &door);
    lab[n.y][n.x] = n.room;
    printf("%c : ", n.room);
    prientQueueOnlyDoor(door);
  }
}

void kresli_mapu(char **lab, int height, int width, int n) {
  go3(lab, height, width, 1, 0, n); //ak nahodou prejde cez door, tak zmensi
  print("kroky.bmp");
}

int main() {
  init();
  vypis_susednost(grid, width, height);
  //kresli_miestnosti(grid, width, height);
  kresli_mapu(grid, width, height, 3);
  return 0;
}