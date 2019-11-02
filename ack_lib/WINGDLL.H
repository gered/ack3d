/////////////////////////////////////////////////////////////////////////////
// WINGDLL.H
//
// Interface class for WinG.
/////////////////////////////////////////////////////////////////////////////

#ifndef _INC_WINGDLL
#define _INC_WINGDLL

typedef HDC (WINAPI *wingCreateDC)( void );
typedef BOOL (WINAPI *wingRecommendDIBFormat)( BITMAPINFO FAR *pFormat );
typedef HBITMAP (WINAPI *wingCreateBitmap)( HDC WinGDC, BITMAPINFO const *pHeader,
        void *ppBits );
typedef void FAR *(WINAPI *wingGetDIBPointer)( HBITMAP WinGBitmap,
        BITMAPINFO FAR *pHeader );
typedef UINT (WINAPI *wingGetDIBColorTable)( HDC WinGDC, UINT StartIndex,
        UINT NumberOfEntries, RGBQUAD FAR *pColors );
typedef UINT (WINAPI *wingSetDIBColorTable)( HDC WinGDC, UINT StartIndex,
        UINT NumberOfEntries, RGBQUAD const FAR *pColors );
typedef HPALETTE (WINAPI *wingCreateHalftonePalette)( void );
typedef HBRUSH (WINAPI *wingCreateHalftoneBrush)( HDC, COLORREF,
            WING_DITHER_TYPE);
typedef BOOL (WINAPI *wingBitBlt)( HDC hdcDest, int nXOriginDest,
        int nYOriginDest, int nWidthDest, int nHeightDest, HDC hdcSrc,
        int nXOriginSrc, int nYOriginSrc );
typedef BOOL (WINAPI *wingStretchBlt)( HDC hdcDest, int nXOriginDest,
        int nYOriginDest, int nWidthDest, int nHeightDest, HDC hdcSrc,
        int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc );


class WinGdll {
  
public:
   WinGdll();
   ~WinGdll();

   BOOL Load();
   BOOL Free();

	HINSTANCE m_hLib;
   BOOL m_bLoaded;

      //I would have used m_ prefix, but too laborious to type when using  
      //the class.  Would have inlined the function pointers,
      //without typedef's, but the compiler didn't like it.
      //Anyway, this method makes casting GetProcAddress()
      //easier.
   wingCreateDC               pCreateDC;
   wingRecommendDIBFormat     pRecommendDIBFormat;
   wingCreateBitmap           pCreateBitmap;
   wingGetDIBPointer          pGetDIBPointer;
   wingGetDIBColorTable       pGetDIBColorTable;
   wingSetDIBColorTable       pSetDIBColorTable;
   wingCreateHalftonePalette  pCreateHalftonePalette;
   wingCreateHalftoneBrush    pCreateHalftoneBrush;
   wingBitBlt                 pBitBlt;
   wingStretchBlt             pStretchBlt;
} ;

#endif // _INC_WINGDLL
