#ifndef GLWM_H
#define GLWM_H
#include <iostream>
#include <limits>
#include <cstdint>
#include <cstring>
#include <GL/glx.h>

#define GLWM_OK                     0
#define GLWM_CONNECT_FAIL           1
#define GLWM_GLX_QUERY_FAIL         2
#define GLWM_GLX_BAD_VERSION        3
#define GLWM_FBC_NO_MATCH           4
#define GLWM_UNSUPPORTED_EXT        5
#define GLWM_CREATE_CONTEXT_FAIL    6
#define GLWM_X_CREATE_WINDOW_FAIL   7
#define GLWM_GLX_CREATE_WINDOW_FAIL 8
#define GLWM_CONTEXT_CURRENT_FAIL   9
#define GLWM_MAP_WINDOW_FAIL        10
#define GLWM_UNMAP_WINDOW_FAIL      11
#define GLWM_SWAP_BUFFER_FAIL       12

class GLWindowManager {
public:
    bool start();
    void stop();
    bool showWindow();
    bool hideWindow();
    bool makeContextCurrent();
    bool swapBuffers();
    uint8_t getError();
    bool getXError(XErrorEvent *);

private:
    static bool xError;
    static XErrorEvent lastError;

    bool started;
    uint8_t error;
    int32_t depth, s, glXMajor, glXMinor;
    const char * glxExts;
    GLXWindow glxW;
    GLXFBConfig fbc;
    GLXContext c;
    Window xW;
    Display * d;

    static int printError(Display *, XErrorEvent *);

    void cleanup();
    bool connectToDisplay();
    bool checkGLXVersion();
    void selectScreen();
    void selectDepth();
    bool selectFBConfig();
    void getGLXExtensions();
    bool glxExtSupported(const char *);
    bool createContext();
    bool createWindow();
};
#endif