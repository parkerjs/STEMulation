#ifndef FORWARD_MATRIX_H
#define FORWARD_MATRIX_H

#include <complex>
#include <type_traits>

namespace math
{

// forward declarations
namespace number_systems { namespace complex { template<typename> class Complex; } }

namespace linear_algebra
{

namespace matrix
{

// forward declarations
class ComplexMatrix;
class GeneralMatrix;
class NumericMatrix;
class RealMatrix;
class ReferenceMatrix;

// specializations that select the type of matrix
template<typename T, typename U = typename std::conditional<std::is_floating_point<T>::value, RealMatrix,
                                  typename std::conditional<std::is_integral<T>::value, NumericMatrix,
                                  GeneralMatrix>::type>::type> struct MatrixClass { typedef U type; };
template<typename T> struct MatrixClass<std::complex<T>> { typedef ComplexMatrix type; };
template<typename T> struct MatrixClass<number_systems::complex::Complex<T>> { typedef ComplexMatrix type; };
template<typename T> struct MatrixClass<T &> { typedef ReferenceMatrix type; };

// forward declarations
template<std::size_t N, typename T, typename Class = typename MatrixClass<T>::type> class Matrix;

}

}

}

#endif
