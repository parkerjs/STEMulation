#ifndef REAL_MATRIX_2D_H
#define REAL_MATRIX_2D_H

#include "complex_matrix_2d.h"
#include "real_matrix.h"

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * Two-dimensional real data type matrix class
 */
template<typename T>
class Matrix<2, T, RealMatrix>
: public Matrix<2, T, ComplexMatrix>,
  public RealMatrix,
  virtual private attributes::abstract::Reflective

{
public:

    /**
     * Typedef declarations
     */
    typedef T data_type;
    typedef typename std::decay<T>::type decay_type;
    typedef RealMatrix matrix_type;

    /**
     * Using declarations
     */
    using Matrix<2, T, ComplexMatrix>::operator +=;
    using Matrix<2, T, ComplexMatrix>::operator -=;
    using Matrix<2, T, ComplexMatrix>::operator *=;
    using Matrix<2, T, ComplexMatrix>::operator /=;
    using Matrix<2, T, ComplexMatrix>::print;
    using Matrix<2, T, NumericMatrix>::transpose;

    /**
     * Forwarding constructor
     */
    template<typename ... Args>
    Matrix(Args && ... args)
    : Matrix<2, T, ComplexMatrix>(std::forward<Args>(args) ...)
    {

    }

    /**
     * Destructor
     */
    virtual ~Matrix(void) override
    {

    }

    /**
     * Forwarding assignment operator
     */
    template<typename Arg>
    auto &operator = (Arg &&arg)
    {
        Matrix<2, T, ComplexMatrix>::operator = (std::forward<Arg>(arg));

        return *this;
    }

    /**
     * Addition assignment operator
     */
    template<typename R, typename V = typename std::decay<R>::type>
    inline typename std::enable_if<std::is_base_of<ComplexMatrix, V>::value &&
                                   !std::is_base_of<RealMatrix, V>::value,
                                   Matrix<2, std::complex<decay_type>>>::type
    operator += (R &&rhs)
    {
        return Matrix<2, std::complex<decay_type>>(*this).operator += (rhs);
    }

    /**
     * Subtraction assignment operator
     */
    template<typename R, typename V = typename std::decay<R>::type>
    inline typename std::enable_if<std::is_base_of<ComplexMatrix, V>::value &&
                                   !std::is_base_of<RealMatrix, V>::value,
                                   Matrix<2, std::complex<decay_type>>>::type
    operator -= (R &&rhs)
    {
        return Matrix<2, std::complex<decay_type>>(*this).operator -= (rhs);
    }

    /**
     * Multiplication assignment operator
     */
    template<typename R, typename V = typename std::decay<R>::type>
    inline typename std::enable_if<std::is_base_of<ComplexMatrix, V>::value &&
                                   !std::is_base_of<RealMatrix, V>::value,
                                   Matrix<2, std::complex<decay_type>>>::type
    operator *= (R &&rhs)
    {
        return Matrix<2, std::complex<decay_type>>(*this).operator *= (rhs);
    }

    /**
     * Division assignment operator
     */
    template<typename R, typename V = typename std::decay<R>::type>
    inline typename std::enable_if<std::is_base_of<ComplexMatrix, V>::value &&
                                   !std::is_base_of<RealMatrix, V>::value,
                                   Matrix<2, std::complex<decay_type>>>::type
    operator /= (R &&rhs)
    {
        return Matrix<2, std::complex<decay_type>>(*this).operator /= (rhs);
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return std::string("Matrix<2, ") + typeid(T).name() + ">";
    }

    /**
     * Function to perform matrix interpolation along rows or columns
     * @param x            abscissa vectors wrt to ordinates along the corresponding dimensions
     * @param xi           the abscissa associated with the desired interpolant
     * @param interpolator a function object which defines the interpolation to be performed; it accepts a
     *                     single abscissa corresponding to the desired interpolant as its first argument, a
     *                     vector of known abscissas as its second argument, and a vector of known ordinates as
     *                     its third argument
     * @param dimension    the dimension along which the interpolation will occur
     */
    template<typename Interpolator>
    Matrix<2, decay_type> interpolate(const std::vector<std::vector<decay_type>> &x, decay_type xi,
                                      Interpolator &&interpolator, const MatrixDimensionType &dimension =
                                      MatrixDimensionType::Enum::Rows) const
    {
        auto &&function = [&interpolator, &x, &xi] (auto &&y, auto && /* not used */)
                          {
                              return std::forward<Interpolator>(interpolator)(xi, x, y);
                          };

        return map(dimension, function);
    }

    /**
     * Function to perform matrix interpolation along rows or columns
     * @param x            abscissa vectors wrt to ordinates along the corresponding dimensions
     * @param xr           the row abscissa associated with the desired row interpolant
     * @param xc           the column abscissa associated with the desired column interpolant
     * @param interpolator a function object which defines the interpolation to be performed; it accepts a
     *                     single abscissa corresponding to the desired interpolant as its first argument, a
     *                     vector of known abscissas as its second argument, and a vector of known ordinates as
     *                     its third argument
     */
    template<typename Interpolator>
    Matrix<2, decay_type> interpolate(const std::vector<std::vector<decay_type>> &x, decay_type xr,
                                      decay_type xc, Interpolator &&interpolator) const
    {
        std::vector<decay_type> xi = { xr, xc };
        auto &&function = [&interpolator, &x, &xi] (auto &&y, auto && /* not used */)
                          {
                              return std::forward<Interpolator>(interpolator)(xi, x, y);
                          };

        return map(MatrixDimensionType::Enum::RowsAndColumns, function);
    }

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    virtual std::ostream &print(std::ostream &stream) const override
    {
        auto &&function = [] (std::ostream &stream, const decay_type &value) -> std::ostream &
                          {
                              auto prec = stream.precision();
                              if (prec > 0)
                                  std::setprecision(prec);

                              if (value == std::numeric_limits<decay_type>::infinity())
                                  stream << "+Inf"; // positive infinity
                              else if (value == -std::numeric_limits<decay_type>::infinity())
                                  stream << "-Inf"; // negative infinity
                              else if (value != value)
                                  stream << "NaN"; // NaN
                              else
                                  stream << value; // valid floating point value

                              return stream;
                          };

        return print(stream, function);
    }
};

}

}

}

#endif
