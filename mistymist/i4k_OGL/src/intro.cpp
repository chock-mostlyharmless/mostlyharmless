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

#include "config.h"
#include "intro.h"
#include "mzk.h"
#include "Parameter.h"

float frand();
int rand();

// -------------------------------------------------------------------
//                          SHADER CODE:
// -------------------------------------------------------------------

const GLchar *fragmentMainBackground="\
uniform sampler3D Texture0;\n\
varying vec3 objectPosition;\n\
varying mat4 parameters;\n\
\n\
vec3 noise(vec3 pos, int iterations, float reduction)\n\
{\n\
   pos *= 2.; /*adjust texture size*/\n\
   float intensity = 1.;\n\
   float size = 1.;\n\
   vec3 result = vec3(0.);\n\
\n\
   for (int k = 0; k < iterations; k++)\n\
   {\n\
      vec3 pos2 = floor(pos*size*16.) + smoothstep(0.,1., (pos*size*16.) - floor(pos*size*16.)) - 0.5;\n\
      vec4 inp = texture3D(Texture0, pos2/16.);\n\
      result += inp.xyz * intensity;\n\
      intensity = intensity * reduction;\n\
      size = size * 1.93;\n\
   }\n\
   \n\
   return result;\n\
}\n\
\n\
vec3 color(vec3 pos, float sphereSize)\n\
{\n\
   float fTime0_X = parameters[0][0];\n\
   float transform = parameters[1][0];\n\
   vec3 relpos = (pos - vec3(0.0, fTime0_X*0.6, 0.0)) * 0.5;\n\
   \n\
   //relpos += noise(relpos*0.03, 3, 0.6) * 1. + fTime0_X * 0.03;\n\
   float brightness, color;\n\
   brightness = noise(relpos*0.2, 5, 0.8).r + dot(pos, vec3(0., 1., 0.));\n\
   \n\
   color = brightness*2. * (1.0 - 0.8*transform);\n\
   return clamp(color * vec3(0.3, .6, 1.), -0.2, 1.5);\n\
}\n\
\n\
vec2 rotate(vec2 pos, float angle)\n\
{\n\
	return vec2(cos(angle)*pos.x - sin(angle)*pos.y,\n\
				sin(angle)*pos.x + cos(angle)*pos.y);\n\
}\n\
\n\
void main(void)\n\
{  \n\
   float fTime0_X = parameters[0][0];\n\
   float whitecolor = parameters[1][1];\n\
   vec3 rayDir = normalize(objectPosition * vec3(1.0, 0.6, 1.0));\n\
   vec3 camPos = vec3(0.0, 0.0, -5.7 + 5.7 * parameters[0][1]);\n\
   \n\
   // rotate camera around y axis\n\
   float alpha;\n\
   alpha = parameters[0][3]*9.42;\n\
   camPos.xz = rotate(camPos.xz, alpha);\n\
   rayDir.xz = rotate(rayDir.xz, alpha);\n\
   alpha = parameters[0][2]*9.42;\n\
   camPos.yz = rotate(camPos.yz, alpha);\n\
   rayDir.yz = rotate(rayDir.yz, alpha);\n\
   \n\
   vec3 rayPos = camPos;\n\
   float sceneSize = 16.0;\n\
   vec3 totalColor = vec3(0.,0.,0.);\n\
   float stepSize;\n\
   float totalDensity = 0.0;\n\
   vec3 totalColorAdder = (vec3(0.02, 0.014, 0.009) + whitecolor*vec3(0.,0.004,0.008)) * (parameters[1][2]);\n\
   \n\
   while(length(rayPos)<sceneSize && totalDensity < 0.9)\n\
   {\n\
      // base head\n\
      vec3 tmpPos = rayPos;\n\
      float base1 = abs(tmpPos.y);\n\
	  float base2 = abs(length(rayPos.xz) - 1.5 + 1.5 * parameters[2][2]) * (0.5 + parameters[3][2]);\n\
	  float socket = length(rayPos + vec3(0., 9., 0.)) - 8.2 + 20.*parameters[3][1];  \n\
	  float base = (max(base1 - 1.1 - parameters[2][3]*20., base2 - 1.5*parameters[2][2] + 0.8));\n\
	  float implicitVal = min(socket * (0.2 + parameters[3][3]), base);\n\
\n\
      vec3 noiseAdder = noise(rayPos * 0.003  - vec3(0.0, fTime0_X*0.006, 0.0), 3, 0.) * 0.1 * parameters[3][0];\n\
      float noiseVal = noise(noiseAdder + rayPos*0.04*vec3(1.0,0.05+0.95*parameters[3][0],1.0) - vec3(0.0, fTime0_X*0.03, 0.0), 7, 0.6).r * 0.6;\n\
      implicitVal -= noiseVal;\n\
      \n\
	  totalColor += totalColorAdder;\n\
      totalDensity += 0.003;\n\
      if (implicitVal < 0.05)\n\
      {\n\
         float localDensity = min(1.0, 0.05 - implicitVal);\n\
		 localDensity = (1.-totalDensity) * localDensity;\n\
         totalColor += color(rayPos, 0.) * localDensity;\n\
         totalDensity += localDensity ;\n\
      }\n\
      \n\
      stepSize = (implicitVal) * 0.3;\n\
      stepSize = max(0.01, stepSize);\n\
      rayPos += rayDir * stepSize;\n\
   }\n\
   \n\
   float grad = normalize(rayPos).y;\n\
   totalColor += (1.-totalDensity) * (grad * vec3(0.0,0.0,0.1) + (1.-grad)*vec3(0.0,0.1,0.2));\n\
   \n\
   gl_FragColor = vec4(totalColor-vec3(0.2), 1.0);\n\
\n\
}";

const GLchar *fragmentOffscreenCopy="\
uniform sampler2D Texture0;\n\
varying vec3 objectPosition;\n\
varying mat4 parameters;\n\
\n\
void main(void)\n\
{  \n\
   float fTime0_X = parameters[0][0];\n\
   vec3 noisePos = objectPosition + fTime0_X;\n\
   vec2 noiseVal;\n\
   noiseVal.x = fract(sin(dot(noisePos.xy, vec2(12.9898, 78.233))) * 43758.5453);\n\
   noiseVal.y = fract(sin(dot(noisePos.xy, vec2(12.9898, 78.233))) * 43753.5453);\n\
   gl_FragColor = texture2D(Texture0, 0.5*objectPosition.xy + 0.5 + 0.0007*noiseVal.xy) + noiseVal.x*0.02;\n\
\n\
}";

const GLchar *vertexMainObject="\
#version 120\n\
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

HWND hWnd;

#define NUM_GL_NAMES 10
const static char* glnames[NUM_GL_NAMES]={
     "glCreateShader", "glCreateProgram", "glShaderSource", "glCompileShader", 
     "glAttachShader", "glLinkProgram", "glUseProgram",
	 "glTexImage3D", "glGetShaderiv","glGetShaderInfoLog",
};

#define glCreateShader ((PFNGLCREATESHADERPROC)glFP[0])
#define glCreateProgram ((PFNGLCREATEPROGRAMPROC)glFP[1])
#define glShaderSource ((PFNGLSHADERSOURCEPROC)glFP[2])
#define glCompileShader ((PFNGLCOMPILESHADERPROC)glFP[3])
#define glAttachShader ((PFNGLATTACHSHADERPROC)glFP[4])
#define glLinkProgram ((PFNGLLINKPROGRAMPROC)glFP[5])
#define glUseProgram ((PFNGLUSEPROGRAMPROC)glFP[6])
#define glTexImage3D ((PFNGLTEXIMAGE3DPROC)glFP[7])
#define glGetShaderiv ((PFNGLGETSHADERIVPROC)glFP[8])
#define glGetShaderInfoLog ((PFNGLGETSHADERINFOLOGPROC)glFP[9])

// The model matrix is used to send the parameters to the hardware...
static float parameterMatrix[16];

static GLuint offscreenTexture;
// Name of the 32x32x32 noise texture
#define FLOAT_TEXTURE
#define NOISE_TEXTURE_SIZE 16 // try smaller?
static GLuint noiseTexture;
#ifdef FLOAT_TEXTURE
static float noiseData[NOISE_TEXTURE_SIZE * NOISE_TEXTURE_SIZE * NOISE_TEXTURE_SIZE * 4];
#else
static unsigned char noiseData[NOISE_TEXTURE_SIZE * NOISE_TEXTURE_SIZE * NOISE_TEXTURE_SIZE * 4];
#endif
static int noiseTmp[4];

typedef void (*GenFP)(void); // pointer to openGL functions
static GenFP glFP[NUM_GL_NAMES]; // pointer to openGL functions
static GLuint shaderPrograms[2];

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
	GLuint vMainObject = glCreateShader(GL_VERTEX_SHADER);
	GLuint fMainBackground = glCreateShader(GL_FRAGMENT_SHADER);	
	GLuint fOffscreenCopy = glCreateShader(GL_FRAGMENT_SHADER);
	shaderPrograms[0] = glCreateProgram();
	shaderPrograms[1] = glCreateProgram();
	// compile sources:
	glShaderSource(vMainObject, 1, &vertexMainObject, NULL);
	glCompileShader(vMainObject);
	glShaderSource(fMainBackground, 1, &fragmentMainBackground, NULL);
	glCompileShader(fMainBackground);
	glShaderSource(fOffscreenCopy, 1, &fragmentOffscreenCopy, NULL);
	glCompileShader(fOffscreenCopy);

	// Check programs
	int tmp, tmp2;
	char err[4097];
	glGetShaderiv(vMainObject, GL_COMPILE_STATUS, &tmp);
	if (!tmp)
	{
		glGetShaderInfoLog(vMainObject, 4096, &tmp2, err);
		err[tmp2]=0;
		MessageBox(hWnd, err, "vMainObject shader error", MB_OK);
		return;
	}
	glGetShaderiv(fMainBackground, GL_COMPILE_STATUS, &tmp);
	if (!tmp)
	{
		glGetShaderInfoLog(fMainBackground, 4096, &tmp2, err);
		err[tmp2]=0;
		MessageBox(hWnd, err, "fMainBackground shader error", MB_OK);
		return;
	}
	glGetShaderiv(fOffscreenCopy, GL_COMPILE_STATUS, &tmp);
	if (!tmp)
	{
		glGetShaderInfoLog(fOffscreenCopy, 4096, &tmp2, err);
		err[tmp2]=0;
		MessageBox(hWnd, err, "fOffscreeCopy shader error", MB_OK);
		return;
	}

	// link shaders:
	glAttachShader(shaderPrograms[0], vMainObject);
	glAttachShader(shaderPrograms[0], fMainBackground);
	glLinkProgram(shaderPrograms[0]);
	glAttachShader(shaderPrograms[1], vMainObject);
	glAttachShader(shaderPrograms[1], fOffscreenCopy);
	glLinkProgram(shaderPrograms[1]);

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
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F,
				 NOISE_TEXTURE_SIZE, NOISE_TEXTURE_SIZE, NOISE_TEXTURE_SIZE,
				 0, GL_RGBA, GL_FLOAT, noiseData);
#else
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8,
				 NOISE_TEXTURE_SIZE, NOISE_TEXTURE_SIZE, NOISE_TEXTURE_SIZE,
				 0, GL_RGBA, GL_UNSIGNED_BYTE, noiseData);
#endif

	// Create a rendertarget texture
	/*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
			     OFFSCREEN_WIDTH, OFFSCREEN_HEIGHT,
				 0, GL_RGBA, GL_UNSIGNED_BYTE, &offscreenTexture);*/
	glGenTextures(1, &offscreenTexture);
	glBindTexture(GL_TEXTURE_2D, offscreenTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, OFFSCREEN_WIDTH, OFFSCREEN_HEIGHT, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);

	// RLY?
	//glEnable(GL_CULL_FACE);
}

void fallingBall(float ftime)
{
	GLUquadric* quad = gluNewQuadric();

	glDisable(GL_BLEND);

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
	/* shader parameters */
	parameterMatrix[1] = params.getParam(2, 1.0f);
	parameterMatrix[2] = params.getParam(3, 1.0f);
	parameterMatrix[3] = params.getParam(4, 1.0f);
	parameterMatrix[4] = params.getParam(5, 1.0f);
	parameterMatrix[5] = params.getParam(6, 1.0f);
	parameterMatrix[6] = params.getParam(8, 0.54f);
	parameterMatrix[7] = params.getParam(9, 0.0f);
	parameterMatrix[8] = params.getParam(12, 0.0f);
	parameterMatrix[9] = params.getParam(13, 0.5f);
	parameterMatrix[10] = params.getParam(14, 0.5f);
	parameterMatrix[11] = params.getParam(15, 0.19f);
	parameterMatrix[12] = params.getParam(16, 0.53f);
	parameterMatrix[13] = params.getParam(17, 1.0f);
	parameterMatrix[14] = params.getParam(18, 0.5f);
	parameterMatrix[15] = params.getParam(19, 0.5f);
	glLoadMatrixf(parameterMatrix);

	// draw offscreen
	glViewport(0, 0, OFFSCREEN_WIDTH, OFFSCREEN_HEIGHT);
	glBindTexture(GL_TEXTURE_3D, offscreenTexture);
	glUseProgram(shaderPrograms[0]);
	glBindTexture(GL_TEXTURE_3D, noiseTexture);
	gluSphere(quad, 2.0f, 16, 16);

	// copy to front
	glViewport(0, 0, XRES, YRES);
	glBindTexture(GL_TEXTURE_3D, offscreenTexture);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, OFFSCREEN_WIDTH, OFFSCREEN_HEIGHT);   //Copy back buffer to texture
	glUseProgram(shaderPrograms[1]);	
	gluSphere(quad, 2.0f, 16, 16);
}

void intro_do( long itime )
{
	float ftime = 0.001f*(float)itime;

    // render
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
    glEnable( GL_CULL_FACE );
	//glCullFace( GL_FRONT );
	//glDisable( GL_BLEND );
    //glEnable( GL_LIGHTING );
    //glEnable( GL_LIGHT0 );
    //glEnable( GL_NORMALIZE );
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);	
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	// clear screan:
	//glClear(GL_COLOR_BUFFER_BIT);

	//glBindTexture(GL_TEXTURE_3D, noiseTexture); // 3D noise?	

	/* Set everything to beginning */
	for (int i = 0; i < 16; i++)
	{
		parameterMatrix[i] = 0.0f;
	}

	fallingBall(ftime);
}
