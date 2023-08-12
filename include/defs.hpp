/**
 * defs.hpp
 * 0.1
 * Michael Zanga
 *
 * This is a collection of defines for handling graphics data.
 */
#ifndef DEFS_H
#define DEFS_H
#include <GL/gl.h>
#include <iostream>

// A triplet or quartet of GLfloats
struct vec3 {GLfloat x, y, z;};
struct vec4 {GLfloat x, y, z, w;};

// A quartet of vec4's, with multiplication operators
struct mat4 {
    vec4 x, y, z, w;

    mat4 & operator*=(const mat4 & rhs);
};

// This operator just uses the multiply-assign operator defined above. Since
// it is inline, its implementation has to be in the header.
inline const mat4 operator*(mat4 lhs, const mat4 & rhs) {
    lhs *= rhs;
    return lhs;
}

// An ostream operator to print a mat4. The format is a leading newline, then
// each vector is printed as a comma-space-separated list followed by a newline
std::ostream & operator<<(std::ostream & os, const mat4 & rhs);

#endif
