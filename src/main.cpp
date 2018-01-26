#include "GLWindowManager.hpp"

using namespace std;
int32_t main(int32_t argc, const char * argv[]) {
    GLWindowManager glwm;
    GLint glMajor, glMinor;

    if(glwm.start()) {
        glwm.showWindow();
        glwm.makeContextCurrent();

        glGetIntegerv(GL_MAJOR_VERSION, &glMajor);
        if(glGetError() != GL_INVALID_ENUM) glGetIntegerv(GL_MINOR_VERSION, &glMinor);
        else {
            cout << "Fell back to old-style get gl version.\n";
            const GLubyte * ver = glGetString(GL_VERSION);
            glMajor = (GLint)atoi((const char *)ver);
            glMinor = (GLint)atoi((const char *)ver+2);
        }
        cout << "GL version " << glMajor << '.' << glMinor << endl;

        glClearColor(.5, .5, .5, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glwm.swapBuffers();

        char * buf;
        cin.getline(buf, 0);
        glwm.stop();
    }
}
