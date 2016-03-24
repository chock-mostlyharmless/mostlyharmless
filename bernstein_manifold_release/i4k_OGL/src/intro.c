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

#include <stdio.h>

#include "config.h"
#include "intro.h"
#include "mzk.h"

#include "script_seed.h"
#include "script_move.h"
#include "script_duration.h"

// -------------------------------------------------------------------
//                          SHADERS:
// -------------------------------------------------------------------
#pragma data_seg(".fragmentMainParticle")
const GLchar fragmentMainParticle[]="\
#version 330 core\n\
in vec2 u;\
in vec4 o;\
out vec4 c;\
void main(void){\
c=smoothstep(.0,.1,min(.9-abs(u).x,.9-.9*abs(u).y-.5*abs(u).x))*o;\
}";

#pragma data_seg(".geometryMainParticle")
const GLchar geometryMainParticle[]="\
#version 330 core\n\
layout(points) in;\
layout(triangle_strip, max_vertices=4) out;\
in vec4 p[];\
in float m[];\
out vec4 o;\
out vec2 u;\
uniform mat4 r;\
void main(){\
vec4 e=gl_in[0].gl_Position;\
float q=.001+abs(e.z-.5)*.025;\
mat2 w=min(q,.2*e.w)*mat2(.55,.2,-.1,.98);\
q=.001/q;\
q=1.*pow(q,2.)*smoothstep(q,.0,m[0])/q;\
if (q>.01){\
o=q*p[0];\
u=vec2(-1.,1.);\
gl_Position=e+vec4(w*u,.0,.0);\
EmitVertex();\
u=vec2(1.,1.);\
gl_Position=e+vec4(w*u,.0,.0);\
EmitVertex();\
u=vec2(-1.,-1.);\
gl_Position=e+vec4(w*u,.0,.0);\
EmitVertex();\
u=vec2(1.,-1.);\
gl_Position=e+vec4(w*u,0.,0.);\
EmitVertex();\
}\
EndPrimitive();\
}";

#pragma data_seg(".vertexMainHand")
const GLchar vertexMainHand[]="\
#version 330 core\n\
layout (location=0) in vec4 position_;\
layout (location=1) in vec4 color_;\
out vec4 p;\
out float m;\
uniform mat4 r;\
bool circle(vec2 pixel, vec2 e, float size) {\
    return (length(pixel-e)<size);\
}\
bool rect(vec2 pixel, vec2 topright, vec2 bottomright) {\
    return (pixel.x < topright.x && pixel.y < topright.y && pixel.x > bottomright.x && pixel.y > bottomright.y);\
}\
bool srect(vec2 pixel, vec4 coords) {\
    return (pixel.x+pixel.y > coords.x && pixel.x+pixel.y < coords.y && pixel.x-pixel.y > coords.z && pixel.x-pixel.y < coords.w);\
}\
bool fotze(vec2 pixel, float size) {\
    return (pixel.x > -size && pixel.x < size && pixel.y > -size && pixel.y < size && pixel.x-pixel.y < 1.4*size && pixel.x-pixel.y > -1.4*size);\
}\
void main(void) {\
    float t = r[0][0];\
    float yrot = sin(t*0.1)*1.;\
    yrot = 1.2 - 1.5 * abs(yrot);\
    mat2 yrotmat = mat2(cos(yrot),sin(yrot),-sin(yrot),cos(yrot));\
    vec3 e = position_.xyz;\
    vec2 q = e.xy * 0.5 + vec2(0.1, 0.1);\
    float alpha = max(0., 1.2 - length(e.xy));\
    vec3 color = vec3(0.6,0.8,0.9);\
    if (circle(q, vec2(0.), 0.2)) {color = vec3(1.,1.,.8); alpha = 1.;}\
    if (circle(q, vec2(-0.15, 0.3), 0.05)) {color = vec3(1.,1.,.8); alpha = 1.;}\
    if (rect(q, vec2(-0.1, 0.3), vec2(-0.2, 0.))) {color = vec3(1.,1.,.8); alpha = 1.;}\
    if (srect(q, vec4(0., 0.6, -0.28, -0.155))) {color = vec3(1.,1.,.8); alpha = 1.;}\
    if (srect(q, vec4(0., 0.65, -0.135, -0.01))) {color = vec3(1.,1.,.8); alpha = 1.;}\
    if (srect(q, vec4(0., 0.65, 0.01, 0.135))) {color = vec3(1.,1.,.8); alpha = 1.;}\
    if (srect(q, vec4(0., 0.58, 0.155, 0.28))) {color = vec3(1.,1.,.8); alpha = 1.;}\
    if (fotze(q, 0.1)) {color = vec3(0.2); alpha = 1.;}\
    if (fotze(q, 0.08)) {color = vec3(0.9,0.5,0.3); alpha = 1.;}\
    alpha *= smoothstep(1., 0.5, abs(e.z));\
    float explode = smoothstep(7., 20., t);\
    e.x += 0.01 * cos(e.z * 454. + t) + explode * sin(e.z*225.);\
    e.y += 0.01 * sin(e.z * 133. + t) + explode * cos(e.z*203.);\
    e.z = e.z * (0.03 + explode);\
    e.xy = e.xy * vec2(0.6, 0.7);\
    e.xz = e.xz * yrotmat;\
    e.z += 0.5 - .3 * yrot; \
    e.z -= 0.12 * t - 1.;\
    gl_Position = vec4(e.xyz, e.z);\
    m = color_.a;\
    alpha = alpha * color_.a;\
    p.rgb = color * alpha / (10. * pow(color_.a + .1*abs(sin(t*0.1+color_.a*100.)), 2.3) + 0.01);\
    p.a = alpha;\
}";

#pragma data_seg(".vertexMainParticle")
const GLchar vertexMainParticle[]="\
#version 330 core\n\
layout (location=0) in vec4 position_;\n\
layout (location=1) in vec4 color_;\n\
out vec4 p;\
out float m;\
uniform mat4 r;\
float GetImplicit(vec3 e) {\
    float t = r[0][0];\
    float first_amount = r[0][1];\
    float second_amount = r[0][2];\
    e.z -= sin(e.y * cos(e.x * 0.4) * 0.3) * second_amount * 5.5;\
    e.x -= sin(e.z*0.5) * second_amount * 5.5;\
    e.y -= sin(e.x*.25) * second_amount * 5.5;\
    e.x -= sin(t * cos(e.x * 2.1) * 0.3) * first_amount * 0.1;\
    e.y -= cos(t * cos(e.x * 0.5) * 0.4) * first_amount * 0.1;\
    e.z -= sin(t * sin(e.y * 0.2) * 0.5) * first_amount * 0.1;\
    float dist = r[3][1] * 25.0;\
    float dist2 = r[3][2] * 5.0;\
    float dist3 = r[3][3];\
    float implicit = length(e) - sin(dist * length(e)) * dist2 - dist3 - 0.2 * sin(t*0.);\
    return implicit;\
}\
void main(void) {\
    float t = r[0][0];\
    float yrot = sin(t*0.15)*0.5;\
    mat2 yrotmat = mat2(cos(yrot),sin(yrot),-sin(yrot),cos(yrot));\
    vec3 e = position_.xyz;\
    float implicit = GetImplicit(e.xyz);\
    float amount = 1. + r[1][1]*0.4 - smoothstep(0.0, 0.4, abs(implicit));\
    const float dd = 0.01;\
    float dx_implicit = GetImplicit(e.xyz + vec3(dd, 0., 0.)) - implicit;\
    float dy_implicit = GetImplicit(e.xyz + vec3(0., dd, 0.)) - implicit;\
    float dz_implicit = GetImplicit(e.xyz + vec3(0., 0., dd)) - implicit;\
    vec3 f = e.xyz * 0.45;\
    f.x += 12. * (r[1][0] * sin(t*0.49) * sin(e.z*7.) + 1. - r[1][0]) * dx_implicit * amount * r[0][3];\
    f.y += 12. * (r[1][0] * sin(t*0.31) * sin(e.x*5.) + 1. - r[1][0]) * dy_implicit * amount * r[0][3];\
    f.z += 12. * (r[1][0] * sin(t*0.37) * sin(e.y*6.) + 1. - r[1][0]) * dz_implicit * amount * r[0][3];\
    f.xz = f.xz * yrotmat;\
    f.x *= 0.56;\
    f.z += 1.5 - 2. * r[2][2];\
    f.x += 0.25 - 0.5 * r[2][3];\
    f.y += 0.25 - 0.5 * r[3][0];\
    gl_Position = vec4(f, f.z);\
    m = color_.a;\
    p.r = 0.6 + length(e.xyz) * .3;\
    p.g = 1.1 - length(e.xyz) * .4;\
    p.b = 1.2 - length(e.xy) * .7 - length(e.z) * 0.3;\
    p.rgb *= pow(1. - m, 30.) * 3. + 1.0;\
    p.rgb += pow(abs(sin(m*100. + t)),10.);\
    p.a = 1.0;\
    float darkener = abs(sin(e.x * r[1][2] * 8.) + cos(e.z * r[1][3] * 8.) - sin(e.y * r[2][0] * 8.));\
    p.a += darkener * r[2][1];\
    p.rgb -= vec3(darkener) * r[2][1] * 3.0;\
    p.rgb = max(vec3(0.), p.rgb);\
    p *= vec4(amount) * (1.0 - abs(m - 0.5));\
}";

// -------------------------------------------------------------------
//                          Constants:
// -------------------------------------------------------------------

#define NUM_PARTICLES_PER_DIM 128
#define TOTAL_NUM_PARTICLES (NUM_PARTICLES_PER_DIM * NUM_PARTICLES_PER_DIM * NUM_PARTICLES_PER_DIM)

//const int kSceneTic = 4410; // Number of 1/44100 seconds per scene-time-unit?
#define kSceneTic (2 * AUDIO_BUFFER_SIZE)

// This is only used if SHADER_DEBUG is on, but I will leave it for now.
HWND hWnd;
#ifdef SHADER_DEBUG
char err[4097];
#endif

static GLuint shaderPrograms[2];
// The vertex array and vertex buffer
unsigned int vaoID;
// 0 is for particle positions, 1 is for particle colors
unsigned int vboID[2];
// And the actual vertices
GLfloat vertices_[TOTAL_NUM_PARTICLES * 3];
GLfloat colors_[TOTAL_NUM_PARTICLES * 4];

// -------------------------------------------------------------------
//                          Code:
// -------------------------------------------------------------------

// Create the particle locations and move them to the GPU
#pragma code_seg(".GenerateParticles")
void GenerateParticles(void) {
    //unsigned int seed = 23690984;
    unsigned int seed = 0;

    // Set vertex location
    int vertex_id = 0;
    int color_id = 0;
    float pp[3];
    pp[2] = 1.0f;
    for (int z = 0; z < NUM_PARTICLES_PER_DIM; z++) {
        pp[1] = -1.0f;
        for (int y = 0; y < NUM_PARTICLES_PER_DIM; y++) {
            pp[0] = -1.0f;
            for (int x = 0; x < NUM_PARTICLES_PER_DIM; x++) {
                for (int dim = 0; dim < 3; dim++) {
                    vertices_[vertex_id++] = pp[dim] + 2.0f / (float)NUM_PARTICLES_PER_DIM * jo_frand(&seed);
                }
                color_id += 3;  // ignore RGB
                // fran
                colors_[color_id++] = jo_frand(&seed);
                pp[0] += 2.0f / (float)NUM_PARTICLES_PER_DIM;
            }
            pp[1] += 2.0f / (float)NUM_PARTICLES_PER_DIM;
        }
        pp[2] -= 2.0f / (float)NUM_PARTICLES_PER_DIM;
    }
}

const char *shader_codes[4] = {
    geometryMainParticle,
    fragmentMainParticle,
    vertexMainParticle,
    vertexMainHand
};
#pragma code_seg(".intro_init")
void intro_init( void ) {
    // Load the script
    // Create and link shader and stuff:

    // init objects:
    shaderPrograms[0] = glCreateProgram();
    shaderPrograms[1] = glCreateProgram();
#if 0
    GLuint gMainParticle = glCreateShader(GL_GEOMETRY_SHADER_EXT);
    GLuint fMainParticle = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint vMainParticle = glCreateShader(GL_VERTEX_SHADER);
    GLuint vHandParticle = glCreateShader(GL_VERTEX_SHADER);
    // compile sources:
    const char *pt = vertexMainParticle;
    glShaderSource(vMainParticle, 1, &pt, NULL);
    glCompileShader(vMainParticle);
    pt = vertexMainHand;
    glShaderSource(vHandParticle, 1, &pt, NULL);
    glCompileShader(vHandParticle);
    pt = geometryMainParticle;
    glShaderSource(gMainParticle, 1, &pt, NULL);
    glCompileShader(gMainParticle);
    pt = fragmentMainParticle;
    glShaderSource(fMainParticle, 1, &pt, NULL);
    glCompileShader(fMainParticle);
#else
    GLuint shaders[4];
#define gMainParticle (shaders[0])
#define fMainParticle (shaders[1])
#define vMainParticle (shaders[2])
#define vHandParticle (shaders[3])

    shaders[0] = glCreateShader(GL_GEOMETRY_SHADER_EXT);
    shaders[1] = glCreateShader(GL_FRAGMENT_SHADER);
    shaders[2] = glCreateShader(GL_VERTEX_SHADER);
    shaders[3] = glCreateShader(GL_VERTEX_SHADER);
    for (int i = 0; i < 4; i++) {
        glShaderSource(shaders[i], 1, &shader_codes[i], NULL);
        glCompileShader(shaders[i]);
    }
#endif

#ifdef SHADER_DEBUG
    // Check programs
    int tmp, tmp2;
    glGetShaderiv(vMainParticle, GL_COMPILE_STATUS, &tmp);
    if (!tmp)
    {
        glGetShaderInfoLog(vMainParticle, 4096, &tmp2, err);
        err[tmp2]=0;
        MessageBox(hWnd, err, "vMainParticle shader error", MB_OK);
        return;
    }
    glGetShaderiv(vHandParticle, GL_COMPILE_STATUS, &tmp);
    if (!tmp)
    {
        glGetShaderInfoLog(vHandParticle, 4096, &tmp2, err);
        err[tmp2]=0;
        MessageBox(hWnd, err, "vHandParticle shader error", MB_OK);
        return;
    }
    glGetShaderiv(gMainParticle, GL_COMPILE_STATUS, &tmp);
    if (!tmp)
    {
        glGetShaderInfoLog(gMainParticle, 4096, &tmp2, err);
        err[tmp2]=0;
        MessageBox(hWnd, err, "gMainParticle shader error", MB_OK);
        return;
    }
    glGetShaderiv(fMainParticle, GL_COMPILE_STATUS, &tmp);
    if (!tmp)
    {
        glGetShaderInfoLog(fMainParticle, 4096, &tmp2, err);
        err[tmp2]=0;
        MessageBox(hWnd, err, "fMainParticle shader error", MB_OK);
        return;
    }
#endif

    // link shaders:
    glAttachShader(shaderPrograms[0], vMainParticle);
    glAttachShader(shaderPrograms[0], gMainParticle);
    glAttachShader(shaderPrograms[0], fMainParticle);
    glLinkProgram(shaderPrograms[0]);
    glAttachShader(shaderPrograms[1], vHandParticle);
    glAttachShader(shaderPrograms[1], gMainParticle);
    glAttachShader(shaderPrograms[1], fMainParticle);
    glLinkProgram(shaderPrograms[1]);

#ifdef SHADER_DEBUG
    int programStatus;
    glGetProgramiv(shaderPrograms[0], GL_LINK_STATUS, &programStatus);
    if (programStatus == GL_FALSE)
    {
        MessageBox(hWnd, "Could not link program", "Shader 0 error", MB_OK);
        return;
    }
    glGetProgramiv(shaderPrograms[1], GL_LINK_STATUS, &programStatus);
    if (programStatus == GL_FALSE)
    {
        MessageBox(hWnd, "Could not link program", "Shader 1 error", MB_OK);
        return;
    }
#endif

    // Fill the vertices_ and colors_ array with reasonable values
    GenerateParticles();

    // Set up vertex buffer and stuff
    glGenVertexArrays(1, &vaoID); // Create our Vertex Array Object  
    glBindVertexArray(vaoID); // Bind our Vertex Array Object so we can use it  

    int maxAttrt;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttrt);

    glGenBuffers(2, vboID); // Generate our Vertex Buffer Object  

                            // Vertex array position data
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]); // Bind our Vertex Buffer Object  
    glBufferData(GL_ARRAY_BUFFER, TOTAL_NUM_PARTICLES * 3 * sizeof(GLfloat),
        vertices_, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW  
    glVertexAttribPointer(0, // attribute
        3, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0, // stride
        (void*)0); // array buffer offset

                   // Vertex array color data
                   // change to GL_STATIC_DRAW and single update for speed.
                   // Move the GenerateParticles copy operation to here.
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vboID[1]); // Bind our Vertex Buffer Object  
    glBufferData(GL_ARRAY_BUFFER, TOTAL_NUM_PARTICLES * 4 * sizeof(GLfloat),
        colors_, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW  
    glVertexAttribPointer(1, // attribute
        4, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0, // stride
        (void*)0); // array buffer offset

#ifdef SHADER_DEBUG
                   // Get all the errors:
    GLenum errorValue = glGetError();
    if (errorValue != GL_NO_ERROR)
    {
        char *errorString = (char *)gluErrorString(errorValue);
        MessageBox(hWnd, errorString, "Init error", MB_OK);
        return;
    }
#endif
}

#pragma code_seg(".intro_do")
void intro_do( long itime )
{
    static int lastTime = 0;
    static int timeDiff = 0;

    // Find the scene in the script
    int scene_id = 0;
    int start_time = 0;
    while (start_time + (int)(script_duration_[scene_id]) * kSceneTic < itime) {
        start_time += (int)(script_duration_[scene_id]) * kSceneTic;
        scene_id++;
    }

#if 0
    // smooth the time
    timeDiff = (100 * timeDiff + (itime - lastTime) * 28) / 128;
    itime = lastTime + timeDiff;
    lastTime = itime;
#else
    itime -= start_time;
#endif

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set the render matrix
    float parameterMatrix[4][4];
    parameterMatrix[0][0] = itime / 44100.0f;

    // Set parameters to other locations, using seed stuff
    unsigned int start_seed = script_seed_[scene_id]; 
    unsigned int seed = start_seed;
    for (int i = 1; i < 16; i++) {
        parameterMatrix[0][i] = jo_frand(&seed);
    }

    parameterMatrix[2][2] += (itime) * 0.000001f * script_move_[scene_id];

    int location = glGetUniformLocation(shaderPrograms[0], "r");
    glUniformMatrix4fv(location, 1, GL_FALSE, &(parameterMatrix[0][0]));
    // render
    glDisable( GL_CULL_FACE );
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // set the viewport (not neccessary?)
    //glGetIntegerv(GL_VIEWPORT, viewport);
    //glViewport(0, 0, XRES, YRES);

    // Set program 1 on seed == 0
    glUseProgram(shaderPrograms[start_seed == 0]);

    glDrawArrays(GL_POINTS, 0, TOTAL_NUM_PARTICLES);
}
