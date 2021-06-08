#ifndef GLWM_H
#define GLWM_H
#include <iostream>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <forward_list>
#include <GL/glx.h>
#include "GLExtensionScanner.hpp"

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
    typedef void (* eventHandlerFunc)(XEvent &);

    bool start();
    void stop();
    bool showWindow();
    bool hideWindow();
    bool makeContextCurrent();
    bool swapBuffers();
    uint8_t getError();
    bool getXError(XErrorEvent *);
    void setLoopFunction(void (*)());
    void startLoop();
    void addEventHandler(eventHandlerFunc);
    void removeEventHandler(eventHandlerFunc);
    void stopLoop();

    GLWindowManager(GLExtensionScanner *);

private:
    static bool xError;
    static XErrorEvent lastError;

    GLXWindow glxW;
    GLXFBConfig fbc;
    GLXContext c;
    Window xW;
    Display * d;
    Atom protos, del;
    bool doLoop, started = false;
    uint8_t error = GLWM_OK;
    int32_t depth, s;
    GLExtensionScanner * gles;
    void (* loopFunc)() = &defaultLoopFunc;
    std::forward_list<eventHandlerFunc> eventHandlers;

    static int printError(Display *, XErrorEvent *);
    static void defaultLoopFunc();

    void cleanup();
    bool connectToDisplay();
    bool checkGLXVersion();
    void selectScreen();
    void selectDepth();
    bool selectFBConfig();
    bool createContext();
    bool createWindow();
    bool setWindowProps();
    void handleEvents();
};
#endif
