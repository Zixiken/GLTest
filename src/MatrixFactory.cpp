#include "MatrixFactory.hpp"
#include <cmath>

using namespace std;

void MatrixFactory::setTranslate(GLfloat x, GLfloat y, GLfloat z) {
    translate.x = x;
    translate.y = y;
    translate.z = z;
    translateDirty = true;
}
void MatrixFactory::setTranslate(vec3 v) {
    translate = v;
    translateDirty = true;
}

void MatrixFactory::setRotate(GLfloat x, GLfloat y, GLfloat z) {
    rotate.x = x;
    rotate.y = y;
    rotate.z = z;
    scaleRotateDirty = true;
}
void MatrixFactory::setRotate(vec3 v) {
    rotate = v;
    scaleRotateDirty = true;
}

void MatrixFactory::setScale(GLfloat x, GLfloat y, GLfloat z) {
    scale.x = x;
    scale.y = y;
    scale.z = z;
    scaleRotateDirty = true;
}
void MatrixFactory::setScale(vec3 v) {
    scale = v;
    scaleRotateDirty = true;
}

mat4 * MatrixFactory::build() {
    if(scaleRotateDirty) {
        getRotateScaleMatrix();
        addTranslate();
    }
    else if(translateDirty) addTranslate();
    translateDirty = scaleRotateDirty = false;
    return &matrix;
}

void MatrixFactory::getRotateScaleMatrix() {
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
    }, rotZMatrix = {
        {zCos, -zSin, 0.0f, 0.0f},
        {zSin, zCos, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    matrix = rotXMatrix * rotYMatrix * rotZMatrix * scaleMatrix;
}

void MatrixFactory::addTranslate() {
    matrix.x.w = translate.x;
    matrix.y.w = translate.y;
    matrix.z.w = translate.z;
}
