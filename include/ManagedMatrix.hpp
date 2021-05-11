#ifndef MANAGEDMATRIX_H
#define MANAGEDMATRIX_H
#include "defs.hpp"

class ManagedMatrix {
public:
    mat4 * build();

    vec3 getTranslate();
    void setTranslate(vec3 v);
    void setTranslate(GLfloat x, GLfloat y, GLfloat z);

    vec3 getRotate();
    void setRotate(vec3 v);
    void setRotate(GLfloat x, GLfloat y, GLfloat z);

    vec3 getScale();
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
