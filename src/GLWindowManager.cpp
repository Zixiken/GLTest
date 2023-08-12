/**
 * GLWindowManager.cpp
 * 0.1
 * Michael Zanga
 */
#include <iostream>
#include <cstdint>
#include <cstring>
#include <ctime>
#include "GLWindowManager.hpp"

using namespace std;

bool GLWindowManager::xError = false;
XErrorEvent GLWindowManager::lastError;

// This is used as the X error handler
int GLWindowManager::printError(Display * d, XErrorEvent * e) {
    char buf[50];
    cout << "An X error occurred:\n";
    XGetErrorText(d, e->error_code, buf, 50);
    cout << buf;
    cout << "\nSerial: " << e->serial;
    cout << "\nError Code: " << static_cast<unsigned int>(e->error_code);
    cout << "\nRequest Code: " << static_cast<unsigned int>(e->request_code);
    cout << "\nMinor Code: " << static_cast<unsigned int>(e->minor_code) << endl;
    lastError = *e;
    xError = 1;
    return e->error_code;
}

void GLWindowManager::defaultLoopFunc() {}

void GLWindowManager::cleanup() {
    XCloseDisplay(d);
    cout << "Closed display." << endl;
}

bool GLWindowManager::connectToDisplay() {
    d = XOpenDisplay(NULL);
    if(!d) {
        cout << "Error conecting to display." << endl;
        error = GLWM_CONNECT_FAIL;
        return false;
    }

    cout << "Connected to display." << endl;
    return true;
}

void GLWindowManager::selectScreen() {
    cout << "Default screen: " << (s = DefaultScreen(d)) << endl;
}

bool GLWindowManager::checkGLXVersion() {
    int32_t major, minor;

    gles->scanGLXExtensions(d, s);
    gles->getGLXVersion(major, minor);
    if(!major && !minor) {
        cout << "Error: could not query GLX version." << endl;
        error = GLWM_GLX_QUERY_FAIL;
        return false;
    }

    if((major == 1 && minor < 3) || major < 1) {
        cout << "Error: GLX version is " << major << '.' << minor
                << ", requires at least 1.3." << endl;
        error = GLWM_GLX_BAD_VERSION;
        return false;
    }

    cout << "GLX version: " << major << '.' << minor << endl;
    return true;
}

void GLWindowManager::selectDepth() {
    cout << "Default depth: " << (depth = DefaultDepth(d, s)) << endl;
}

bool GLWindowManager::selectFBConfig() {
    static int32_t attribs[] = {
        GLX_DOUBLEBUFFER, True,
        GLX_AUX_BUFFERS, (int32_t)GLX_DONT_CARE,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, depth,
        GLX_STENCIL_SIZE, (int32_t)GLX_DONT_CARE,
        GLX_X_RENDERABLE, True,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_CONFIG_CAVEAT, GLX_NONE,
        None
    };

    int32_t numConfigs, bestConfig = 0, bestSamples = 0;
    GLXFBConfig * configList = glXChooseFBConfig(d, s, attribs, &numConfigs);

    if(!configList) {
        cout << "Error: no matching framebuffer configs found." << endl;
        error = GLWM_FBC_NO_MATCH;
        return false;
    }

    // Search through the available FBConfigs for the multisample one with the
    // most samples. If no multisample, the first in the list is used.
    for(int32_t i = 0; i < numConfigs; i++) {
        int32_t multisample, samples;
        glXGetFBConfigAttrib(d, configList[i], GLX_SAMPLE_BUFFERS, &multisample);
        if(!multisample) continue;
        glXGetFBConfigAttrib(d, configList[i], GLX_SAMPLES, &samples);
        if(samples > bestSamples) {
            bestSamples = samples;
            bestConfig = i;
        }
    }

    if(bestSamples) cout << "Using a multisample framebuffer config with "
            << bestSamples << " samples." << endl;
    else cout << "Using a singlesample framebuffer config." << endl;
    fbc = configList[bestConfig];
    XFree(configList);
    return true;
}

bool GLWindowManager::createContext() {
    static const char * ext = "GLX_ARB_create_context";
    static int attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        //GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
        None
    };

    // We need the function to create a context first
    PFNGLXCREATECONTEXTATTRIBSARBPROC _glXCreateContextAttribsARB = NULL;

    if(!gles->isExtensionSupported(ext) || !(_glXCreateContextAttribsARB =
            (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB(
                    (const GLubyte *)"glXCreateContextAttribsARB"))) {
        cout << "Error: GLX extension " << ext << " is not supported." << endl;
        error = GLWM_UNSUPPORTED_EXT;
        return false;
    }

    c = _glXCreateContextAttribsARB(d, fbc, NULL, True, attribs);
    if(xError || !c) {
        cout << "Error: could not create GLX context." << endl;
        error = GLWM_CREATE_CONTEXT_FAIL;
        return false;
    }

    if(glXIsDirect(d, c)) cout << "Created direct GLX context." << endl;
    else cout << "Created indrect GLX context." << endl;
    return true;
}

bool GLWindowManager::createWindow() {
    Window rootW = RootWindow(d, s);
    XVisualInfo * vi = glXGetVisualFromFBConfig(d, fbc);
    XSetWindowAttributes attrs;

    // The window has a black background, captures key presses and releases,
    // and comes with a colormap. Size is 640x640.
    attrs.background_pixel = 0;
    attrs.event_mask = KeyPressMask | KeyReleaseMask;
    attrs.colormap = XCreateColormap(d, rootW, vi->visual, AllocNone);

    xW = XCreateWindow(d, rootW, 0, 0, 640, 640, 0, depth, InputOutput,
            vi->visual, CWBackPixel|CWColormap|CWEventMask, &attrs);
    XFree(vi);
    XSync(d, False);
    if(xError || !xW) {
        cout << "Error creating X window." << endl;
        error = GLWM_X_CREATE_WINDOW_FAIL;
        return false;
    }
    cout << "Created X window." << endl;

    glxW = glXCreateWindow(d, fbc, xW, NULL);
    XSync(d, False);
    if(xError || !glxW) {
        cout << "Error creating GLX window." << endl;
        error = GLWM_GLX_CREATE_WINDOW_FAIL;
        return false;
    }
    cout << "Created GLX window." << endl;

    return true;
}

bool GLWindowManager::setWindowProps() {
    // Tell the window to listen to the close button
    protos = XInternAtom(d, "WM_PROTOCOLS", False);
    del = XInternAtom(d, "WM_DELETE_WINDOW", False);
    return XSetWMProtocols(d, xW, &del, 1);
}

void GLWindowManager::handleEvents() {
    XEvent e, n;

    // Handle all events in the queue
    while(XEventsQueued(d, QueuedAfterReading)) {
        XNextEvent(d, &e);
        // Stop the loop when the close button is pressed
        if(e.type == ClientMessage) {
            if(e.xclient.message_type == protos &&
                    (Atom)(e.xclient.data.l[0]) == del)
                doLoop = false;
        // Skip key presses and releases that are repeats from holding a key
        } else if(e.type == KeyRelease && XEventsQueued(d, QueuedAfterFlush)) {
            XPeekEvent(d, &n);
            if(n.type == KeyPress
            && n.xkey.time == e.xkey.time
            && n.xkey.keycode == e.xkey.keycode) {
                XNextEvent(d, &n);
                continue;
            }
        }
        // Send the event to all handlers in the list
        for(auto i = eventHandlers.begin(); i != eventHandlers.end(); i++) (*i)(e);
    }
}

bool GLWindowManager::init() {
    if(initialized) return false;
    //Reset error if a previous init failed
    error = GLWM_OK;
    xError = false;

    // This is the basic process for getting a GLX window. Many of these
    // functions are using simple hardcoded defaults that might need a revisit
    // later, like selecting the FBConfig and the window size.
    XSetErrorHandler(GLWindowManager::printError);
    if(!connectToDisplay()) return false;
    selectScreen();
    //GLX >=1.3 is needed
    if(!checkGLXVersion()) {
        cleanup();
        return false;
    }
    selectDepth();
    if(!selectFBConfig()
    || !createContext()
    || !createWindow()
    || !setWindowProps()) {
        cleanup();
        return false;
    }

    initialized = true;
    return true;
}

void GLWindowManager::deinit() {
    if(initialized) {
        cleanup();
        initialized = false;
    }
}

bool GLWindowManager::showWindow() {
    if(!initialized) return false;

    XMapWindow(d, xW);
    XSync(d, False);
    if(xError) {
        cout << "Error displaying window." << endl;
        error = GLWM_MAP_WINDOW_FAIL;
        return false;
    }
    cout << "Displayed window." << endl;
    return true;
}

bool GLWindowManager::hideWindow() {
    if(!initialized) return false;

    XUnmapWindow(d, xW);
    XSync(d, False);
    if(xError) {
        cout << "Error hiding window." << endl;
        error = GLWM_UNMAP_WINDOW_FAIL;
        return false;
    }
    cout << "Hid window." << endl;
    return true;
}

bool GLWindowManager::makeContextCurrent() {
    if(!initialized) return false;

    if(!glXMakeContextCurrent(d, glxW, glxW, c) || xError) {
        cout << "Error making context current." << endl;
        error = GLWM_CONTEXT_CURRENT_FAIL;
        return false;
    }
    cout << "Made context current." << endl;
    return true;
}

bool GLWindowManager::swapBuffers() {
    if(!initialized) return false;

    glXSwapBuffers(d, glxW);
    if(xError) {
        cout << "Error swapping buffers." << endl;
        error = GLWM_SWAP_BUFFER_FAIL;
        return false;
    }
    return true;
}

uint8_t GLWindowManager::getError() { return error; }

bool GLWindowManager::getXError(XErrorEvent * e) {
    if(xError && e != NULL) *e = lastError;
    return xError;
}

void GLWindowManager::setLoopFunction(void (* func)()) {
    if(!func) loopFunc = defaultLoopFunc;
    else loopFunc = func;
}

void GLWindowManager::startLoop() {
    doLoop = true;
    clock_t time;
    // This is currently calculated for a hardcoded 60fps
    clock_t clocksPerFrame = CLOCKS_PER_SEC / 60;
    struct timespec sleepTime;
    sleepTime.tv_sec = 0;

    while(doLoop) {
        time = clock();
        loopFunc();
        handleEvents();
	time = clock()-time;
        // Sleep if we haven't reached the frame time. I assume there are edge
	// cases where the overhead of these steps may overrun the frame time
        if(time < clocksPerFrame) {
            sleepTime.tv_nsec = (float)(clocksPerFrame-time) / CLOCKS_PER_SEC * 1000000000;
            nanosleep(&sleepTime, NULL);
	}
    }
}

void GLWindowManager::addEventHandler(eventHandlerFunc func) {
    for(auto i = eventHandlers.begin(); i != eventHandlers.end(); i++)
        if(*i == func) return;
    eventHandlers.push_front(func);
}

void GLWindowManager::removeEventHandler(eventHandlerFunc func) {eventHandlers.remove(func);}

void GLWindowManager::stopLoop() {doLoop = false;}

GLWindowManager::GLWindowManager(GLExtensionScanner * scanner): gles(scanner) {
    eventHandlers = forward_list<eventHandlerFunc>();
}
