/*****************************************************************************
 ACKGIF.C


#include <malloc.h>

 Routine to load a 256 color .GIF file into a memory buffer.  *Only* 256
 color images are supported here!  Sorry, no routines to SAVE .GIFs...
 Memory required is allocated on the fly.

 Mark Morley
 morley@camosun.bc.ca

    Modified for use in the ACK environment by Lary Myers

*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <bios.h>
#include <fcntl.h>
#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

#define MAX_CODES     4096

extern unsigned char    colordat[]; // Palette buffer in AckReadiff module


static FILE*          fp;
static short          curr_size;
static short          clear;
static short          ending;
static short          newcodes;
static short          top_slot;
static short          slot;
static short          navail_bytes = 0;
static short          nbits_left = 0;
static unsigned char  b1;
static unsigned char  byte_buff[257];
static unsigned char* pbytes;
static unsigned char* stack;
static unsigned char* suffix;
static unsigned short*  prefix;

static unsigned long code_mask[13] =
{
   0L,
   0x0001L, 0x0003L,
   0x0007L, 0x000FL,
   0x001FL, 0x003FL,
   0x007FL, 0x00FFL,
   0x01FFL, 0x03FFL,
   0x07FFL, 0x0FFFL
};

//=============================================================================
//
//=============================================================================
static short get_next_code(void)
{
    short  i;
    static unsigned long ret;

if (!nbits_left)
    {
    if (navail_bytes <= 0)
    {
    pbytes = byte_buff;
    navail_bytes = getc(fp);

    if (navail_bytes)
        {
        for (i = 0; i < navail_bytes; ++i )
        *(byte_buff + i) = getc( fp );
        }
    }
    b1 = *pbytes++;
    nbits_left = 8;
    --navail_bytes;
    }

ret = b1 >> (8 - nbits_left);
while (curr_size > nbits_left)
    {
    if (navail_bytes <= 0)
    {
    pbytes = byte_buff;
    navail_bytes = getc(fp);
    if (navail_bytes)
        {
        for( i = 0; i < navail_bytes; ++i )
           *(byte_buff + i) = getc( fp );
        }
    }
    b1 = *pbytes++;
    ret |= b1 << nbits_left;
    nbits_left += 8;
    --navail_bytes;
    }

nbits_left -= curr_size;

return((short)(ret & *(code_mask + curr_size)));
}


//=============================================================================
//
//=============================================================================
unsigned char *AckReadgif(char *picname)
{
    unsigned char   *sp;
    unsigned char   *buffer,*OrgBuffer;
    short       code, fc, oc;
    short       i;
    unsigned char   size;
    short       c;
    unsigned short  wt,ht;
    int         bSize;
    unsigned char   buf[1028];
    unsigned char   red;
    unsigned char   grn;
    unsigned char   blu;


if (!rsHandle)
    {
    fp = fopen(picname,"rb");
    if( !fp )
    {
    ErrorCode = ERR_BADFILE;
    return(NULL);
    }
    }
else
    {
    fp = fdopen(rsHandle,"rb");
    if (fp == NULL)
    {
    ErrorCode = ERR_BADPICNAME;
    return(0L);
    }

    fseek(fp,rbaTable[(ULONG)picname],SEEK_SET);
    }

fread(buf,1,6,fp);
if( strncmp( (char*)buf, "GIF", 3 ) )
    {
    if (!rsHandle)
    fclose(fp);

    ErrorCode = ERR_INVALIDFORM;
    return(NULL);
    }

fread(buf,1,7,fp);
for (i = 0; i < 768;)
    {
    red = getc(fp);
    grn = getc(fp);
    blu = getc(fp);
    colordat[i++] = red >> 2;
    colordat[i++] = grn >> 2;
    colordat[i++] = blu >> 2;
    }

fread(buf,1,5,fp);
fread(buf,1,2,fp);
wt = (*(short *)buf);
fread(buf,1,2,fp);
ht = (*(short *)buf);
//wt = getw(fp);
//ht = getw(fp);
bSize = (ht * wt) + (sizeof(short) * 2);
buffer = (UCHAR *)AckMalloc(bSize);
if (buffer == NULL)
    {
    if (!rsHandle)
    fclose(fp);

    ErrorCode = ERR_NOMEMORY;
    return(NULL);
    }

OrgBuffer = buffer;

(*(short *)buffer) = wt;
buffer += sizeof(short);
(*(short *)buffer) = ht;
buffer += sizeof(short);

fread(buf,1,1,fp);
size = getc(fp);

if (size < 2 || 9 < size)
    {
    if (!rsHandle)
    fclose(fp);
    AckFree(OrgBuffer);
    ErrorCode = ERR_INVALIDFORM;
    return(NULL);
    }

stack = (unsigned char *) AckMalloc(MAX_CODES + 1);
suffix = (unsigned char *) AckMalloc(MAX_CODES + 1);
prefix = (unsigned short *) AckMalloc(sizeof(short) * (MAX_CODES + 1));

if (stack == NULL || suffix == NULL || prefix == NULL)
    {
    if (!rsHandle)
    fclose(fp);
    AckFree(OrgBuffer);
    ErrorCode = ERR_NOMEMORY;
    return(NULL);
    }

curr_size = size + 1;
top_slot = 1 << curr_size;
clear = 1 << size;
ending = clear + 1;
slot = newcodes = ending + 1;
navail_bytes = nbits_left = 0;
oc = fc = 0;
sp = stack;

while ( (c = get_next_code()) != ending )
    {
    if (c == clear)
    {
    curr_size = size + 1;
    slot = newcodes;
    top_slot = 1 << curr_size;
    while ( (c = get_next_code()) == clear );

    if( c == ending )
        break;

    if( c >= slot )
        c = 0;

    oc = fc = c;
    *buffer++ = c;
    }
    else
    {
    code = c;
    if (code >= slot)
        {
        code = oc;
        *sp++ = fc;
        }

    while (code >= newcodes)
        {
        *sp++ = *(suffix + code);
        code = *(prefix + code);
        }

    *sp++ = code;
    if (slot < top_slot)
        {
        *(suffix + slot) = fc = code;
        *(prefix + slot++) = oc;
        oc = c;
        }

    if (slot >= top_slot && curr_size < 12)
        {
        top_slot <<= 1;
        ++curr_size;
        }

    while (sp > stack)
        {
        --sp;
        *buffer++ = *sp;
        }
    }
    }

AckFree(stack);
AckFree(suffix);
AckFree(prefix);

if (!rsHandle)
    fclose(fp);

return(OrgBuffer);
}
