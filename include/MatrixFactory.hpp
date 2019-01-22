#ifndef MATRIXFACTORY_H
#define MATRIXFACTORY_H
#include "defs.hpp"

class MatrixFactory {
public:
    mat4 * build();

    void setTranslate(vec3 v);
    void setTranslate(GLfloat x, GLfloat y, GLfloat z);
    void setRotate(vec3 v);
    void setRotate(GLfloat x, GLfloat y, GLfloat z);
    void setScale(vec3 v);
    void setScale(GLfloat x, GLfloat y, GLfloat z);

private:
    vec3 rotate, translate, scale;
    mat4 matrix;
    bool translateDirty, scaleRotateDirty;

    void getRotateScaleMatrix();
    void addTranslate();
};

#endif
