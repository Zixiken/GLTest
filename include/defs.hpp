#ifndef DEFS_H
#define DEFS_H
#include <GL/gl.h>
#include <iostream>

using namespace std;

struct vec3 {GLfloat x, y, z;};
struct vec4 {GLfloat x, y, z, w;};
struct mat4 {
    vec4 x, y, z, w;

    mat4 & operator*=(const mat4 & rhs);
};

inline const mat4 operator*(mat4 lhs, const mat4 & rhs) {
    lhs *= rhs;
    return lhs;
}

ostream & operator<<(ostream & os, const mat4 & rhs);

#endif
