//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008                                        //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glext.h"

#include "intro.h"
#include "mzk.h"
#include "Parameter.h"

float frand();
int rand();

// -------------------------------------------------------------------
//                          SHADER CODE:
// -------------------------------------------------------------------

const GLchar *fragmentMainBackground="\
#version 140\n\
uniform sampler3D Texture0;\n\
varying vec3 objectPosition;\n\
varying mat4 parameters;\n\
\n\
vec3 noise(vec3 pos, int iterations, float reduction)\n\
{\n\
   pos *= 4.; /*adjust texture size*/\n\
   float intensity = 1.;\n\
   float size = 1.;\n\
   vec3 result = vec3(0.);\n\
\n\
   for (int k = 0; k < iterations; k++)\n\
   {\n\
      vec3 pos2 = floor(pos*size*8.) + smoothstep(0.,1., (pos*size*8.) - floor(pos*size*8.)) - 0.5;\n\
      vec4 inp = texture3D(Texture0, pos2/8.);\n\
      result += inp.xyz * intensity;\n\
      intensity = intensity * reduction;\n\
      size = size * 2.0;\n\
   }\n\
   \n\
   return result;\n\
}\n\
\n\
vec3 color(vec3 pos, float sphereSize)\n\
{\n\
   vec3 relpos = pos * 0.5;\n\
   \n\
    relpos += noise(relpos * 0.1 , 2, 0.6) * 0.2;\n\
   \n\
   float brightness = -1.0, color, whiteness;\n\
      \n\
   brightness = noise(relpos, 5, 0.8).r * 0.2 + 1.5*length(pos) - 0.75;\n\
   \n\
   color = clamp(abs(brightness * 3.0), 0.0, 1.0);\n\
   whiteness = clamp(brightness, 0.0, 0.5);\n\
   \n\
   vec3 bumpNormal = normalize(pos);\n\
   \n\
   float hemi = 0.7 + 0.5 * bumpNormal.y;   \n\
   float hemiSpec = pow(hemi,6.) * clamp(0.2 + 5.0 * whiteness, 0.0, 0.2);\n\
\n\
   return hemiSpec + hemi * (whiteness + color * vec3(0.3, 0.6, 1.0)); \n\
}\n\
\n\
void main(void)\n\
{  \n\
   float fTime0_X = parameters[0][0];\n\
   vec3 tvNoise =  noise(objectPosition*5.5 + vec3(0.,0.,fTime0_X*0.01), 2, 0.8);\n\
   vec3 objPos = objectPosition + tvNoise * 0.0;\n\
\n\
   vec3 rayDir = normalize(objPos * vec3(1.0, 0.6, 1.0));\n\
   vec3 camPos = vec3(0.0, 0.0, -1.7 + 0.75 * sin(fTime0_X*0.13));\n\
   \n\
   float alpha = fTime0_X * 0.37;\n\
   camPos.xz = vec2(cos(alpha)*camPos.x - sin(alpha)*camPos.z,\n\
                    sin(alpha)*camPos.x + cos(alpha)*camPos.z);\n\
   rayDir.xz = vec2(cos(alpha)*rayDir.x - sin(alpha)*rayDir.z,\n\
                    sin(alpha)*rayDir.x + cos(alpha)*rayDir.z);\n\
                    \n\
   \n\
   vec3 rayPos = camPos;\n\
   float sceneSize = 2.5;\n\
   vec3 totalColor = vec3(0.0);\n\
   float stepSize;\n\
   float totalDensity = 0.0;\n\
   \n\
   while(length(rayPos)<sceneSize && totalDensity < 0.95)\n\
   {\n\
      float sphereVal = length(rayPos * vec3(1.3, 0.7, 1.3)) - 0.5;\n\
	  float floorVal = rayPos.y + 0.6;\n\
	  float implicitVal = min(sphereVal, floorVal);\n\
      float distort = 1.;\n\
      float noiseVal = \n\
         noise(rayPos*0.07 - vec3(fTime0_X*0.01), 5, 0.5+sin(fTime0_X*0.27)*0.2).r * 0.7 * distort;\n\
      \n\
	  totalColor += vec3(1./50., 1./70., 1./90.) * 1.0 * clamp(sin(fTime0_X*0.07-1.), 0.0, 1.0);\n\
      totalDensity += 1./100.;\n\
      if (implicitVal - noiseVal < 0.05)\n\
      {\n\
         float localDensity = min(1.0, 2.*(0.05 - implicitVal + noiseVal));\n\
         totalColor = totalColor + (1.-totalDensity) * color(rayPos, 0.) * localDensity;\n\
         totalDensity = totalDensity + (1.-totalDensity) * localDensity;\n\
      }\n\
      \n\
      stepSize = (implicitVal - noiseVal) * 0.25;\n\
      stepSize = max(0.01, stepSize) * (tvNoise.r*0.04+0.96);\n\
      rayPos += rayDir * stepSize;\n\
   }\n\
\n\
   gl_FragColor = vec4(totalColor, 1.0);\n\
}";

const GLchar *vertexMainObject="\
#version 140\n\
varying vec3 objectPosition;\
varying mat4 parameters;\
\
void main(void)\
{\
   parameters = gl_ModelViewMatrix;\
   objectPosition = vec3(gl_Vertex.x, gl_Vertex.y, 1.0);\
   gl_Position = vec4(gl_Vertex.x, gl_Vertex.y, 0.5, 1.0);\
}";

// -------------------------------------------------------------------
//                          Constants:
// -------------------------------------------------------------------

#define fzn  0.125f
#define fzf  32.0f

#pragma data_seg(".projectionMatrix")
static const float projectionMatrix[16] = {
    4.0f, 0.00f,  0.0f,                    0.0f,
	0.0f, 8.0f,  0.0f,                    0.0f,
    0.0f, 0.00f, -(fzf+fzn)/(fzf-fzn),    -1.0f,
    0.0f, 0.00f, -2.0f*fzf*fzn/(fzf-fzn),  0.0f };
/*static const float projectionMatrix[16] = {
    10.0f, 0.00f,  0.0f,                    0.0f,
	0.0f, 17.7f,  0.0f,                    0.0f,
    0.0f, 0.00f, -(fzf+fzn)/(fzf-fzn),    -1.0f,
    0.0f, 0.00f, -2.0f*fzf*fzn/(fzf-fzn),  0.0f };*/

HWND hWnd;

#define NUM_GL_NAMES 10
const static char* glnames[NUM_GL_NAMES]={
     "glCreateShader", "glCreateProgram", "glShaderSource", "glCompileShader", 
     "glAttachShader", "glLinkProgram", "glUseProgram",
	 "glTexImage3D", "glGetShaderiv","glGetShaderInfoLog",
};

// The model matrix is used to send the parameters to the hardware...
static float parameterMatrix[16];

// Name of the 32x32x32 noise texture
#define FLOAT_TEXTURE
#define NOISE_TEXTURE_SIZE 8 // try smaller?
static GLuint noiseTexture;
#ifdef FLOAT_TEXTURE
static float noiseData[NOISE_TEXTURE_SIZE * NOISE_TEXTURE_SIZE * NOISE_TEXTURE_SIZE * 4];
#else
static unsigned char noiseData[NOISE_TEXTURE_SIZE * NOISE_TEXTURE_SIZE * NOISE_TEXTURE_SIZE * 4];
#endif
static int noiseTmp[4];

typedef void (*GenFP)(void); // pointer to openGL functions
static GenFP glFP[NUM_GL_NAMES]; // pointer to openGL functions
static GLuint shaderProgram;

// -------------------------------------------------------------------
//                          Code:
// -------------------------------------------------------------------

void intro_init( void )
{
	// create openGL functions
	for (int i=0; i<NUM_GL_NAMES; i++) glFP[i] = (GenFP)wglGetProcAddress(glnames[i]);

	// create noise Texture
#ifdef FLOAT_TEXTURE
	for (int i = 0; i < NOISE_TEXTURE_SIZE * NOISE_TEXTURE_SIZE * NOISE_TEXTURE_SIZE * 4; i++)
	{
		noiseData[i] = frand() - 0.5f;
	}
#else
	for (int i = 0; i < NOISE_TEXTURE_SIZE * NOISE_TEXTURE_SIZE * NOISE_TEXTURE_SIZE * 4; i++)
	{
		noiseData[i] = (unsigned char)rand();
	}
#endif

	// Create and link shader and stuff:
	// I will have to separate these to be able to use more than one shader...
	// TODO: I should make some sort of compiling and linking loop...
	
	// init objects:	
	GLuint vMainObject = ((PFNGLCREATESHADERPROC)(glFP[0]))(GL_VERTEX_SHADER);
	//GLuint fNoise = ((PFNGLCREATESHADERPROC)(glFP[0]))(GL_FRAGMENT_SHADER);	
	GLuint fMainBackground = ((PFNGLCREATESHADERPROC)(glFP[0]))(GL_FRAGMENT_SHADER);	
	shaderProgram = ((PFNGLCREATEPROGRAMPROC)glFP[1])();
	// compile sources:
	((PFNGLSHADERSOURCEPROC)glFP[2]) (vMainObject, 1, &vertexMainObject, NULL);
	((PFNGLCOMPILESHADERPROC)glFP[3])(vMainObject);
	//((PFNGLSHADERSOURCEPROC)glFP[2]) (fNoise, 1, &fragmentNoise, NULL);
	//((PFNGLCOMPILESHADERPROC)glFP[3])(fNoise);
	((PFNGLSHADERSOURCEPROC)glFP[2]) (fMainBackground, 1, &fragmentMainBackground, NULL);
	((PFNGLCOMPILESHADERPROC)glFP[3])(fMainBackground);

	// Check programs
	int tmp, tmp2;
	char err[4097];
	((PFNGLGETSHADERIVPROC)glFP[8])(vMainObject, GL_COMPILE_STATUS, &tmp);
	if (!tmp)
	{
		((PFNGLGETSHADERINFOLOGPROC)glFP[9])(vMainObject, 4096, &tmp2, err);
		err[tmp2]=0;
		MessageBox(hWnd, err, "vMainObject shader error", MB_OK);
		return;
	}
	((PFNGLGETSHADERIVPROC)glFP[8])(fMainBackground, GL_COMPILE_STATUS, &tmp);
	if (!tmp)
	{
		((PFNGLGETSHADERINFOLOGPROC)glFP[9])(fMainBackground, 4096, &tmp2, err);
		err[tmp2]=0;
		MessageBox(hWnd, err, "fMainBackground shader error", MB_OK);
		return;
	}

	// link shaders:
	((PFNGLATTACHSHADERPROC)glFP[4])(shaderProgram, vMainObject);
	//((PFNGLATTACHSHADERPROC)glFP[4])(shaderPrograms[0], fNoise);
	((PFNGLATTACHSHADERPROC)glFP[4])(shaderProgram, fMainBackground);
	((PFNGLLINKPROGRAMPROC)glFP[5])(shaderProgram);

	// Set texture.
	glEnable(GL_TEXTURE_3D); // automatic?
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_3D, noiseTexture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	//glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, NOISE_TEXTURE_SIZE, NOISE_TEXTURE_SIZE, NOISE_TEXTURE_SIZE, 0, GL_RGBA, 
	//	         GL_UNSIGNED_BYTE, noiseData);
#ifdef FLOAT_TEXTURE
	((PFNGLTEXIMAGE3DPROC) glFP[7])(GL_TEXTURE_3D, 0, GL_RGBA32F,
									NOISE_TEXTURE_SIZE, NOISE_TEXTURE_SIZE, NOISE_TEXTURE_SIZE,
									0, GL_RGBA, GL_FLOAT, noiseData);
#else
	((PFNGLTEXIMAGE3DPROC) glFP[7])(GL_TEXTURE_3D, 0, GL_RGBA8,
									NOISE_TEXTURE_SIZE, NOISE_TEXTURE_SIZE, NOISE_TEXTURE_SIZE,
									0, GL_RGBA, GL_UNSIGNED_BYTE, noiseData);
#endif

	// RLY?
	//glEnable(GL_CULL_FACE);
}

void fallingBall(float ftime)
{
	GLUquadric* quad = gluNewQuadric();

	glDisable(GL_BLEND);

	// attenuate time...
	//ftime = atan(ftime) * 1.9f;
	float spritzer = (ftime * ftime - 1.5f);
	if (spritzer < 0.0f) spritzer = -5.0f;	

	// Use background shader:	
	((PFNGLUSEPROGRAMPROC) glFP[6])(shaderProgram);	

	//glCullFace(GL_FRONT);

	// draw background:
#if 0
    glMatrixMode(GL_PROJECTION);	
	glLoadMatrixf(projectionMatrix);
	glTranslatef(params.getParam(20, 0.41f) * 6.0f - 3.0f, params.getParam(21, 0.56f) * 6.0f - 3.0f, -params.getParam(22, 0.36f) * 20.0f);
	glRotatef(params.getParam(9, 0.15f) * 360.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(params.getParam(12, 0.31f) * 360.0f, 0.0f, 1.0f, 0.0f); 
	glRotatef(params.getParam(13, 0.35f) * 360.0f, 0.0f, 0.0f, 1.0f);
#endif
	glMatrixMode(GL_MODELVIEW);	


	parameterMatrix[0] = ftime; // time	
	/* vertex shader parameters */
	//2:0.59 3:0.11 18:0.22 20:0.54 21:0.48 22:0.57 42:0.28 43:0.72 50:0.60 
	parameterMatrix[1] = params.getParam(2, 0.59f);
	parameterMatrix[2] = params.getParam(3, 0.11f);
	parameterMatrix[3] = params.getParam(4, 0.69f);
	parameterMatrix[4] = params.getParam(42, 0.28f);
	parameterMatrix[5] = params.getParam(43, 0.72f);
	parameterMatrix[6] = params.getParam(50, 0.60f);
	/* pixel shader parameters */
	//14:0.25 15:0.82 16:0.17 17:0.00 18:0.24 19:0.12 // Baseline
    //14:0.00 15:0.54 16:0.54 17:0.10 18:0.06 19:0.50 // Blue fog
	//14:0.09 15:0.23 16:0.65 17:0.27 18:0.69 19:1.00
	//14:0.02 15:0.91 16:0.43 17:0.10 18:0.00 19:0.28 
	parameterMatrix[8] = params.getParam(14, 0.25f);
	parameterMatrix[9] = params.getParam(15, 0.82f);
	parameterMatrix[10] = params.getParam(16, 0.17f);
	parameterMatrix[11] = params.getParam(17, 0.00f);
	parameterMatrix[12] = params.getParam(18, 0.24f);
	parameterMatrix[13] = params.getParam(19, 0.12f);

	glLoadMatrixf(parameterMatrix);		
	gluSphere(quad, 2.0f, 16, 16);
}

void intro_do( long itime )
{
	float ftime = 0.001f*(float)itime;

    // render
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
    glEnable( GL_CULL_FACE );
	//glDisable( GL_BLEND );
    //glEnable( GL_LIGHTING );
    //glEnable( GL_LIGHT0 );
    //glEnable( GL_NORMALIZE );
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);	
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	// clear screan:
	glClear(GL_COLOR_BUFFER_BIT);

	//glBindTexture(GL_TEXTURE_3D, noiseTexture); // 3D noise?	

	/* Set everything to beginning */
	for (int i = 0; i < 16; i++)
	{
		parameterMatrix[i] = 0.0f;
	}

	fallingBall(ftime);
}

