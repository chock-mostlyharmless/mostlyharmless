//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008                                        //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <mmsystem.h>
#include "../intro.h"
#include "../config.h"

static const PIXELFORMATDESCRIPTOR pfd =
    {
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA,
    32,
    0, 0, 0, 0, 0, 0, 8, 0,
    0, 0, 0, 0, 0,
    32, 0, 0,
    PFD_MAIN_PLANE,
    0, 0, 0, 0
    };

#ifdef SETRESOLUTION
#pragma data_seg(".screenSettings")
static DEVMODE screenSettings = { {0},
    #if _MSC_VER < 1400
    0,0,148,0,0x001c0000,{0},0,0,0,0,0,0,0,0,0,{0},0,32,XRES,YRES,0,0,      // Visual C++ 6.0
    #else
    0,0,156,0,0x001c0000,{0},0,0,0,0,0,{0},0,32,XRES,YRES,{0}, 0,           // Visuatl Studio 2005
    #endif
    #if(WINVER >= 0x0400)
    0,0,0,0,0,0,
    #if (WINVER >= 0x0500) || (_WIN32_WINNT >= 0x0400)
    0,0
    #endif
    #endif
    };
#endif

void entrypoint( void )
{              
    // full screen
    #ifdef SETRESOLUTION
    if( ChangeDisplaySettings(&screenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL) return;
    ShowCursor( 0 );
    #endif
    // create window
    HWND hWnd = CreateWindow( "static",0,WS_POPUP|WS_VISIBLE|WS_MAXIMIZE,0,0,0,0,0,0,0,0);
    HDC hDC = GetDC(hWnd);
    // initalize opengl
    if( !SetPixelFormat(hDC,ChoosePixelFormat(hDC,&pfd),&pfd) ) return;
    HGLRC hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC,hRC);

	// init intro
	intro_init();

    long t;
	long to = timeGetTime();
    do 
	{
		//ShowCursor(false);

        t = timeGetTime(); 
        //if( !to ) to=t; 
        t = t-to;//-150;
    
        intro_do( t );
    
        //SwapBuffers ( hDC );   
        wglSwapLayerBuffers( hDC, WGL_SWAP_MAIN_PLANE );
	}while ( !GetAsyncKeyState(VK_ESCAPE) );

    sndPlaySound(0,0);

    ExitProcess(0);
}

