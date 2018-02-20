#include "GLWindowManager.hpp"
#include <cmath>

using namespace std;

typedef struct {GLfloat x, y, z;} vec3;
typedef struct {GLfloat x, y, z, w;} vec4;
typedef struct {vec4 x, y, z, w;} mat4;

GLExtensionScanner gles;
GLWindowManager glwm(&gles);
vec3 data[3] = {
    {-1.0f, -1.0f, 0.0f},
    {1.0f, -1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f}
};
mat4 world = {
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}
};

GLuint VBO, VAO, shaderProg;
GLint worldLoc, rgbLoc;
const GLchar * vProg = (const GLchar *)
"#version 330\n\
layout (location = 0) in vec3 position;\n\
uniform mat4 world;\n\
out vec3 rgb;\n\
void main() {\n\
    gl_Position = world * vec4(position, 1.0);\n\
    rgb = vec3((gl_Position.x+1)/2, (gl_Position.y+1)/2, world[0][0]);\n\
}";
const GLchar * fProg = (const GLchar *)
"#version 330\n\
in vec3 rgb;\n\
out vec4 color;\n\
void main() {\n\
    color = vec4(rgb, 1.0f);\n\
}";

// GL 1.5
PFNGLBINDBUFFERPROC _glBindBuffer = NULL;
PFNGLBUFFERDATAPROC _glBufferData = NULL;
PFNGLGENBUFFERSPROC _glGenBuffers = NULL;

// GL 2.0
PFNGLATTACHSHADERPROC _glAttachShader = NULL;
PFNGLCOMPILESHADERPROC _glCompileShader = NULL;
PFNGLCREATEPROGRAMPROC _glCreateProgram = NULL;
PFNGLCREATESHADERPROC _glCreateShader = NULL;
PFNGLDETACHSHADERPROC _glDetachShader = NULL;
PFNGLDELETESHADERPROC _glDeleteShader = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC _glDisableVertexAttribArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC _glEnableVertexAttribArray = NULL;
PFNGLGETPROGRAMINFOLOGPROC _glGetProgramInfoLog = NULL;
PFNGLGETPROGRAMIVPROC _glGetProgramiv = NULL;
PFNGLGETSHADERINFOLOGPROC _glGetShaderInfoLog = NULL;
PFNGLGETSHADERIVPROC _glGetShaderiv = NULL;
PFNGLGETUNIFORMLOCATIONPROC _glGetUniformLocation = NULL;
PFNGLLINKPROGRAMPROC _glLinkProgram = NULL;
PFNGLSHADERSOURCEPROC _glShaderSource = NULL;
//PFNGLUNIFORM1FPROC _glUniform1f = NULL;
//PFNGLUNIFORM3FPROC _glUniform3f = NULL;
PFNGLUNIFORMMATRIX4FVPROC _glUniformMatrix4fv = NULL;
PFNGLUSEPROGRAMPROC _glUseProgram = NULL;
PFNGLVALIDATEPROGRAMPROC _glValidateProgram = NULL;
PFNGLVERTEXATTRIBPOINTERPROC _glVertexAttribPointer = NULL;

// GL 3.0
PFNGLGENVERTEXARRAYSPROC _glGenVertexArrays = NULL;
PFNGLBINDVERTEXARRAYPROC _glBindVertexArray = NULL;

bool initPointers() {
    if(!(_glBindBuffer = (PFNGLBINDBUFFERPROC)glXGetProcAddress(
            (const GLubyte *)"glBindBuffer"))) return false;
    if(!(_glBufferData = (PFNGLBUFFERDATAPROC)glXGetProcAddress(
            (const GLubyte *)"glBufferData"))) return false;
    if(!(_glGenBuffers = (PFNGLGENBUFFERSPROC)glXGetProcAddress(
            (const GLubyte *)"glGenBuffers"))) return false;

    if(!(_glAttachShader = (PFNGLATTACHSHADERPROC)glXGetProcAddress(
            (const GLubyte *)"glAttachShader"))) return false;
    if(!(_glCompileShader = (PFNGLCOMPILESHADERPROC)glXGetProcAddress(
            (const GLubyte *)"glCompileShader"))) return false;
    if(!(_glCreateProgram = (PFNGLCREATEPROGRAMPROC)glXGetProcAddress(
            (const GLubyte *)"glCreateProgram"))) return false;
    if(!(_glCreateShader = (PFNGLCREATESHADERPROC)glXGetProcAddress(
            (const GLubyte *)"glCreateShader"))) return false;
    if(!(_glDetachShader = (PFNGLDETACHSHADERPROC)glXGetProcAddress(
            (const GLubyte *)"glDetachShader"))) return false;
    if(!(_glDeleteShader = (PFNGLDELETESHADERPROC)glXGetProcAddress(
            (const GLubyte *)"glDeleteShader"))) return false;
    if(!(_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress(
            (const GLubyte *)"glDisableVertexAttribArray"))) return false;
    if(!(_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress(
            (const GLubyte *)"glEnableVertexAttribArray"))) return false;
    if(!(_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glXGetProcAddress(
            (const GLubyte *)"glGetProgramInfoLog"))) return false;
    if(!(_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glXGetProcAddress(
            (const GLubyte *)"glGetProgramiv"))) return false;
    if(!(_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glXGetProcAddress(
            (const GLubyte *)"glGetShaderInfoLog"))) return false;
    if(!(_glGetShaderiv = (PFNGLGETSHADERIVPROC)glXGetProcAddress(
            (const GLubyte *)"glGetShaderiv"))) return false;
    if(!(_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glXGetProcAddress(
            (const GLubyte *)"glGetUniformLocation"))) return false;
    if(!(_glLinkProgram = (PFNGLLINKPROGRAMPROC)glXGetProcAddress(
            (const GLubyte *)"glLinkProgram"))) return false;
    if(!(_glShaderSource = (PFNGLSHADERSOURCEPROC)glXGetProcAddress(
            (const GLubyte *)"glShaderSource"))) return false;
    //if(!(_glUniform1f = (PFNGLUNIFORM1FPROC)glXGetProcAddress(
    //        (const GLubyte *)"glUniform1f"))) return false;
    //if(!(_glUniform3f = (PFNGLUNIFORM3FPROC)glXGetProcAddress(
    //        (const GLubyte *)"glUniform3f"))) return false;
    if(!(_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)glXGetProcAddress(
            (const GLubyte *)"glUniformMatrix4fv"))) return false;
    if(!(_glUseProgram = (PFNGLUSEPROGRAMPROC)glXGetProcAddress(
            (const GLubyte *)"glUseProgram"))) return false;
    if(!(_glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)glXGetProcAddress(
            (const GLubyte *)"glValidateProgram"))) return false;
    if(!(_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glXGetProcAddress(
            (const GLubyte *)"glVertexAttribPointer"))) return false;

    if(!(_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glXGetProcAddress(
            (const GLubyte *)"glGenVertexArrays"))) return false;
    if(!(_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glXGetProcAddress(
            (const GLubyte *)"glBindVertexArray"))) return false;
    //if(!(_gl = (PFNGLPROC)glXGetProcAddress(
    //        (const GLubyte *)"gl"))) return false;
    return true;
}

void loop() {
    static float scale = 0.0f, scale2 = 0.0f;
    scale += 0.01f;
    scale2 += 0.023f;
    float sine = sin(scale), cosine = cos(scale), sine2 = sin(scale2), cosine2 = cos(scale2);
    world.x.w = sine2*0.5f;
    world.y.w = cosine2*0.5f;
    world.x.x = cosine*sine2;
    world.x.y = -sine*sine;
    world.y.x = sine*sine2;
    world.y.y = cosine*sine;
    _glUniformMatrix4fv(worldLoc, 1, GL_TRUE, (const GLfloat *)&world);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

bool compileShader(const GLchar * prog, GLuint shaderObj) {
    GLint l = strlen(prog);
    _glShaderSource(shaderObj, 1, &prog, &l);
    _glCompileShader(shaderObj);
    GLint success;
    _glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
    if(!success) {
        GLchar log[1024];
        _glGetShaderInfoLog(shaderObj, sizeof(log), NULL, log);
        cerr << "Error compiling shader:\n" << log << endl;
        return false;
    } else return true;
}

void initShaderProgram() {
    shaderProg = _glCreateProgram();
    GLuint vShaderObj = _glCreateShader(GL_VERTEX_SHADER), fShaderObj = _glCreateShader(GL_FRAGMENT_SHADER);
    if(!(compileShader(vProg, vShaderObj) && compileShader(fProg, fShaderObj))) {
        glwm.stop();
        exit(1);
    }
    _glAttachShader(shaderProg, vShaderObj);
    _glAttachShader(shaderProg, fShaderObj);
    _glLinkProgram(shaderProg);
    GLint success;
    _glGetProgramiv(shaderProg, GL_LINK_STATUS, &success);
    if(!success) {
        GLchar log[1024];
        _glGetProgramInfoLog(shaderProg, sizeof(log), NULL, log);
        cerr << "Error linking shader:\n" << log << endl;
        glwm.stop();
        exit(1);
    }
    _glDetachShader(shaderProg, vShaderObj);
    _glDetachShader(shaderProg, fShaderObj);
    _glDeleteShader(vShaderObj);
    _glDeleteShader(fShaderObj);
    _glValidateProgram(shaderProg);
    cout << hex << glGetError() << endl;
    _glUseProgram(shaderProg);
}

int32_t main(int32_t argc, const char * argv[]) {
    GLint glMajor, glMinor;

    if(!glwm.start()) {
        cerr << "Could not start window manager!\n";
        cerr << "Error code: " << glwm.getError() << endl;
        exit(1);
    }

    glwm.makeContextCurrent();
    gles.scanGLExtensions();
    gles.getGLVersion(glMajor, glMinor);
    cout << "GL version " << glMajor << '.' << glMinor << endl;
    if(!initPointers()) {
        cout << "Failed to init pointers" << endl;
        glwm.stop();
        exit(1);
    }

    initShaderProgram();
    if((worldLoc = _glGetUniformLocation(shaderProg, "world")) == -1) {
        cerr << "Could not get uniform location!" << endl;
        glwm.stop();
        exit(1);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    _glGenVertexArrays(1, &VAO);
    _glBindVertexArray(VAO);
    _glGenBuffers(1, &VBO);
    _glBindBuffer(GL_ARRAY_BUFFER, VBO);
    _glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    _glEnableVertexAttribArray(0);
    _glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glwm.setLoopFunction(&loop);
    glwm.showWindow();
    glwm.loop();
    glwm.stop();
}
