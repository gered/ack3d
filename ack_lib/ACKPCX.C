// This source file contains the functions needed to read in PCX files.
// (c) 1995 ACK Software (Lary Myers)
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

//typedef unsigned short USHORT;

#include "ack3d.h"      // Main ACK-3D internal and interface data structures
#include "ackeng.h"     // Internal data structures and constants
#include "ackext.h"     // Defines external (global) variables

typedef struct
{
  char  manufacturer;    /* Always set to 0 */
  char  version;     /* Always 5 for 256-color files */
  char  encoding;    /* Always set to 1 */
  char  bits_per_pixel;  /* Should be 8 for 256-color files */
  short xmin,ymin;   /* Coordinates for top left corner */
  short xmax,ymax;   /* Width and height of image */
  short hres;        /* Horizontal resolution of image */
  short vres;        /* Vertical resolution of image */
  char  palette16[48];   /* EGA palette; not used for 256-color files */
  char  reserved;    /* Reserved for future use */
  char  color_planes;    /* Color planes */
  short bytes_per_line;  /* Number of bytes in 1 line of pixels */
  short palette_type;    /* Should be 2 for color palette */
  char  filler[58];  /* Nothing but junk */
} PcxHeader;

typedef struct
{
    PcxHeader hdr;
    UCHAR   *bitmap;
    UCHAR   pal[768];
    unsigned short imagebytes,width,height;
} PcxFile;

#define PCX_MAX_SIZE 64000L
enum {PCX_OK,PCX_NOMEM,PCX_TOOBIG,PCX_NOFILE};

enum {NORMAL,RLE};
enum {FALSE,TRUE};

    PcxFile pcxGlobal;       // data structure for reading PCX files

extern unsigned char colordat[];

//
//  This routine loads a 256 color PCX file.
//
unsigned char *AckReadPCX(char *filename)
{
    long i;
    int mode=NORMAL,nbytes;
    UCHAR abyte,*p;
    FILE *f;
    PcxFile *pcx;

pcx = &pcxGlobal;

if (!rsHandle)
    {
    f=fopen(filename,"rb");
    if (f==NULL)
    {
    ErrorCode = ERR_BADFILE;
    return NULL;
    }
    }
else
    {
    f = fdopen (rsHandle, "rb");
    if (f == NULL)
    {
    ErrorCode = ERR_BADPICNAME;
    return (0L);
    }

    fseek (f, rbaTable[(ULONG) filename], SEEK_SET);
    }


fread(&pcx->hdr,sizeof(PcxHeader),1,f);
pcx->width=1+pcx->hdr.xmax-pcx->hdr.xmin;
pcx->height=1+pcx->hdr.ymax-pcx->hdr.ymin;
pcx->imagebytes=(unsigned int)(pcx->width*pcx->height);

if (pcx->imagebytes > PCX_MAX_SIZE)
    {
    if (!rsHandle)
    fclose(f);
    ErrorCode = ERR_INVALIDFORM;
    return(NULL);
    }

pcx->bitmap=(UCHAR*)AckMalloc(pcx->imagebytes+4);

if (pcx->bitmap == NULL)
    {
    if (!rsHandle)
    fclose(f);
    ErrorCode = ERR_NOMEMORY;
    return(NULL);
    }
p=&pcx->bitmap[4];

for (i=0;i<pcx->imagebytes;i++)
    {
    if(mode == NORMAL)
    {
    abyte=fgetc(f);
    if (abyte > 0xbf)
        {
        nbytes=abyte & 0x3f;
        abyte=fgetc(f);
        if (--nbytes > 0)
        mode=RLE;
        }
    }
    else if (--nbytes == 0)
        mode=NORMAL;
    *p++=abyte;
    }

fseek(f,-768L,SEEK_END);      // get palette from pcx file
fread(colordat,768,1,f);
p=colordat;
for (i=0;i<768;i++)        // bit shift palette
    *p++=*p >>2;

if (!rsHandle)
    fclose(f);

p = pcx->bitmap;
(*(short *)p) = pcx->width;
p += sizeof(short);
(*(short *)p) = pcx->height;

return(pcx->bitmap);         // return success
}

// **** End of Source ****

