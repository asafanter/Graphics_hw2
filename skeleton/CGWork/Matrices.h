#ifndef MATRICES_H
#define MATRICES_H

#include "Matrix.h"
#include "Vector.h"
#include "TranformationMatrix.h"

using Mat2d = Matrix<double, 2, 2>;
using Mat3d = Matrix<double, 3, 3>;
using Mat2f = Matrix<float, 2, 2>;
using Mat3f = Matrix<float, 3, 3>;
using Tmatd = TransformationMatrix<double>;
using Tmatf = TransformationMatrix<float>;

using Vec2d = Vector<double, 2>;
using Vec3d = Vector<double, 3>;
using Vec4d = Vector<double, 4>;
using Vec2f = Vector<float, 2>;
using Vec3f = Vector<float, 3>;

#endif // MATRICES_H