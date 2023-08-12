/**
 * ManagedMatrix.cpp
 * 0.1
 * Michael Zanga
 */
#include "ManagedMatrix.hpp"
#include <cmath>

using namespace std;

vec3 ManagedMatrix::getTranslate() {return translate;}

void ManagedMatrix::setTranslate(GLfloat x, GLfloat y, GLfloat z) {
    translate.x = x;
    translate.y = y;
    translate.z = z;
    translateDirty = true;
}
void ManagedMatrix::setTranslate(vec3 v) {
    translate = v;
    translateDirty = true;
}

vec3 ManagedMatrix::getRotate() {return rotate;}

void ManagedMatrix::setRotate(GLfloat x, GLfloat y, GLfloat z) {
    rotate.x = x;
    rotate.y = y;
    rotate.z = z;
    scaleRotateDirty = true;
}
void ManagedMatrix::setRotate(vec3 v) {
    rotate = v;
    scaleRotateDirty = true;
}

vec3 ManagedMatrix::getScale() {return scale;}

void ManagedMatrix::setScale(GLfloat x, GLfloat y, GLfloat z) {
    scale.x = x;
    scale.y = y;
    scale.z = z;
    scaleRotateDirty = true;
}
void ManagedMatrix::setScale(vec3 v) {
    scale = v;
    scaleRotateDirty = true;
}

// Avoid recalculating the rotate*scale transform matrix if only
// the translate vector has changed.
mat4 * ManagedMatrix::build() {
    if(scaleRotateDirty) {
        getRotateScaleMatrix();
        addTranslate();
    }
    else if(translateDirty) addTranslate();
    translateDirty = scaleRotateDirty = false;
    return &matrix;
}

void ManagedMatrix::getRotateScaleMatrix() {
    GLfloat xCos = cos(rotate.x), xSin = sin(rotate.x),
            yCos = cos(rotate.y), ySin = sin(rotate.y),
            zCos = cos(rotate.z), zSin = sin(rotate.z);
    mat4 scaleMatrix = {
        {scale.x, 0.0f, 0.0f, 0.0f},
        {0.0f, scale.y, 0.0f, 0.0f},
        {0.0f, 0.0f, scale.z, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }, rotXMatrix = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, xCos, -xSin, 0.0f},
        {0.0f, xSin, xCos, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }, rotYMatrix = {
        {yCos, 0.0f, -ySin, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {ySin, 0.0f, yCos, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    // Start with the Z rotation matrix here
    matrix = {
        {zCos, -zSin, 0.0f, 0.0f},
        {zSin, zCos, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    matrix *= rotYMatrix;
    matrix *= rotXMatrix;
    matrix *= scaleMatrix;
}

void ManagedMatrix::addTranslate() {
    matrix.x.w = translate.x;
    matrix.y.w = translate.y;
    matrix.z.w = translate.z;
}
