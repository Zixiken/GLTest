/*
 * GLWindowManager.hpp
 * 0.1
 * Michael Zanga
 *
 * A basic window manager that creates an X11 window with an OpenGL context.
 * Supports event handlers that use XEvents, and running a function on loop.
 * This requires a GLExtensionScanner object to aid in detecting the
 * current machine's graphics capabilities. libX11 and glx are used for
 * interacting with the X server and graphics context.
 */
#ifndef GLWM_H
#define GLWM_H
#include <forward_list>
#include <GL/glx.h>
#include "GLExtensionScanner.hpp"

//Various status codes for error detection
#define GLWM_OK                     0  //No error
#define GLWM_CONNECT_FAIL           1  //XOpenDisplay returned error
#define GLWM_GLX_QUERY_FAIL         2  //gLXQueryVersion returned error
#define GLWM_GLX_BAD_VERSION        3  //GLX version present is below 1.3
#define GLWM_FBC_NO_MATCH           4  //No frame buffer config found matching requested options
                                       //(8-bit color depth, double buffer)
#define GLWM_UNSUPPORTED_EXT        5  //Required function glXCreateContextAttribsARB
                                       //is not supported
#define GLWM_CREATE_CONTEXT_FAIL    6  //glXCreateContextAttribsARB returned error
#define GLWM_X_CREATE_WINDOW_FAIL   7  //XCreateWindow returned error
#define GLWM_GLX_CREATE_WINDOW_FAIL 8  //glXCreateWindow returned error
#define GLWM_CONTEXT_CURRENT_FAIL   9  //glXMakeContextCurrent returned error
#define GLWM_MAP_WINDOW_FAIL        10 //XMapWindow returned error
#define GLWM_UNMAP_WINDOW_FAIL      11 //XUnmapWindow returned error
#define GLWM_SWAP_BUFFER_FAIL       12 //glXSwapBuffers returned error

class GLWindowManager {
public:
    //This function typedef is the signature of all event handler functions
    //used by addEventHandler and removeEventHandler.
    typedef void (* eventHandlerFunc)(XEvent &);

    /*
     * This will attempt to connect to the X server and create the window and
     * graphics context, unless the manager has already been initialized.
     *
     * Returns: true if initialization succeeded, false if an error occurred.
     */
    bool init();

    //This will disconnect from the X server, removing the created window as
    //well. No action if the manager was not initialized.
    void deinit();

    //Make the window visible on the screen (shows in taskbar, etc.)
    //Returns: true if the window was shown, false if an X error occurred.
    bool showWindow();

    //Make the window invisible on the screen (removes from taskbar, etc.)
    //Returns: true if the window was hidden, false if an X error occurred.
    bool hideWindow();

    //Bind the graphics context to this process.
    //Returns: false if an error was generated (see getXError), true otherwise.
    bool makeContextCurrent();

    //Swap the front and back buffers of the window.
    //Returns: false if an error was generated (see getXError), true otherwise.
    bool swapBuffers();

    //Returns the latest error code that was generated, see the defines near
    //the top of this file for their meanings.
    uint8_t getError();

    //Returns true if an X error has occurred, and populates the passed
    //XErrorEvent with the error. Otherwise, returns false.
    bool getXError(XErrorEvent * e);

    //Set the function to be executed when the loop is running. This must
    //take no arguments and return nothing. Default is an empty function,
    //and passing null has the same effect.
    void setLoopFunction(void (* func)());

    //Start the loop, which runs the loop function and handles X events.
    //This targets 60 "frames" per second.
    void startLoop();

    //Adds the passed eventHandlerFunc to the list of event handlers.
    //This function has no effect if the handler is already in the list.
    //All functions in the list will be passed XEvent references that
    //the window receives in sequence.
    void addEventHandler(eventHandlerFunc func);

    //Removes the passed eventHandlerFunc from the list of event handlers.
    void removeEventHandler(eventHandlerFunc func);

    //Stop the loop. The current run will still finish executing.
    void stopLoop();

    //Constructor, takes a pointer to a GLExtensionScanner.
    GLWindowManager(GLExtensionScanner * scanner);

private:
    static bool xError;
    static XErrorEvent lastError;

    GLXWindow glxW;
    GLXFBConfig fbc;
    GLXContext c;
    Window xW;
    Display * d;
    Atom protos, del;
    bool doLoop, initialized = false;
    uint8_t error;
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
