// This source file contains the functions needed to read in BBM files.
// (c) 1995 ACK Software (Lary Myers)
//=============================================================================
// This function will return a pointer to a buffer that holds the raw image.
// just free the pointer to delete this buffer. After returning, the array
// colordat will hold the adjusted palette of this pic.
//
// Also, this has been modified to only read in form PBM brushes. form ILBM's
// (the "old" type) are not supported. use the "new" deluxe paint .lbm type
// and do not choose "old".
//=============================================================================
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"
#include "iff.h"

extern  char    rsName[];

unsigned char      colordat[768];  /* maximum it can be...256 colors    */

unsigned char      cplanes[8][80]; /* setting max at 640 pixels width   */
                   /* thats 8 pixels per byte per plane */
unsigned char      *pplanes= &cplanes[0][0];  /* for a form pbm     */

#define MAX_BUF_POS 4096

    int     rdbufpos;
    int     rdSize;
    UCHAR       rdBuffer[MAX_BUF_POS+2];

short ReadNxtBlock(short handle)
{
    short   retlen;

retlen = read(handle,rdBuffer,MAX_BUF_POS);
rdbufpos = 0;

return(retlen);
}

//=============================================================================
//
//=============================================================================
void CloseFile(FILE *fp)
{

fclose(fp);
if (rsHandle)
    {
    rsHandle = open(rsName,O_RDONLY|O_BINARY);
    if (rsHandle < 1)
    rsHandle = 0;
    }

}


unsigned char *AckReadiff(char *picname)
   {
   FILE     *pic;
   short    handle;
   form_chunk   fchunk;
   ChunkHeader  chunk;
   BitMapHeader bmhd;
   long length;
   char value;     /* must remain signed, no matter what. ignore any warnings. */
   short sofar;
   short width,height,planes;
   short pixw;
   unsigned char *destx, *savedestx;

    rdbufpos = MAX_BUF_POS + 1;
    rdSize = MAX_BUF_POS;

    if (!rsHandle)
    {
    if ((pic = fopen(picname,"rb")) == NULL)
        {
        ErrorCode = ERR_BADPICNAME;
        return(0L);
        }
    }
    else
    {
    pic = fdopen(rsHandle,"rb");
    if (pic == NULL)
        {
        ErrorCode = ERR_BADPICNAME;
        return(0L);
        }

    fseek(pic,rbaTable[(ULONG)picname],SEEK_SET);
    }
   fread(&fchunk,1,sizeof(form_chunk),pic); /* read in the first 12 bytes*/

   if (fchunk.type != FORM)
      {
      CloseFile(pic);
        ErrorCode = ERR_INVALIDFORM;
        return(0L);
        }

    if (fchunk.subtype != ID_PBM)
        {
        CloseFile(pic);
        ErrorCode = ERR_NOPBM;
        return(0L);
        }
    /* now lets loop...Because the Chunks can be in any order! */
    while(1)
        {
        fread(&chunk,1,sizeof(ChunkHeader),pic);
        chunk.ckSize = ByteFlipLong(chunk.ckSize);
        if (chunk.ckSize & 1) chunk.ckSize ++;  /* must be word aligned */
        if(chunk.ckID == ID_BMHD)
      {
      fread(&bmhd,1,sizeof(BitMapHeader),pic);
      bmhd.w=iffswab(bmhd.w);          /* the only things we need. */
      bmhd.h=iffswab(bmhd.h);
      destx = (unsigned char *)AckMalloc((bmhd.w * bmhd.h)+4);
      if ( !destx )
         {
         CloseFile(pic);
         ErrorCode = ERR_NOMEMORY;
         return(0L);
         }

      savedestx = destx;

      destx[0] = bmhd.w%256;
      destx[1] = bmhd.w/256;
      destx[2] = bmhd.h%256;
      destx[3] = bmhd.h/256;
      destx += 4;
      continue;
      }
        if(chunk.ckID == ID_CMAP)
      {
      short i;
      unsigned char r,g;

      fread(colordat,1,chunk.ckSize,pic);
      for (i=0;i<768;i++)
            {
             r = colordat[i];     /* r,g do not stand for red and green */
            g = r >> 2;
            colordat[i] = g;
            }
      continue;
      }
        if(chunk.ckID == ID_BODY)
      {
      for(height = 0; height<bmhd.h; height ++)
          {
          unsigned char *dest;
          dest = (unsigned char *)&(pplanes[0]); /* point at first char  */
          sofar = bmhd.w;               /* number of bytes = 8  */
          if (sofar&1) sofar++;
          while (sofar)
        {
        if (bmhd.compression)
            {
            value=fgetc(pic);     /* get the next byte    */
            if (value > 0)
                {
                short len;
                len = value +1;
                sofar -= len;
                if(!(fread(dest,len,1,pic)))
              {
              CloseFile(pic);
              ErrorCode = ERR_BADPICFILE;
              AckFree(savedestx);
              return(0L);
              }
                dest +=len;
                }
            else
                {
                short count;
                count = -value; /* get amount to dup */
                count ++;
                sofar -= count;
                value=fgetc(pic);
                while (--count >= 0) *dest++ = value;
                }
            }
        else
            {
            fread(dest,sofar,1,pic); /* just throw on plane */
            sofar = 0;
            }
        }
          if (sofar < 0)
        {
        CloseFile(pic);
        }
          _fmemcpy(destx,pplanes,bmhd.w);
          destx += bmhd.w;
          }
      break; /* leave if we've unpacked the BODY*/
      }

        fseek(pic,chunk.ckSize,SEEK_CUR);
        }

    CloseFile(pic);
    return((char *)savedestx);
    }

long ByteFlipLong(long NUMBER)
   {
   /* Hey, I didn;t write this function!!! */
   long Y, T;
   short I;

   T = NUMBER;
   Y=0;for (I=0;I<4;I++){Y = Y | (T & 0xFF);if (I<3) {Y = Y << 8;T = T >> 8;}}
   return(Y);
   }

short iffswab(unsigned short number)
   {
   unsigned short xx1,xx2;
   unsigned short result;

   xx1 = number <<8; xx2 = number >>8; result = xx1|xx2;
   return(result);
   }
// **** End of Source ****

