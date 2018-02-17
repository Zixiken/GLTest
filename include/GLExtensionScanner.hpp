#ifndef GLES_H
#define GLES_H
#include <GL/glx.h>
#include <cstdint>
#include <cstring>
#include <cstdlib>

#define NUM_EXTENSIONS 667

class GLExtensionScanner {
public:
    void scanGLExtensions();
    void scanGLXExtensions(Display *, int32_t);
    bool isExtensionSupported(const char *);
    void getGLVersion(GLint &, GLint &);
    void getGLXVersion(int32_t &, int32_t &);

private:
    static const char * const extensions[NUM_EXTENSIONS];

    bool extSupported[NUM_EXTENSIONS] = {};
    int32_t glXMajor = 0, glXMinor = 0;
    GLint glMajor = 0, glMinor = 0;

    uint16_t getExtensionIndex(const char *);
    int8_t compareExtensionStrings(const char *, const char *);
};
#endif
