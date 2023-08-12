/*
 * GLExtensionScanner.hpp
 * 0.1
 * Michael Zanga
 *
 * A library that handles querying OpenGL version information and supported
 * extensions. Requires an X display and screen for GLX extensions.
 */
#ifndef GLES_H
#define GLES_H
#include <GL/glx.h>
#include <cstdint>
#include <cstring>
#include <cstdlib>

#define NUM_EXTENSIONS 667

class GLExtensionScanner {
public:
    // Scan through GL extensions, saving the enabled or disabled state
    void scanGLExtensions();
    // Scan through GLX extensions, saving the enabled or disabled state
    void scanGLXExtensions(Display * d, int32_t s);
    // Returns whether or not the given extension is supported
    bool isExtensionSupported(const char * ext);
    // Retrieve the major and minor GL version information
    void getGLVersion(GLint & major, GLint & minor);
    // Retrieve the major and minor GLX version information
    void getGLXVersion(int32_t & major, int32_t & minor);

private:
    static const char * const extensions[NUM_EXTENSIONS];

    bool extSupported[NUM_EXTENSIONS] = {};
    int32_t glXMajor = 0, glXMinor = 0;
    GLint glMajor = 0, glMinor = 0;

    uint16_t getExtensionIndex(const char *);
    int8_t compareExtensionStrings(const char *, const char *);
};
#endif
