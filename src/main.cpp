#include "GLWindowManager.hpp"

using namespace std;

void loop() {
    
}

int32_t main(int32_t argc, const char * argv[]) {
    GLExtensionScanner gles;
    GLWindowManager glwm(&gles);
    GLint glMajor, glMinor;

    if(!glwm.start()) {
        cerr << "Could not start window manager!\n";
        cerr << "Error code: " << glwm.getError() << endl;
        exit(1);
    }

    glwm.showWindow();
    glwm.makeContextCurrent();

    gles.scanGLExtensions();
    gles.getGLVersion(glMajor, glMinor);
    cout << "GL version " << glMajor << '.' << glMinor << endl;

    glwm.setLoopFunction(&loop);
    glwm.loop();
    glwm.stop();
}
