
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <io.h>
#include <string.h>
#include <fcntl.h>
#include <sys\stat.h>

#define MAX_RBA     500



    unsigned    long    rTable[MAX_RBA+1];

//=============================================================================
// for dealing with paths
//=============================================================================

char dataPath[200] = "";
char tempFilename[200] = "";

void FindFilePaths(const char *dataFile) {
    char *end;

    if (dataFile) {
        end = strrchr(dataFile, '\\');
        if (end)
            strncpy(dataPath, dataFile, ((end+1) - dataFile));
    }
}

char* GetCombinedPath(const char *base, const char *file) {
    strncpy(tempFilename, base, 200);
    strncat(tempFilename, file, 200);
    return tempFilename;
}


/****************************************************************************
**                                     **
****************************************************************************/
char *StripEndOfLine(char *s)
{
    int     len;
    char    ch;

len = strlen(s);

while (--len >= 0)
    {
    ch = s[len];
    if (ch != ' ' && ch != ';' && ch != '\t' && ch != 13 && ch != 10)
    break;

    s[len] = '\0';
    }

return(s);
}



/******************************************************************************
*                                         *
******************************************************************************/
int main(int argc,char *argv[])
{
        int InHandle,OutHandle;
        int hNum;
        long    rba;
    unsigned    int hLen,len;
    unsigned    char    *xferbuf;
        FILE    *fp;
        char    dbuf[200];

printf("\nResource file builder Version 1.0\n");
printf("Copyright 1993 Lary Myers\n");

if (argc < 3)
    {
    printf("Syntax: BPIC datafile.ext outfile.ext\n");
    printf(" Where: datafile.ext is an ASCII file containing filenames\n");
    printf("        to be compiled into the outfile.ext resource file.\n");
    return(1);
    }

FindFilePaths(argv[1]);

xferbuf = malloc(64000);
if (xferbuf == NULL)
    {
    printf("Unable to get memory to run.\n");
    return(1);
    }

fp = fopen(argv[1],"rt");
if (fp == NULL)
    {
    printf("Unable to open datafile %s\n",argv[1]);
    return(1);
    }

OutHandle = open(argv[2],O_RDWR|O_BINARY|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
if (OutHandle < 1)
    {
    printf("Error creating outfile %s\n",argv[2]);
    fclose(fp);
    return(1);
    }

hLen = MAX_RBA * sizeof(long);
memset(rTable,0,hLen);

write(OutHandle,rTable,hLen);
hNum = 0;
rba = hLen;

while (1)
    {
    if (feof(fp))
    break;

    *dbuf = '\0';
    fgets(dbuf,198,fp);
    if (*dbuf == ';')
    continue;

    StripEndOfLine(dbuf);

    if (!strlen(dbuf))
    continue;

    printf("Processing....%s\n",dbuf);
    GetCombinedPath(dataPath, dbuf);
    InHandle = open(tempFilename,O_RDWR|O_BINARY);

    if (InHandle < 1)
    {
    printf("Error opening file %s - Number %d\n",dbuf,hNum);
    break;
    }

    rTable[hNum++] = rba;


    while (1)
    {
    len = read(InHandle,xferbuf,64000U);
    if (len)
        {
        if (write(OutHandle,xferbuf,len) != len)
        {
        printf("Error writing to resource file. Check disk space.\n");
        break;
        }
        rba += len;
        }

    if (len < 64000)
        break;

    }

    close(InHandle);
    }

rTable[hNum] = rba;

lseek(OutHandle,0L,SEEK_SET);
write(OutHandle,rTable,hLen);
close(OutHandle);
fclose(fp);

printf("Processing complete.\n");

return(0);
}

