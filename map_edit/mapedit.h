/* Header file for ACK-3D Map Editor */

#ifndef MAPEDIT_H_INCLUDED
#define MAPEDIT_H_INCLUDED

#define CURRENT_SQUAREX     161
#define CURRENT_SQUAREY     4

#define PASS_X          243
#define PASS_Y          2

#define SCREEN_COLOR        20

#define BLACK           0
#define LIGHTBLUE       64
#define BLUE            69
#define LIGHTMAGENTA        112
#define MAGENTA         119
#define RED         40
#define LIGHTRED        32
#define GREEN           88
#define LIGHTGREEN      80
#define YELLOW          120

typedef struct {
    short     x;
    short     y;
    short     x1;
    short     y1;
} RECT;

typedef struct {
    short     BoxX;
    short     BoxY;
    char    *FileName;
    char    DoBeep;
    short     NumButtons;
    RECT    ButtonCoords[4];
} BOXES;

#define BOX_ALREADY_1_OBJECT    0
#define BOX_NEW_WARNING     1
#define BOX_MODIFIED_WARNING    2
#define BOX_SAVED       3
#define BOX_ALREADY_START_CODE  4
#define BOX_MAX_SPECIAL_CODE    5

/* Prototypes */
void SetVmode(short);
void SetPalette(unsigned char *);
char *AddExtent(char *,char *);
unsigned short inkey(void);
void SetVGAmode(void);
void SetTextMode(void);

#endif

