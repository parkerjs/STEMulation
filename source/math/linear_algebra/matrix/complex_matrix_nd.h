#ifndef COMPLEX_MATRIX_ND_H
#define COMPLEX_MATRIX_ND_H

#include "complex.h"
#include "complex_matrix.h"
#include "numeric_matrix_nd.h"

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * N-dimensional complex data type matrix class
 */
template<std::size_t N, typename T>
class Matrix<N, T, ComplexMatrix>
: public ComplexMatrix,
  public Matrix<N, T, NumericMatrix>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Typedef declarations
     */
    typedef T data_type;
    typedef typename std::decay<T>::type decay_type;
    typedef ComplexMatrix matrix_type;

    /**
     * Using declarations
     */
    using Matrix<N, T, NumericMatrix>::print;

    /**
     * Forwarding constructor
     */
    template<typename ... Args>
    Matrix(Args && ... args)
    : Matrix<N, T, NumericMatrix>(std::forward<Args>(args) ...)
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
        Matrix<N, T, NumericMatrix>::operator = (std::forward<Arg>(arg));

        return *this;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "Matrix<" + std::to_string(N) + ", " + typeid(T).name() + ">";
    }

    /**
     * Compute the mean of elements along a given dimension
     * @param dimension the matrix dimension along which the calculation will occur
     */
    Matrix<N - 1, decay_type> mean(std::size_t dimension = 0) const
    {
        auto &&function = [] (auto &&vector, auto &&/* not used */)
        {
            return std::accumulate(vector.cbegin(), vector.cend(), T(0)) / vector.size();
        };

        return this->map(dimension, function);
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

            return stream << number_systems::complex::Complex<double>(value);
        };

        return print(stream, function);
    }

    /**
     * Compute the standard deviation of elements along a given dimension
     * @param dimension the matrix dimension along which the calculation will occur
     */
    Matrix<N - 1, decay_type> standardDeviation(std::size_t dimension = 0) const
    {
        auto &&result = variance(dimension);
        std::transform(result.cbegin(), result.cend(), result.begin(), std::sqrt);

        return result;
    }

    /**
     * Compute the variance of elements along rows or columns
     * @param dimension the matrix dimension along which the calculation will occur
     */
    Matrix<N - 1, decay_type> variance(std::size_t dimension = 0) const
    {
        auto &&function = [] (auto &&vector, auto &&/* not used */)
        {
            auto &&mean = std::accumulate(vector.cbegin(), vector.cend(), (T)0) / vector.size();
            std::vector<T> r(vector.size());

            std::transform(vector.cbegin(), vector.cend(), r.begin(), [&mean] (auto &&x) { return x - mean; });

            return std::inner_product(r.cbegin(), r.cend(), r.begin(), (T)0, std::plus<T>(),
                                      std::multiplies<T>()) / (r.size() - 1);
        };

        return this->map(dimension, function);
    }
};

}

}

}

#endif
