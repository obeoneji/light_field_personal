/* $Id: //depot/povray/3.5/windows/pvclient.h#2 $ */

#ifndef _PVCLIENT_H_
#define _PVCLIENT_H_

#define IBS_HORZCAPTION    0x4000L
#define IBS_VERTCAPTION    0x8000L
#define HTNEWBUTTON        127
#define HTCLOSEBUTTON      126
#define CM_MAXIMINIMIZE    8000
#define PVNC_MAXIMINIMIZE  127
#define CAPTIONXY          (GetSystemMetrics (SM_CYCAPTION) / 2 + 1)

UINT    WINAPI ibGetCaptionSize (HWND hWnd ) ;
UINT    WINAPI ibSetCaptionSize (HWND hWnd, UINT nSize) ;
LRESULT WINAPI ibDefWindowProc (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) ;
VOID WINAPI ibAdjustWindowRect (LPRECT lprc, UINT flags, UINT nCapSize) ;
VOID WINAPI ibAdjustWindowRectHWND (HWND hWnd, LPRECT lprc) ;
UINT    WINAPI pvncGetCaptionSize (HWND hWnd ) ;
UINT    WINAPI pvncSetCaptionSize (HWND hWnd, UINT nSize) ;
LRESULT WINAPI pvncDefWindowProc (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) ;
LRESULT WINAPI pvncDefDlgProc (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) ;
LRESULT WINAPI pvnc95DefWindowProc (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) ;
LRESULT WINAPI pvnc95DefDlgProc (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) ;
LRESULT WINAPI ncDefWindowProc (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) ;
VOID WINAPI pvncAdjustWindowRect (LPRECT lprc, UINT flags, UINT nCapSize) ;
VOID WINAPI pvncAdjustWindowRectHWND (HWND hWnd, LPRECT lprc) ;
BOOL NEAR PASCAL pvncGetStatusbarClient (HWND hWnd, RECT *rect, HFONT *font) ;

/////////////////////////////////////////////////////////////////////
// Little known fact:
//    ExtTextOut() is the fastest way to draw a filled rectangle
//    in Windows (if you don't want dithered colors or borders).
//
//    The following macro (DRAWFASTRECT) draws a filled rectangle
//    with no border and a solid color.  It uses the current back-
//    ground color as the fill color.
//////////////////////////////////////////////////////////////////////

#define DRAWFASTRECT(hdc,lprc) ExtTextOut(hdc,0,0,ETO_OPAQUE,lprc,NULL,0,NULL)

typedef LRESULT (WINAPI *defWP) (HWND, UINT, WPARAM, LPARAM) ;

#endif


