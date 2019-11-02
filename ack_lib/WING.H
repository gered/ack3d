/*****************************************************************************\
*
* wing.h - WinG functions, types, and definitions
*
*          Copyright (c) 1994 Microsoft Corp. All rights reserved.
*
\*****************************************************************************/

#ifndef _INC_WING
#define _INC_WING

#ifndef _INC_WINDOWS
#include <windows.h>    /* Include windows.h if not already included */
#endif

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif

#if defined(WIN32) || defined(_WIN32)
#define WINGAPI WINAPI
#else
#define WINGAPI WINAPI _loadds
#endif


/***** WingDC and WinGBitmap *************************************************/

HDC WINGAPI WinGCreateDC( void );

BOOL WINGAPI WinGRecommendDIBFormat( BITMAPINFO FAR *pFormat );

HBITMAP WINGAPI WinGCreateBitmap( HDC WinGDC, BITMAPINFO const FAR *pHeader,
        void FAR *FAR *ppBits );

void FAR *WINGAPI WinGGetDIBPointer( HBITMAP WinGBitmap,
        BITMAPINFO FAR *pHeader );

UINT WINGAPI WinGGetDIBColorTable( HDC WinGDC, UINT StartIndex,
        UINT NumberOfEntries, RGBQUAD FAR *pColors );

UINT WINGAPI WinGSetDIBColorTable( HDC WinGDC, UINT StartIndex,
        UINT NumberOfEntries, RGBQUAD const FAR *pColors );


/***** Halftoning ************************************************************/

HPALETTE WINGAPI WinGCreateHalftonePalette( void );

typedef enum WING_DITHER_TYPE
{
    WING_DISPERSED_4x4,
    WING_DISPERSED_8x8,

    WING_CLUSTERED_4x4

} WING_DITHER_TYPE;

HBRUSH WINGAPI WinGCreateHalftoneBrush( HDC Context, COLORREF crColor,
        WING_DITHER_TYPE DitherType );


/***** Blts ******************************************************************/

BOOL WINGAPI WinGBitBlt( HDC hdcDest, int nXOriginDest,
        int nYOriginDest, int nWidthDest, int nHeightDest, HDC hdcSrc,
        int nXOriginSrc, int nYOriginSrc );

BOOL WINGAPI WinGStretchBlt( HDC hdcDest, int nXOriginDest,
        int nYOriginDest, int nWidthDest, int nHeightDest, HDC hdcSrc,
        int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc );

#ifdef __cplusplus
}                       /* End of extern "C" */
#endif

#endif // _INC_WING

