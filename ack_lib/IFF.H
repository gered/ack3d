#ifndef IFF_H_INCLUDED
#define IFF_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#define LONG unsigned long
#define ULONG unsigned long
#define UBYTE unsigned char
#define UWORD unsigned short
#define DWORD unsigned long

long ByteFlipLong(long);
UCHAR *Readiff(char *);  /* this returns back a bitmap with a picture in it*/

typedef LONG ID;  /* An ID is four printable ASCII chars like FORM or DPPV */

/* original
#define MakeID(d,c,b,a) ((DWORD)(a)<<24 | (DWORD)(b)<<16 | (DWORD)(c)<<8 | (DWORD)(d) )*/
#define MakeID(d,c,b,a) (((DWORD)(a)<<24) | ((DWORD)(b)<<16) | ((DWORD)(c)<<8) | ((DWORD)(d)))

#define FORM MakeID('F','O','R','M')
#define PROP MakeID('P','R','O','P')
#define LIST MakeID('L','I','S','T')
#define CAT  MakeID('C','A','T',' ')
#define FILLER MakeID(' ',' ',' ',' ')

typedef struct
    {
    ID    type;
    long  cksize;
    ID    subtype;
    } form_chunk;

typedef struct {
    ID     ckID;
    LONG  ckSize;
    } ChunkHeader;

typedef struct {
    ID     ckID;
    LONG  ckSize;
    UBYTE ckData[ 1 /*REALLY: ckSize*/ ];
    } Chunk;

#define ID_PBM  MakeID('P','B','M',' ')
#define ID_ILBM MakeID('I','L','B','M')
#define ID_BMHD MakeID('B','M','H','D')
#define ID_CMAP MakeID('C','M','A','P')
#define ID_GRAB MakeID('G','R','A','B')
#define ID_DEST MakeID('D','E','S','T')
#define ID_SPRT MakeID('S','P','R','T')
#define ID_CAMG MakeID('C','A','M','G')
#define ID_BODY MakeID('B','O','D','Y')

/* ---------- BitMapHeader ---------------------------------------------*/

typedef UBYTE Masking;     /* Choice of masking technique.*/
#define mskNone                 0
#define mskHasMask              1
#define mskHasTransparentColor  2
#define mskLasso                3

#define cmpNone      0
#define cmpByteRun1  1

/* A BitMapHeader is stored in a BMHD chunk. */
typedef struct {
    UWORD w, h;         /* raster width & height in pixels */
    UWORD  x, y;         /* position for this image */
    UBYTE nPlanes;      /* # source bitplanes */
    UBYTE masking;    /* masking technique */
    UBYTE compression;  /* compression algoithm */
    UBYTE pad1;     /* UNUSED.  For consistency, put 0 here.*/
    UWORD transparentColor;   /* transparent "color number" */
    UBYTE xAspect, yAspect;   /* aspect ratio, a rational number x/y */
    UWORD  pageWidth, pageHeight;  /* source "page" size in pixels */
    } BitMapHeader;
/* RowBytes computes the number of bytes in a row, from the width in pixels.*/
#define RowBytes(w)   (((w) + 15) >> 4 << 1)

/* A CMAP chunk is a packed array of ColorRegisters (3 bytes each). */
typedef struct {
    UBYTE red, green, blue;   /* MUST be UBYTEs so ">> 4" won't sign extend.*/
    } ColorRegister;

/* Use this constant instead of sizeof(ColorRegister). */
#define sizeofColorRegister  3

long ByteFlipLong(long);
void ByteFlipShort(short *);
short iffswab(unsigned short);
short swab(unsigned short);

#ifdef __cplusplus
};
#endif

#endif

