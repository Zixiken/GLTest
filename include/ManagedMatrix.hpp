/**
 * ManagedMatrix.hpp
 * 0.1
 * Michael Zanga
 *
 * A builder for transformation matrixies (defined in defs.hpp). Just supply
 * the translate, rotate, and scale data, and call build(). The object contains
 * a mat4 and returns a pointer to it. The same object can be reused without
 * having to reset all the transform data.
 */
#ifndef MANAGEDMATRIX_H
#define MANAGEDMATRIX_H
#include "defs.hpp"

class ManagedMatrix {
public:
    // Build a mat4 using previously supplied transform data, and return a
    // pointer to the mat4.
    mat4 * build();

    /**
     * get_(): Return the translate, rotate, or scale data as a vec3.
     * set_(...): Set the translate, rotate, or scale data. This can be passed
     *   as a vec3, or as a trio of GLfloats.
     * vec3 is defined in defs.hpp.
     */
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
