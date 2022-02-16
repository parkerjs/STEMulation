#ifndef REAL_MATRIX_ND_H
#define REAL_MATRIX_ND_H

#include "complex_matrix_nd.h"
#include "real_matrix.h"

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * N-dimensional real data type matrix class
 */
template<std::size_t N, typename T>
class Matrix<N, T, RealMatrix>
: public Matrix<N, T, ComplexMatrix>,
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
    using Matrix<N, T, ComplexMatrix>::operator +=;
    using Matrix<N, T, ComplexMatrix>::operator -=;
    using Matrix<N, T, ComplexMatrix>::print;

    /**
     * Forwarding constructor
     */
    template<typename ... Args>
    Matrix(Args && ... args)
    : Matrix<N, T, ComplexMatrix>(std::forward<Args>(args) ...)
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
        Matrix<N, T, ComplexMatrix>::operator = (std::forward<Arg>(arg));

        return *this;
    }

    /**
     * Addition assignment operator
     */
    template<typename R, typename V = typename std::decay<R>::type>
    inline typename std::enable_if<std::is_base_of<ComplexMatrix, V>::value &&
                                   !std::is_base_of<RealMatrix, V>::value,
                                   Matrix<N, std::complex<decay_type>>>::type
    operator += (R &&rhs)
    {
        return Matrix<N, std::complex<decay_type>>(*this).operator += (rhs);
    }

    /**
     * Subtraction assignment operator
     */
    template<typename R, typename V = typename std::decay<R>::type>
    inline typename std::enable_if<std::is_base_of<ComplexMatrix, V>::value &&
                                   !std::is_base_of<RealMatrix, V>::value,
                                   Matrix<N, std::complex<decay_type>>>::type
    operator -= (R &&rhs)
    {
        return Matrix<N, std::complex<decay_type>>(*this).operator -= (rhs);
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "Matrix<" + std::to_string(N) + ", " + typeid(T).name() + ">";
    }

    /**
     * Function to perform matrix interpolation; interpolation will be performed along the dimensions in
     * ascending order up to the number of elements in vector xi
     * @param x            abscissa vectors wrt to ordinates along the corresponding dimensions
     * @param xi           the n-tuple which corresponds to the desired interpolant
     * @param interpolator a function object which defines the interpolation to be performed; it accepts a
     *                     single abscissa corresponding to the desired interpolant as its first argument, a
     *                     vector of known abscissas as its second argument, and a vector of known ordinates as
     *                     its third argument
     */
    template<std::size_t M, typename Interpolator>
    Matrix<M, decay_type> interpolate(const std::vector<std::vector<decay_type>> &x,
                                      const std::vector<decay_type> &xi,
                                      Interpolator &&interpolator) const
    {
        std::vector<std::size_t> dimensions(std::min(M, xi.size()));
        std::iota(dimensions.begin(), dimensions.end(), 0);

        auto &&function = [&interpolator, &x, &xi] (auto &&y, auto &&i)
                          {
                              return std::forward<Interpolator>(interpolator)(xi[i], x[i], y);
                          };

        return this->map<M>(dimensions, function);
    }

    /**
     * Function to perform matrix interpolation
     * @param x            abscissa vectors wrt to ordinates along the corresponding dimensions
     * @param xi           the n-tuple which corresponds to the desired interpolant
     * @param dimensions   the matrix dimension(s) along which the calculation will occur
     * @param interpolator a function object which defines the interpolation to be performed; it accepts a
     *                     single abscissa corresponding to the desired interpolant as its first argument, a
     *                     vector of known abscissas as its second argument, and a vector of known ordinates as
     *                     its third argument
     */
    template<std::size_t M, typename Interpolator>
    Matrix<M, decay_type> interpolate(const std::vector<std::vector<decay_type>> &x,
                                      const std::vector<decay_type> &xi,
                                      const std::vector<std::size_t> &dimensions,
                                      Interpolator &&interpolator) const
    {
        auto &&function = [&interpolator, &x, &xi] (auto &&y, auto &&i)
                          {
                              return std::forward<Interpolator>(interpolator)(xi[i], x[i], y);
                          };

        return this->template map<M>(dimensions, function);
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
