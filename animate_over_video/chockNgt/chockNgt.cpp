// Camera_capture.cpp : Definiert den Einstiegspunkt f�r die Anwendung.
//
#include "stdafx.h"
#include "chockNgt.h"
//#include "bass.h"
#include "TextureManager.h"
#include "Configuration.h"
#include "TextDisplay.h"
#include "Frame.h"

#include <vector>

int X_OFFSCREEN = 512;
int Y_OFFSCREEN = 256;

// The frame that is currently edited
int current_frame_ = 0;
bool show_shadow_ = false;  // Show some previous frames
bool show_video_ = true;  // Show the background video (instead of white?)

// Debug: There is just one global frame that can be edited.
std::vector<Frame>frames_;

// For textDisplay. May have to adjust somehow?
float aspectRatio = 1.2f;

TextDisplay text_display_;

LRESULT CALLBACK WindowProc (HWND, UINT, WPARAM, LPARAM);

// -------------------------------------------------------------------
//                          Constants:
// -------------------------------------------------------------------

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
// main window stuff
HDC mainDC;
HGLRC mainRC;
HWND mainWnd;

float fCurTime = 0.0f;

TextureManager textureManager;

int Save(const char *filename, char *error_string) {
    FILE *file;

    fopen_s(&file, filename, "wb");
    if (NULL == file) {
        snprintf(error_string, MAX_ERROR_LENGTH, "Could not open file %s", filename);
        return -1;
    }

    int num_elements = frames_.size();
    fwrite(&num_elements, sizeof(num_elements), 1, file);

    for (int i = 0; i < num_elements; i++) {
        int ret_val = frames_[i].Save(file, error_string);
        if (ret_val < 0) {
            fclose(file);
            return ret_val;
        }
    }

    fclose(file);
    return 0;
}

int Load(const char *filename, char *error_string) {
    FILE *file;

    fopen_s(&file, filename, "rb");
    if (NULL == file) {
        snprintf(error_string, MAX_ERROR_LENGTH, "Could not open file %s", filename);
        return -1;
    }

    int num_elements;
    fread(&num_elements, sizeof(num_elements), 1, file);

    for (int i = 0; i < num_elements; i++) {
        frames_.push_back(Frame());
        int ret_val = frames_[i].Load(file, error_string);
        if (ret_val < 0) {
            fclose(file);
            return ret_val;
        }
    }

    fclose(file);
    return 0;
}

void glInit()
{
	mainDC = GetDC(mainWnd);
    if( !SetPixelFormat(mainDC,ChoosePixelFormat(mainDC,&pfd),&pfd) ) return;
    mainRC = wglCreateContext(mainDC);
    wglMakeCurrent(mainDC, mainRC);

	// Create and initialize everything needed for texture Management
	char errorString[MAX_ERROR_LENGTH+1];
	if (textureManager.init(errorString, mainDC))
	{
		MessageBox(mainWnd, errorString, "Texture Manager Load", MB_OK);
		return;
	}

    text_display_.init(&textureManager, errorString);

    glEnable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
}

void glUnInit()
{
	wglDeleteContext(mainRC);
	ReleaseDC(mainWnd, mainDC);
}

void DrawQuadColor(float startX, float endX, float startY, float endY,
                   float startU, float endU, float startV, float endV,
                   float red, float green, float blue, float alpha) {
    glColor4f(red, green, blue, alpha);
    glBegin(GL_QUADS);
    glTexCoord2f(startU, endV);
    glVertex3f(startX, endY, 0.99f);
    glTexCoord2f(endU, endV);
    glVertex3f(endX, endY, 0.99f);
    glTexCoord2f(endU, startV);
    glVertex3f(endX, startY, 0.99f);
    glTexCoord2f(startU, startV);
    glVertex3f(startX, startY, 0.99f);
    glEnd();
}

void DrawQuadColor(float startX, float endX, float startY, float endY, float red, float green, float blue, float alpha) {
    DrawQuadColor(startX, endX, startY, endY, 0.0f, 1.0f, 0.0f, 1.0f, red, green, blue, alpha);
}

void DrawQuad(float startX, float endX, float startY, float endY, float startU, float endU, float startV, float endV, float alpha) {
    DrawQuadColor(startX, endX, startY, endY, startU, endU, startV, endV, 1.0f, 1.0f, 1.0f, alpha);
}
void DrawQuad(float startX, float endX, float startY, float endY, float startV, float endV, float alpha) {
    DrawQuad(startX, endX, startY, endY, 0.0f, 1.0f, startV, endV, alpha);
}
void DrawQuad(float startX, float endX, float startY, float endY, float alpha) {
    DrawQuad(startX, endX, startY, endY, 0.0f, 1.0f, 0.0f, 1.0f, alpha);
}

//WinMain -- Main Window
int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow )
{
    MSG msg;
	msg.message = WM_CREATE;

    WNDCLASS wc;
    wc.style = CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "hotparticle";

    RegisterClass (&wc);

	// Create the window
#ifdef FULLSCREEN
    mainWnd = CreateWindow(wc.lpszClassName,"hot particle",WS_POPUP|WS_VISIBLE|WS_MAXIMIZE,0,0,0,0,0,0,hInstance,0);
#else
	mainWnd = CreateWindow(wc.lpszClassName,"hot particle",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX,CW_USEDEFAULT,CW_USEDEFAULT,XRES,YRES,0,0,hInstance,0);
#endif
	
	RECT windowRect;
	GetClientRect(mainWnd, &windowRect);
	X_OFFSCREEN = windowRect.right - windowRect.left;
	Y_OFFSCREEN = windowRect.bottom - windowRect.top;

    // Initialize first frame
    frames_.push_back(Frame());

	glInit();

    ShowWindow(mainWnd,SW_SHOW);
    UpdateWindow(mainWnd);
 
	int startTime = timeGetTime();
	long lastTime = 0;

	// start music playback
	GetAsyncKeyState(VK_ESCAPE);

    const float kTransformationMatrixLeft[4][4] = {
        {0.5f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {-0.5f, 0.0f, 0.0f, 1.0f},
    };
    const float kTransformationMatrixRight[4][4] = {
        { 0.5f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { +0.5f, 0.0f, 0.0f, 1.0f },
    };

	do
    {
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			//if (!IsDialogMessage(mainWnd, &msg))
			{
				TranslateMessage (&msg);
				DispatchMessage (&msg);
			}
		}
		if (msg.message == WM_QUIT) break; // early exit on quit

		// update timer
		long curTime = timeGetTime() - startTime;
		fCurTime = (float)curTime * 0.001f;
		long deltaTime = curTime - lastTime;
		float fDeltaTime = (float) deltaTime * 0.001f;
		lastTime = curTime;

		// render
		wglMakeCurrent(mainDC, mainRC);
		
		// Set the stuff to render to "rendertarget"
		//glViewport(0, 0, X_OFFSCREEN, Y_OFFSCREEN);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
        //GLuint tex_id;

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(kTransformationMatrixLeft[0]);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        int return_value = 1;  // Assume we are finished

        // Check for error
        if (return_value < 0) return -1;

        // Draw video background
        float videoTime = fCurTime;
        GLuint texID;
        char errorString[MAX_ERROR_LENGTH + 1];

        if (show_video_) {
            if (textureManager.GetVideoFrameID("Begrussung_N1.wmv", &texID, errorString, current_frame_) < 0) {
                MessageBox(mainWnd, errorString, "Texture manager get video ID", MB_OK);
                return -1;
            }
            glBindTexture(GL_TEXTURE_2D, texID);
            DrawQuad(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f);
        }

        // Show the frame number
        char frame_number_text[1024];
        sprintf_s(frame_number_text, sizeof(frame_number_text), "%d", current_frame_);
        text_display_.ShowText(0.0f, 0.0f, frame_number_text);

        // DEBUG: Draw a frame
        for (int draw_frame = current_frame_ - 2; draw_frame <= current_frame_; draw_frame++) {
            if (draw_frame >= 0 && (draw_frame == current_frame_ || show_shadow_)) {
                float alpha = 1.0f - 0.3f * (current_frame_ - draw_frame);
                return_value = frames_[draw_frame].Draw(&textureManager, errorString, alpha);
                if (return_value < 0) {
                    MessageBox(mainWnd, errorString, "Could not draw frame", MB_OK);
                    return -1;
                }
            }
        }

        // Draw the right frame without the video
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(kTransformationMatrixRight[0]);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if (textureManager.getTextureID("black.png", &texID, errorString) < 0) {
            MessageBox(mainWnd, errorString, "Texture manager get texture ID", MB_OK);
            return -1;
        }
        glBindTexture(GL_TEXTURE_2D, texID);
        DrawQuad(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f);
        return_value = frames_[current_frame_].DrawFancy(&textureManager, errorString);
        if (return_value < 0) {
            MessageBox(mainWnd, errorString, "Could not fancy draw frame", MB_OK);
            return -1;
        }

		// swap buffers
		wglSwapLayerBuffers(mainDC, WGL_SWAP_MAIN_PLANE);

		//Sleep(5);
    //} while (msg.message != WM_QUIT && !GetAsyncKeyState(VK_ESCAPE));
    } while (msg.message != WM_QUIT);

	glUnInit();
	
    return msg.wParam;
}

//Main Window Procedure WindowProc
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);

    RECT windowRect;
    GetClientRect(mainWnd, &windowRect);
    float width = (float)(windowRect.right - windowRect.left);
    float height = (float)(windowRect.bottom - windowRect.top);
    float xp = 2.0f * float(x) / width * 2.0f - 1.0f;
    if (xp > 1.0f) xp -= 2.0f;
    float yp = 1.0f - float(y) / height * 2.0f;

    char error_string[MAX_ERROR_LENGTH + 1];

    switch (message)                  /* handle the messages */
    {
    case WM_DESTROY:
        PostQuitMessage(0);   /* send a WM_QUIT to the message queue */
        break;
 
    case WM_LBUTTONDOWN:
        frames_[current_frame_].StartNewLine();
        frames_[current_frame_].AddLineNode(xp, yp);
        break;

    case WM_MOUSEMOVE:
        if (wParam & MK_LBUTTON) {
            frames_[current_frame_].AddLineNode(xp, yp);
        }
        break;

    case WM_KEYDOWN:
        switch (wParam) {
        case 'o':
        case 'O':
        case 'z':
        case 'Z':
            current_frame_--;
            if (current_frame_ < 0) current_frame_ = 0;
            while (current_frame_ >= (int)frames_.size()) {
                frames_.push_back(Frame());
            }
            break;
        case 'p':
        case 'P':
        case 'x':
        case 'X':
            current_frame_++;
            while (current_frame_ >= (int)frames_.size()) {
                frames_.push_back(Frame());
            }
            break;
        case 'c':  // c: Delete last line of frame
        case 'C':
            frames_[current_frame_].DeleteLastLine();
            break;
        case 'm':
        case 'M':
            show_shadow_ = !show_shadow_;
            break;
        case 'n':
        case 'N':
            show_video_ = !show_video_;
            break;
        case 's':
        case 'S':
            if (Save("savefile.frames", error_string) < 0) {
                MessageBox(mainWnd, error_string, "Save", MB_OK);
            }
            break;
        case'l':
        case'L':
            if (Load("savefile.frames", error_string) < 0) {
                MessageBox(mainWnd, error_string, "Load", MB_OK);
            }
        }

    default:              /* for messages that we don't deal with */
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}
