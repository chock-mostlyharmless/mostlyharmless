// LiveCoding.cpp : Definiert den Einstiegspunkt f�r die Anwendung.
//

#include "stdafx.h"
#include "LiveCoding.h"
#include "Configuration.h"
#include "glext.h"
#include "GLNames.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "FluidSimulation.h"
#include "Editor.h"
#include "Parameter.h"
#include "bass.h"

#define MAX_LOADSTRING 100
#define BLOB_FADE_SPEED 1.0f

// The used effect (will be changeable later on)
#define NUM_USED_PROGRAMS 10
char *usedShader[NUM_USED_PROGRAMS] = {"empty.jlsl", "vp1.jlsl", "vp2.jlsl", "vp3.jlsl", "vp4.jlsl", "vp5.jlsl", "vp6.jlsl", "vp7.jlsl", "vp8.jlsl", "vp9.jlsl"};
char *usedProgram[NUM_USED_PROGRAMS] = {"empty.gprg", "vp1.gprg", "vp2.gprg", "vp3.gprg", "vp4.gprg", "vp5.gprg", "vp6.gprg", "vp7.gprg", "vp8.gprg", "vp9.gprg"};
int usedIndex = 0;
float aspectRatio = (float)XRES / (float)YRES;

float music_start_time_ = -10000.0f;

long start_time_ = 0;
FluidSimulation fluid_simulation_;
HSTREAM mp3Str;

/*************************************************
 * GL Core variables
 *************************************************/
GenFP glFP[NUM_GL_NAMES]; // pointer to openGL functions
const static char* glnames[NUM_GL_NAMES]={
     "glCreateShader", "glCreateProgram", "glShaderSource", "glCompileShader", 
     "glAttachShader", "glLinkProgram", "glUseProgram",
	 "glTexImage3D", "glGetShaderiv","glGetShaderInfoLog",
	 "glDeleteProgram", "glDeleteShader",
	 "glActiveTexture", "glGetUniformLocation", "glUniform1i", "glUniform1f",
	 "glMultiTexCoord2f"
};

/*************************************************
 * The core managing units that hold the resources
 *************************************************/
ShaderManager shaderManager;
TextureManager textureManager;
Editor editor;

/*************************************************
 * Window core variables
 *************************************************/
HINSTANCE hInst;								// Aktuelle Instanz
TCHAR szTitle[MAX_LOADSTRING];					// Titelleistentext
TCHAR szWindowClass[MAX_LOADSTRING];			// Klassenname des Hauptfensters
// The size of the window that we render to...
RECT windowRect;

typedef struct
{
    //---------------
    HINSTANCE   hInstance;
    HDC         hDC;
    HGLRC       hRC;
    HWND        hWnd;
    //---------------
    int         full;
    //---------------
    char        wndclass[4];	// window class and title :)
    //---------------
}WININFO;

static const PIXELFORMATDESCRIPTOR pfd =
    {
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA,
    24,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,  // accum
    0,             // zbuffer
    0,              // stencil!
    0,              // aux
    PFD_MAIN_PLANE,
    0, 0, 0, 0
    };

static WININFO wininfo = {  0,0,0,0,0,
							{'l','c','_',0}
                            };

/**************************************************
 * Parameters from the midi stuff
 ****************************************************/
// ---------------------------------------------------------------
//					Parameter interpolation stuff
// ------------------------------------------------------------
// I want to interpolate the new values from the old ones.
const int maxNumParameters = 25;
const static float defaultParameters[maxNumParameters] = 
{
	-1.0f, -1.0f,
	0.0f, 0.0f,	0.0f, 0.0f, 0.0f,	// 2-6 ~= 1-5
	-1.0f,
	0.0f, 0.0f,						// 8,9 ~= 6,7
	-1.0f, -1.0f,
	0.0f, 0.0f,						// 12,13 ~= 8-9
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,			// 14-22 ~= 1b-9b
};
float interpolatedParameters[maxNumParameters];
const int NUM_KEYS = 127;
static int keyPressed[NUM_KEYS] = {0};

// BPM stuff
const int NUM_BEAT_TIMES = 7;
float BPM = 0.0f;
int beatDurations[NUM_BEAT_TIMES] = {900, 1200, 1100, 1000, 1400, 1000, 1000};
int lastBeatTime = 0;
float blob = 0.;



/*************************************************
 * OpenGL initialization
 *************************************************/
static int initGL(WININFO *winInfo)
{
	char errorString[MAX_ERROR_LENGTH + 1];

	// Create openGL functions
	for (int i=0; i<NUM_GL_NAMES; i++) glFP[i] = (GenFP)wglGetProcAddress(glnames[i]);

	// Create and initialize the shader manager
	if (shaderManager.init(errorString))
	{
		MessageBox(winInfo->hWnd, errorString, "Shader Manager Load", MB_OK);
		return -1;
	}

	// Create and initialize everything needed for texture Management
	if (textureManager.init(errorString))
	{
		MessageBox(winInfo->hWnd, errorString, "Texture Manager Load", MB_OK);
		return -1;
	}

	// Create the text editor
	if (editor.init(&shaderManager, &textureManager, errorString))
	{
		MessageBox(winInfo->hWnd, errorString, "Editor init", MB_OK);
		return -1;
	}
	blob = 0.;

    // Create the fluid simulation stuff
    fluid_simulation_.Init(false);

	return 0;
}

/*************************************************
 * Windows callback
 *************************************************/
static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    int time = timeGetTime() - start_time_;
    float ftime = 0.001f * time;

	// salvapantallas
	if( uMsg==WM_SYSCOMMAND && (wParam==SC_SCREENSAVE || wParam==SC_MONITORPOWER) )
		return( 0 );

	// boton x o pulsacion de escape
	//if( uMsg==WM_CLOSE || uMsg==WM_DESTROY || (uMsg==WM_KEYDOWN && wParam==VK_ESCAPE) )
	if( uMsg==WM_CLOSE || uMsg==WM_DESTROY )
		{
		PostQuitMessage(0);
        return( 0 );
		}

	// Reaction to command keys
    if( uMsg==WM_KEYDOWN )
    {
		switch (wParam)
		{
#if 0
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
#endif

		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_END:
		case VK_HOME:
			break;

		case VK_RETURN:
		case VK_DELETE:
		case VK_BACK:
			break;

        case 'q':
        case 'Q':
            music_start_time_ = 0.001f * (timeGetTime() - start_time_);
            // start music playback
#ifdef MUSIC
            BASS_Init(-1, 44100, 0, hWnd, NULL);
            mp3Str = BASS_StreamCreateFile(FALSE, "Goethe_music.mp3", 0, 0, 0);
            BASS_ChannelPlay(mp3Str, TRUE);
            BASS_Start();
#endif
            break;
        case 'w':
        case 'W':
            music_start_time_ = -10000.0f;
#ifdef MUSIC
            BASS_Stop();
            BASS_ChannelStop(mp3Str);
            BASS_StreamFree(mp3Str);
            BASS_Free();
#endif
            break;

        case 'a':
        case 'A':
            fluid_simulation_.request_set_points_ = true;
            break;

        case 'o':
        case 'O':
            fluid_simulation_.Init(false);
            break;

        case 'p':
        case 'P':
            fluid_simulation_.Init(true);
            break;

        case 'x':
        case 'X':
            fluid_simulation_.PushApart();

        case 'm':
        case 'M':
            if (GetAsyncKeyState(VK_CONTROL) < 0) {
                // TODO: Minimization again.
                SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
                ShowWindow(hWnd, SW_MAXIMIZE);
                GetClientRect(hWnd, &windowRect);
                glViewport(0, 0, windowRect.right - windowRect.left, abs(windowRect.bottom - windowRect.top)); //NEW
                aspectRatio = (float)(windowRect.right - windowRect.left) / (float)(abs(windowRect.bottom - windowRect.top));
                ShowCursor(false);
            }
            break;

		default:
			break;
		}
    }

    if (uMsg == WM_KEYUP) {
        switch(wParam) {
        default:
            break;
        }
    }

    return( DefWindowProc(hWnd,uMsg,wParam,lParam) );
}


static void window_end( WININFO *info )
{
    if( info->hRC )
    {
        wglMakeCurrent( 0, 0 );
        wglDeleteContext( info->hRC );
    }

    if( info->hDC  ) ReleaseDC( info->hWnd, info->hDC );
    if( info->hWnd ) DestroyWindow( info->hWnd );

    UnregisterClass( info->wndclass, info->hInstance );

    if( info->full )
    {
        ChangeDisplaySettings( 0, 0 );
		ShowCursor( 1 );
    }
}


static int window_init( WININFO *info )
{
	unsigned int	PixelFormat;
    DWORD			dwExStyle, dwStyle;
    DEVMODE			dmScreenSettings;
	RECT			rec;

    WNDCLASS		wc;

    ZeroMemory( &wc, sizeof(WNDCLASS) );
    wc.style         = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = info->hInstance;
    wc.lpszClassName = info->wndclass;
	
    if( !RegisterClass(&wc) )
        return( 0 );

    if( info->full )
    {
        dmScreenSettings.dmSize       = sizeof(DEVMODE);
        dmScreenSettings.dmFields     = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
        dmScreenSettings.dmBitsPerPel = 24;
        dmScreenSettings.dmPelsWidth  = XRES;
        dmScreenSettings.dmPelsHeight = YRES;
        if( ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
            return( 0 );
        dwExStyle = WS_EX_APPWINDOW;
        dwStyle   = WS_VISIBLE | WS_POPUP;// | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ShowCursor( 0 );
    }
    else
    {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        dwStyle   = WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
    }

    rec.left   = 0;
    rec.top    = 0;
    rec.right  = XRES;
    rec.bottom = YRES;
    AdjustWindowRect( &rec, dwStyle, 0 );
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = XRES;
	windowRect.bottom = YRES;

    info->hWnd = CreateWindowEx( dwExStyle, wc.lpszClassName, "live coding", dwStyle,
                               (GetSystemMetrics(SM_CXSCREEN)-rec.right+rec.left)>>1,
                               (GetSystemMetrics(SM_CYSCREEN)-rec.bottom+rec.top)>>1,
                               rec.right-rec.left, rec.bottom-rec.top, 0, 0, info->hInstance, 0 );
    if( !info->hWnd )
        return( 0 );

    if( !(info->hDC=GetDC(info->hWnd)) )
        return( 0 );

    if( !(PixelFormat=ChoosePixelFormat(info->hDC,&pfd)) )
        return( 0 );

    if( !SetPixelFormat(info->hDC,PixelFormat,&pfd) )
        return( 0 );

    if( !(info->hRC=wglCreateContext(info->hDC)) )
        return( 0 );

    if( !wglMakeCurrent(info->hDC,info->hRC) )
        return( 0 );
    
    return( 1 );
}

void DrawMusic(float ftime) {
    const int kNumEdges = 8;
    float delta_angle = 3.141592f * 2.0f / kNumEdges;
    const float music_beat = 0.405f;
    const float rotation_speed = 0.05f;

    // Initialize OGL stuff
    GLuint textureID;
    GLuint programID;
    char errorText[MAX_ERROR_LENGTH + 1];
    shaderManager.getProgramID("SimpleTexture.gprg", &programID, errorText);
    glUseProgram(programID);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);

    // Just a 5-edge thingie
    for (int person = 0; person < 2; person++) {
        //float divergence = person * -sinf(ftime * 3.1415f / 160.0f * 8.0f);
        //if (divergence < 0.0f) divergence = 0.0f;
        float divergence = person * sinf(ftime * 3.1415f / 160.0f * 7.5f);
        divergence *= divergence;
        if (ftime < 20.0f) divergence = 0.0f;

        for (int edge = 0; edge < kNumEdges; edge++) {
            float start_angle = edge * delta_angle;
            int next_edge = edge + 1;
            if (edge == kNumEdges - 1) next_edge = 0;
            float end_angle = next_edge * delta_angle;

            float beat_overdrive = sinf(ftime * 3.1415f / 160.0f) * 2.0f + .5f;
            float rotation = rotation_speed * 3.1415f * 2.0f * ftime;
            float beat = sinf(ftime * 3.1415f * 2.0f / music_beat);
            //beat *= beat * beat;
            rotation += rotation_speed * beat * music_beat * beat_overdrive;
            start_angle -= rotation;
            end_angle -= rotation;

            float start_line[2] = { cosf(start_angle), sinf(start_angle) };
            float end_line[2] = { cosf(end_angle), sinf(end_angle) };

            float star_amount = 0.2f * sinf(ftime * 3.1415f / 160.0f);
            float inner_dist_start = 0.27f + star_amount * sinf(0.2f * ftime + 3.1415f * 2.0f * edge / kNumEdges * 10 + divergence*star_amount);
            float inner_dist_end = 0.27f + star_amount * sinf(0.2f * ftime + 3.1415f * 2.0f * next_edge / kNumEdges * 10 + divergence*star_amount);
            float outer_dist_start = 0.35f + star_amount * sinf(0.2f * ftime + 3.1415f * 2.0f * edge / kNumEdges * 10 + divergence);
            float outer_dist_end = 0.35f + star_amount * sinf(0.2f * ftime + 3.1415f * 2.0f * next_edge / kNumEdges * 10 + divergence);

            float size_overdrive = sinf(ftime * 3.1415f / 160.0f) * 2.0f + .4f;
            float size = 1.0f + 0.2f * size_overdrive +
                size_overdrive * (0.25f * sinf(ftime * 0.4f + divergence * 3.14f) + 0.15f * sinf(ftime * 0.25f + divergence * 3.14f));
            inner_dist_start *= size;
            inner_dist_end *= size;
            outer_dist_start *= size;
            outer_dist_end *= size;

            glVertex2f(start_line[0] * inner_dist_start, start_line[1] * inner_dist_start * aspectRatio);
            glVertex2f(start_line[0] * outer_dist_start, start_line[1] * outer_dist_start * aspectRatio);
            glVertex2f(end_line[0] * outer_dist_end, end_line[1] * outer_dist_end * aspectRatio);
            glVertex2f(end_line[0] * inner_dist_end, end_line[1] * inner_dist_end * aspectRatio);
        }
    }

    glEnd();
}

void intro_do(long t, long delta_time)
{
	char errorText[MAX_ERROR_LENGTH+1];
	float ftime = 0.001f*(float)t;
    float fdelta_time = 0.001f * (float)(delta_time);
	GLuint textureID;

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	// Those are key-Press indicators. I only act on 0-to-1.
	for (int i = 0; i < maxNumParameters; i++)
	{
		interpolatedParameters[i] = 0.95f * interpolatedParameters[i] +
									0.05f * params.getParam(i, defaultParameters[i]);
	}
	// Update key press events.
	for (int i = 0; i < NUM_KEYS; i++)
	{
		if (params.getParam(i, 0.0) > 0.5f) keyPressed[i]++;
		else keyPressed[i] = 0;
	}

	// BPM => spike calculation
	float BPS = BPM / 60.0f;
	float jumpsPerSecond = BPS / 1.0f; // Jump on every fourth beat.
	static float phase = 0.0f;
	float jumpTime = (ftime * jumpsPerSecond) + phase;
	jumpTime -= (float)floor(jumpTime);
	if (keyPressed[41] == 1)
	{
		phase -= jumpTime;
		jumpTime = 0.0f;
		if (phase < 0.0f) phase += 1.0;
	}
	jumpTime = jumpTime * jumpTime;
	// spike is between 0.0 and 1.0 depending on the position within whatever.
	float spike = 0.5f * cosf(jumpTime * 3.1415926f * 1.5f) + 0.5f;
	// blob is growing down from 1. after keypress
	static float lastFTime = 0.f;
	blob *= (float)exp(-(float)(ftime - lastFTime) * BLOB_FADE_SPEED);
	lastFTime = ftime;

	// Set the program uniforms
	GLuint programID;
	shaderManager.getProgramID(usedProgram[usedIndex], &programID, errorText);
	glUseProgram(programID);
	GLuint loc = glGetUniformLocation(programID, "aspectRatio");
	glUniform1f(loc, aspectRatio);
	loc = glGetUniformLocation(programID, "time");
	glUniform1f(loc, (float)(t * 0.001f));
	// For now I am just sending the spike to the shader. I might need something better...
	loc = glGetUniformLocation(programID, "spike");
	glUniform1f(loc, spike);
	loc = glGetUniformLocation(programID, "blob");
	glUniform1f(loc, blob);
	loc = glGetUniformLocation(programID, "knob1");
	glUniform1f(loc, interpolatedParameters[14]);
	loc = glGetUniformLocation(programID, "knob2");
	glUniform1f(loc, interpolatedParameters[15]);
	loc = glGetUniformLocation(programID, "knob3");
	glUniform1f(loc, interpolatedParameters[16]);
	loc = glGetUniformLocation(programID, "knob4");
	glUniform1f(loc, interpolatedParameters[17]);
	loc = glGetUniformLocation(programID, "knob5");
	glUniform1f(loc, interpolatedParameters[18]);
	loc = glGetUniformLocation(programID, "knob6");
	glUniform1f(loc, interpolatedParameters[19]);
	loc = glGetUniformLocation(programID, "knob7");
	glUniform1f(loc, interpolatedParameters[20]);
	loc = glGetUniformLocation(programID, "knob8");
	glUniform1f(loc, interpolatedParameters[21]);
	loc = glGetUniformLocation(programID, "knob9");
	glUniform1f(loc, interpolatedParameters[22]);
	loc = glGetUniformLocation(programID, "slider1");
	glUniform1f(loc, interpolatedParameters[2]);
	loc = glGetUniformLocation(programID, "slider2");
	glUniform1f(loc, interpolatedParameters[3]);
	loc = glGetUniformLocation(programID, "slider3");
	glUniform1f(loc, interpolatedParameters[4]);
	loc = glGetUniformLocation(programID, "slider4");
	glUniform1f(loc, interpolatedParameters[5]);
	loc = glGetUniformLocation(programID, "slider5");
	glUniform1f(loc, interpolatedParameters[6]);
	loc = glGetUniformLocation(programID, "slider6");
	glUniform1f(loc, interpolatedParameters[8]);
	loc = glGetUniformLocation(programID, "slider7");
	glUniform1f(loc, interpolatedParameters[9]);
	loc = glGetUniformLocation(programID, "slider8");
	glUniform1f(loc, interpolatedParameters[12]);
	loc = glGetUniformLocation(programID, "slider9");
	glUniform1f(loc, interpolatedParameters[13]);

	// Set texture identifiers
	GLint texture_location;
	texture_location = glGetUniformLocation(programID, "Noise3DTexture");
	glUniform1i(texture_location, 0);
	texture_location = glGetUniformLocation(programID, "DepthSensorTexture");
	glUniform1i(texture_location, 1);
	texture_location = glGetUniformLocation(programID, "BGTexture");
	glUniform1i(texture_location, 2);

	// render to larger offscreen texture
	glActiveTexture(GL_TEXTURE2);
	textureManager.getTextureID("hermaniak.tga", &textureID, errorText);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glActiveTexture(GL_TEXTURE1);
	//textureManager.getTextureID(TM_DEPTH_SENSOR_NAME, &textureID, errorText);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glActiveTexture(GL_TEXTURE0);
	textureManager.getTextureID(TM_NOISE3D_NAME, &textureID, errorText);
	glBindTexture(GL_TEXTURE_3D, textureID);

#if 0
	if (usedIndex > 4) {
		glViewport(0, 0, X_HIGHLIGHT, Y_HIGHLIGHT);
	} else {
		glViewport(0, 0, X_OFFSCREEN, Y_OFFSCREEN);
	}
#endif

    // TODO: Here is the rendering done!
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shaderManager.getProgramID("DitherTexture.gprg", &programID, errorText);
    glUseProgram(programID);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);

    glDisable(GL_CULL_FACE);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glDisable(GL_BLEND);

    if (ftime - music_start_time_ > 159.0f ||
        ftime - music_start_time_ < 0.5f) {  // Do the fluid stuff
        fluid_simulation_.UpdateTime(fdelta_time);
        fluid_simulation_.GetTexture();
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
        glEnd();
    } else {  // Do the rigit dance stuff
        glViewport(0, 0, X_HIGHLIGHT, Y_HIGHLIGHT);
        DrawMusic(ftime - music_start_time_);
        textureManager.getTextureID(TM_HIGHLIGHT_NAME, &textureID, errorText);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, X_HIGHLIGHT, Y_HIGHLIGHT);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, fluid_simulation_.GetBackBuffer());
        fluid_simulation_.SetBackBuffer();
        
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        int xres = windowRect.right - windowRect.left;
        int yres = windowRect.bottom - windowRect.top;
        glViewport(0, 0, xres, yres);
        DrawMusic(ftime - music_start_time_);
    }
#if 0
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    shaderManager.getProgramID("SimpleTexture.gprg", &programID, errorText);
    glUseProgram(programID);
    textureManager.getTextureID("blatt.tga", &textureID, errorText);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();
#endif

#if 0
	// Copy backbuffer to texture
	if (usedIndex > 4) {
		textureManager.getTextureID(TM_HIGHLIGHT_NAME, &textureID, errorText);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, X_HIGHLIGHT, Y_HIGHLIGHT);
	} else {
		textureManager.getTextureID(TM_OFFSCREEN_NAME, &textureID, errorText);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, X_OFFSCREEN, Y_OFFSCREEN);
	}

	// Copy backbuffer to front (so far no improvement)
	int xres = windowRect.right - windowRect.left;
	int yres = windowRect.bottom - windowRect.top;
	glViewport(0, 0, xres, yres);
    shaderManager.getProgramID("SimpleTexture.gprg", &programID, errorText);
	glUseProgram(programID);
	loc = glGetUniformLocation(programID, "time");
	glUniform1f(loc, (float)(t * 0.001f));
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glEnd();
#endif
}


int WINAPI WinMain( HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    MSG         msg;
    int         done=0;
    WININFO     *info = &wininfo;

    info->hInstance = GetModuleHandle( 0 );

    //if( MessageBox( 0, "fullscreen?", info->wndclass, MB_YESNO|MB_ICONQUESTION)==IDYES ) info->full++;

    if (!window_init(info))
    {
        window_end(info);
        MessageBox(0, "window_init()!", "error", MB_OK|MB_ICONEXCLAMATION);
        return 0;
    }

	if (initGL(info))
	{
		return 0;
	}

    //intro_init();

	// Initialize COM
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) exit(-1);

	// Example editor usage
	char errorText[MAX_ERROR_LENGTH+1];
	char filename[SM_MAX_FILENAME_LENGTH+1];
	sprintf_s(filename, SM_MAX_FILENAME_LENGTH, "shaders/%s", usedShader[usedIndex]);
	if (editor.loadText(filename, errorText))
	{
		MessageBox(info->hWnd, errorText, "Editor init", MB_OK);
		return -1;
	}

    start_time_ = timeGetTime();
    long last_time = 0;
    
    while( !done )
        {
		long t = timeGetTime() - start_time_;

        while( PeekMessage(&msg,0,0,0,PM_REMOVE) )
        {
            if( msg.message==WM_QUIT ) done=1;
		    TranslateMessage( &msg );
            DispatchMessage( &msg );
        }

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        intro_do(t, t - last_time);
		editor.render(t);
        last_time = t;

		SwapBuffers( info->hDC );
	}

    window_end( info );

#ifdef MUSIC
	// music uninit
	BASS_Free();
#endif

	// Un-initialize COM
	CoUninitialize();

    return( 0 );
}

// Note that a key was pressed
void registerParameterChange(int keyID)
{
	const int beatKey = 41;
	const int blobKey = 40;
	const int minBeatTime = 100;
	const int maxBeatTime = 5000;
	int sortedBeatDurations[NUM_BEAT_TIMES];

	// get the blobber
	if (params.getParam(blobKey) > 0.5f) blob = 1.f;

	// Do nothing on key release!
	if (params.getParam(beatKey) < 0.5f) return;

	if (keyID == beatKey)
	{
		int t = timeGetTime();
		int timeDiff = t - lastBeatTime;
		if (timeDiff > minBeatTime && timeDiff < maxBeatTime)
		{
			for (int i = 0; i < NUM_BEAT_TIMES-1; i++)
			{
				beatDurations[i] = beatDurations[i+1];
			}
			beatDurations[NUM_BEAT_TIMES-1] = timeDiff;
		}
		lastBeatTime = t;
	}

	// copy sorted beat durations
	for (int i = 0; i < NUM_BEAT_TIMES; i++)
	{
		sortedBeatDurations[i] = beatDurations[i];
	}

	// Calculate median of beat durations by bubble sorting.
	bool sorted = false;
	while (!sorted)
	{
		sorted = true;
		for (int i = 0; i < NUM_BEAT_TIMES-1; i++)
		{
			if (sortedBeatDurations[i] < sortedBeatDurations[i+1]) {
				int tmp = sortedBeatDurations[i+1];
				sortedBeatDurations[i+1] = sortedBeatDurations[i];
				sortedBeatDurations[i] = tmp;
				sorted = false;
			}
		}
	}

	BPM = 60.0f * 1000.0f / sortedBeatDurations[NUM_BEAT_TIMES/2];
}