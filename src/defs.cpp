#include "defs.hpp"

using namespace std;

mat4 & mat4::operator*=(const mat4 & rhs) {
    mat4 temp;
    if(&rhs != this) {
        temp.x.x = (x.x*rhs.x.x)+(x.y*rhs.y.x)+(x.z*rhs.z.x)+(x.w*rhs.w.x);
        temp.x.y = (x.x*rhs.x.y)+(x.y*rhs.y.y)+(x.z*rhs.z.y)+(x.w*rhs.w.y);
        temp.x.z = (x.x*rhs.x.z)+(x.y*rhs.y.z)+(x.z*rhs.z.z)+(x.w*rhs.w.z);
        temp.x.w = (x.x*rhs.x.w)+(x.y*rhs.y.w)+(x.z*rhs.z.w)+(x.w*rhs.w.w);

        temp.y.x = (y.x*rhs.x.x)+(y.y*rhs.y.x)+(y.z*rhs.z.x)+(y.w*rhs.w.x);
        temp.y.y = (y.x*rhs.x.y)+(y.y*rhs.y.y)+(y.z*rhs.z.y)+(y.w*rhs.w.y);
        temp.y.z = (y.x*rhs.x.z)+(y.y*rhs.y.z)+(y.z*rhs.z.z)+(y.w*rhs.w.z);
        temp.y.w = (y.x*rhs.x.w)+(y.y*rhs.y.w)+(y.z*rhs.z.w)+(y.w*rhs.w.w);

        temp.z.x = (z.x*rhs.x.x)+(z.y*rhs.y.x)+(z.z*rhs.z.x)+(z.w*rhs.w.x);
        temp.z.y = (z.x*rhs.x.y)+(z.y*rhs.y.y)+(z.z*rhs.z.y)+(z.w*rhs.w.y);
        temp.z.z = (z.x*rhs.x.z)+(z.y*rhs.y.z)+(z.z*rhs.z.z)+(z.w*rhs.w.z);
        temp.z.w = (z.x*rhs.x.w)+(z.y*rhs.y.w)+(z.z*rhs.z.w)+(z.w*rhs.w.w);

        temp.w.x = (w.x*rhs.x.x)+(w.y*rhs.y.x)+(w.z*rhs.z.x)+(w.w*rhs.w.x);
        temp.w.y = (w.x*rhs.x.y)+(w.y*rhs.y.y)+(w.z*rhs.z.y)+(w.w*rhs.w.y);
        temp.w.z = (w.x*rhs.x.z)+(w.y*rhs.y.z)+(w.z*rhs.z.z)+(w.w*rhs.w.z);
        temp.w.w = (w.x*rhs.x.w)+(w.y*rhs.y.w)+(w.z*rhs.z.w)+(w.w*rhs.w.w);
        *this = temp;
    }
    return *this;
}

ostream & operator<<(ostream & os, const mat4 & rhs) {
    os << '\n'
       << rhs.x.x << ", " << rhs.x.y << ", " << rhs.x.z << ", " << rhs.x.w << '\n'
       << rhs.y.x << ", " << rhs.y.y << ", " << rhs.y.z << ", " << rhs.y.w << '\n'
       << rhs.z.x << ", " << rhs.z.y << ", " << rhs.z.z << ", " << rhs.z.w << '\n'
       << rhs.w.x << ", " << rhs.w.y << ", " << rhs.w.z << ", " << rhs.w.w << '\n';
    return os;
}
