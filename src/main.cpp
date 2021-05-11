#include <cstdlib>
#include <iomanip>
#include "defs.hpp"
#include "GLWindowManager.hpp"
#include "ManagedMatrix.hpp"
#include "CliManager.hpp"

using namespace std;

bool doLoop = false;
unsigned short height;

GLExtensionScanner gles;
GLWindowManager glwm(&gles);
ManagedMatrix world;
vec3 data[] = {
    {-0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f, -0.5f},
    {0.0f, -0.5f, 0.5f},
    {0.0f, 0.5f, 0.0f}
};
GLubyte indices[][3] = {
    {0, 1, 2},
    {0, 1, 3},
    {0, 2, 3},
    {1, 2, 3}
};
vec3 colorData[] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 1.0f}
};

GLuint IBO, VBO, CBO, VAO, shaderProg;
GLint worldLoc;
const GLchar * vProg = (const GLchar *)
"#version 330\n\
layout (location = 0) in vec3 position;\n\
layout (location = 1) in vec3 color;\n\
uniform mat4 world;\n\
out vec3 fragColor;\n\
void main() {\n\
    gl_Position = world * vec4(position, 1.0);\n\
    fragColor = color;\n\
}";
const GLchar * fProg = (const GLchar *)
"#version 330\n\
in vec3 fragColor;\n\
out vec4 color;\n\
void main() {\n\
    color = vec4(fragColor, 1.0f);\n\
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
    cout << hex << glGetError() << dec << endl;
    _glUseProgram(shaderProg);
}

void updateDisplay() {
    setPos(height-4, 1);
    cout << "Q -Z   W +Y   E +Z";
    setPos(height-2, 1);
    cout << "A -X   S -Y   D +X";

    setPos(height-4, 2, 0, 21);
    cout << INVERSE << "Translate:";
    setCol(3);
    vec3 tmp = world.getTranslate();
    cout << tmp.x << ' ' << tmp.y << ' ' << tmp.z << NORMAL;

    setPos(height-3, 2, 0, 25);
    cout << "Scale:";
    setCol(3);
    tmp = world.getScale();
    cout << tmp.x << ' ' << tmp.y << ' ' << tmp.z;

    setPos(height-2, 2, 0, 24);
    cout << "Rotate:";
    setCol(3);
    tmp = world.getRotate();
    cout << tmp.x << ' ' << tmp.y << ' ' << tmp.z << flush;
    setPos(height-1, 1);
}

void loop() {
    static float scale = 0.0f;
    scale += 0.01f;
    if(scale >= 3.14f) scale = 0.0f;
    world.setRotate(scale, 0.0f, scale);
    _glUniformMatrix4fv(worldLoc, 1, GL_TRUE, (const GLfloat *)(world.build()));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_BYTE, 0);
    glwm.swapBuffers();
    updateDisplay();
}

void handleKeys(XEvent & e) {
    if(e.type == KeyPress) {
        KeySym keysym = XLookupKeysym(&e.xkey, 0);
       	switch (keysym) {
            case XK_Escape: glwm.stopLoop(); break;
            case XK_space: glwm.setLoopFunction(doLoop ? NULL : &loop); doLoop = !doLoop; break;
        }
    }
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

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    _glGenVertexArrays(1, &VAO);
    _glBindVertexArray(VAO);
    _glGenBuffers(1, &VBO);
    _glBindBuffer(GL_ARRAY_BUFFER, VBO);
    _glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    _glEnableVertexAttribArray(0);
    _glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    _glGenBuffers(1, &IBO);
    _glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    _glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    _glGenBuffers(1, &CBO);
    _glBindBuffer(GL_ARRAY_BUFFER, CBO);
    _glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);
    _glEnableVertexAttribArray(1);
    _glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    world.setScale(1.0f, 1.0f, 1.0f);
    world.setTranslate(0.0f, 0.0f, 0.0f);

    glwm.showWindow();

    if(!initManager(1, 32, 4, 3)) {
        cerr << "Terminal too small or could not get terminal info, exiting" << endl;
	glwm.stop();
        exit(1);
    }

    height = getHeight();
    cout << setprecision(6);
    updateDisplay();

    glwm.addEventHandler(handleKeys);
    glwm.startLoop();
    closeManager();
    glwm.stop();
}
