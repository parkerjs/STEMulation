#ifndef NUMERIC_MATRIX_ND_H
#define NUMERIC_MATRIX_ND_H

#include "arithmetic_attributes.h"
#include "arithmetic_matrix_operations.h"
#include "general_matrix_nd.h"
#include "numeric_matrix.h"

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * N-dimensional numeric data type matrix class
 */
template<std::size_t N, typename T>
class Matrix<N, T, NumericMatrix>
: public attributes::operators::arithmetic::ElementaryArithmeticAttributes<Matrix<N, T>>,
  public Matrix<N, T, GeneralMatrix>,
  public NumericMatrix,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Typedef declarations
     */
    typedef T data_type;
    typedef typename std::decay<T>::type decay_type;
    typedef NumericMatrix matrix_type;

    /**
     * Friend declarations
     */
    friend class Matrix<N, typename std::conditional<std::is_reference<T>::value,
                        decay_type, decay_type &>::type, NumericMatrix>;

    /**
     * Forwarding constructor
     */
    template<typename ... Args>
    Matrix(Args && ... args)
    : Matrix<N, T, GeneralMatrix>(std::forward<Args>(args) ...)
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
    inline auto &operator = (Arg &&arg)
    {
        Matrix<N, T, GeneralMatrix>::operator = (std::forward<Arg>(arg));

        return *this;
    }

    /**
     * Unary minus operator
     */
    Matrix<N, decay_type> operator - () const
    {
        Matrix<N, decay_type> result(this->m_subscripts);
        std::transform(this->cbegin(), this->cend(), result.begin(), std::negate<decay_type>());

        return result;
    }

    /**
     * Addition assignment operator
     */
    template<typename U, typename V = Matrix<N, decay_type>,
             typename Result = typename std::conditional<std::is_reference<T>::value, V, V &>::type>
    Result operator += (const Matrix<N, U> &rhs)
    {
        typedef Matrix<N, typename std::decay<U>::type> X;
        typename std::conditional<std::is_reference<U>::value, X, const X &>::type right(rhs);

        Result left(static_cast<Matrix<N, T> &>(*this));
        if (left.empty())
            left.initialize(right.dimensions());

        if (left.subscripts() == right.subscripts())
            for (std::size_t i = 0; i < left.size(); ++i)
                left[i] += right[i];
        else
        {
            attributes::concrete::StaticSynchronizable<Matrix<N, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Matrix addition failed." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<N, T>>::unlock(0);
        }

        return left;
    }

    /**
     * Subtraction assignment operator
     */
    template<typename U, typename V = Matrix<N, decay_type>,
             typename Result = typename std::conditional<std::is_reference<T>::value, V, V &>::type>
    Result operator -= (const Matrix<N, U> &rhs)
    {
        typedef Matrix<N, typename std::decay<U>::type> X;
        typename std::conditional<std::is_reference<U>::value, X, const X &>::type right(rhs);

        Result left(static_cast<Matrix<N, T> &>(*this));
        if (left.empty())
            left.initialize(right.dimensions());

        if (left.subscripts() == right.subscripts())
            for (std::size_t i = 0; i < left.size(); ++i)
                left[i] -= right[i];
        else
        {
            attributes::concrete::StaticSynchronizable<Matrix<N, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Matrix subtraction failed." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<N, T>>::unlock(0);
        }

        return left;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "Matrix<" + std::to_string(N) + ", " + typeid(T).name() + ">";
    }

    /**
     * Find the maximum elements along a given dimension
     * @param dimension the matrix dimension along which the calculation will occur
     */
    template<typename U = decay_type, typename std::enable_if<std::is_arithmetic<U>::value, int>::type = 0>
    Matrix<N - 1, decay_type> maximum(std::size_t dimension = 0) const
    {
        auto &&function = [] (auto &&vector, auto && /* not used */)
                          {
                              return *std::max_element(vector.cbegin(), vector.cend());
                          };

        return this->map(dimension, function);
    }

    /**
     * Find the minimum elements along a given dimension
     * @param dimension the matrix dimension along which the calculation will occur
     */
    template<typename U = decay_type, typename std::enable_if<std::is_arithmetic<U>::value, int>::type = 0>
    Matrix<N - 1, decay_type> minimum(std::size_t dimension = 0) const
    {
        auto &&function = [] (auto &&vector, auto && /* not used */)
                          {
                              return *std::min_element(vector.cbegin(), vector.cend());
                          };

        return this->map(dimension, function);
    }

    /**
     * Compute the product of elements along a given dimension
     * @param dimension the matrix dimension along which the calculation will occur
     */
    virtual Matrix<N - 1, decay_type> product(std::size_t dimension = 0) const final
    {
        auto &&function = [] (auto &&vector, auto && /* not used */)
                          {
                              return std::accumulate(vector.cbegin(),
                                                     vector.cend(), (decay_type)1,
                                                     std::multiplies<decay_type>());
                          };

        return this->map(dimension, function);
    }

    /**
     * Compute the sum of elements along a given dimension
     * @param dimension the matrix dimension along which the calculation will occur
     */
    virtual Matrix<N - 1, decay_type> sum(std::size_t dimension = 0) const final
    {
        auto &&function = [] (auto &&vector, auto && /* not used */)
                          {
                              return std::accumulate(vector.cbegin(), vector.cend(), (decay_type)0);
                          };

        return this->map(dimension, function);
    }
};

}

}

}

#endif
