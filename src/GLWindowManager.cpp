#include "GLWindowManager.hpp"

using namespace std;

bool GLWindowManager::xError = false;
XErrorEvent GLWindowManager::lastError;

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

bool GLWindowManager::checkGLXVersion() {
    if(!glXQueryVersion(d, &glXMajor, &glXMinor)) {
        cout << "Error: could not query GLX version." << endl;
        error = GLWM_GLX_QUERY_FAIL;
        return false;
    }

    if((glXMajor == 1 && glXMinor < 3) || glXMajor < 1) {
        cout << "Error: GLX version is " << glXMajor << '.' << glXMinor
                << ", requires at least 1.3." << endl;
        error = GLWM_GLX_BAD_VERSION;
        return false;
    }

    cout << "GLX version: " << glXMajor << '.' << glXMinor << endl;
    return true;
}

void GLWindowManager::selectScreen() {
    cout << "Default screen: " << (s = DefaultScreen(d)) << endl;
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

void GLWindowManager::getGLXExtensions() {
    glxExts = glXQueryExtensionsString(d, s);
    cout << "Supported GLX extensions:\n" << glxExts << endl;
}

bool GLWindowManager::glxExtSupported(const char * ext) {
    const char * loc = NULL, * start = glxExts, * after;
    size_t len = strlen(ext);

    while(1) {
        if(!(loc = strstr(start, ext))) return false;
        else {
            after = loc+len;
            if((loc == start || *(loc-1) == ' ')
                    && (*after == ' ' || *after == 0))
                return true;
            else start = after;
        }
    }
}

bool GLWindowManager::createContext() {
    static const char * ext = "GLX_ARB_create_context";
    static int attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 2,
        None
    };

    PFNGLXCREATECONTEXTATTRIBSARBPROC _glXCreateContextAttribsARB = NULL;

    if(!glxExtSupported(ext) || !(_glXCreateContextAttribsARB =
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

    attrs.background_pixel = 0;
    attrs.event_mask = StructureNotifyMask;
    attrs.colormap = XCreateColormap(d, rootW, vi->visual, AllocNone);

    xW = XCreateWindow(d, rootW, 0, 0, 100, 100, 0, depth, InputOutput,
            vi->visual, CWBackPixel|CWEventMask|CWColormap, &attrs);
    XFree(vi);
    if(xError || !xW) {
        cout << "Error creating X window." << endl;
        error = GLWM_X_CREATE_WINDOW_FAIL;
        return false;
    }
    cout << "Created X window." << endl;

    glxW = glXCreateWindow(d, fbc, xW, NULL);
    if(xError || !glxW) {
        cout << "Error creating GLX window." << endl;
        error = GLWM_GLX_CREATE_WINDOW_FAIL;
        return false;
    }
    cout << "Created GLX window." << endl;

    return true;
}

bool GLWindowManager::start() {
    if(started) return false;

    XSetErrorHandler(GLWindowManager::printError);
    if(!connectToDisplay()) return false;
    if(!checkGLXVersion()) {
        cleanup();
        return false;
    }
    selectScreen();
    selectDepth();
    if(!selectFBConfig()) {
        cleanup();
        return false;
    }
    getGLXExtensions();
    if(!createContext() || !createWindow()) {
        cleanup();
        return false;
    }

    started = true;
    return true;
}

void GLWindowManager::stop() {
    if(started) {
        cleanup();
        started = false;
    }
}

bool GLWindowManager::showWindow() {
    if(!started) return false;

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
    if(!started) return false;

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
    if(!started) return false;

    if(!glXMakeContextCurrent(d, glxW, glxW, c) || xError) {
        cout << "Error making context current." << endl;
        error = GLWM_CONTEXT_CURRENT_FAIL;
        return false;
    }
    cout << "Made context current." << endl;
    return true;
}

bool GLWindowManager::swapBuffers() {
    if(!started) return false;

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
