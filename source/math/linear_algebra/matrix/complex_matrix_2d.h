#ifndef COMPLEX_MATRIX_2D_H
#define COMPLEX_MATRIX_2D_H

#include "complex.h"
#include "complex_matrix.h"
#include "conjugate.h"
#include "numeric_matrix_2d.h"

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * Two-dimensional complex data type matrix class
 */
template<typename T>
class Matrix<2, T, ComplexMatrix>
: public ComplexMatrix,
  public Matrix<2, T, NumericMatrix>,
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
    using Matrix<2, T, NumericMatrix>::print;

    /**
     * Forwarding constructor
     */
    template<typename ... Args>
    Matrix(Args && ... args)
    : Matrix<2, T, NumericMatrix>(std::forward<Args>(args) ...)
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
        Matrix<2, T, NumericMatrix>::operator = (std::forward<Arg>(arg));

        return *this;
    }

    /**
     * Retrieve the antisymmetric portion of a square matrix
     */
    Matrix<2, decay_type> antiSymmetric(void) const
    {
        Matrix<2, decay_type> result;
        if (!this->isSquare())
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Matrix is not square." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }
        else
        {
            result.initialize(this->m_rows, this->m_columns);
            for (std::size_t i = 0, k = 0; i < this->m_rows; ++i)
            {
                std::size_t offset = i;
                for (std::size_t j = 0; j < this->m_columns; ++j, ++k)
                {
                    result.m_vector[k] = 0.5 * (this->m_vector[k] - this->m_vector[offset]);
                    offset += this->m_rows;
                }
            }
        }

        return result;
    }

    /**
     * Construct a companion matrix
     * @param coeff an vector of coefficients corresponding a polynomial of the form:
       \verbatim

              (n)          (n-1)
         a   x    + a     x      + ... +  a   x + a
          (n)        (n-1)                 (1)     (0)
       \endverbatim
     *
     */
    static Matrix<2, decay_type> createCompanion(const std::vector<decay_type> &coefficients)
    {
        Matrix<2, decay_type> result;
        auto size = coefficients.size();
        if (!coefficients.empty())
        {
            result.initialize(size - 1, size - 1);
            for (std::size_t i = 0, offset = 0; i < size; ++i)
            {
                offset += size;
                result.m_vector[i] = -coefficients[i + 1] / coefficients[0];
                if (i > 0)
                    result.m_vector[offset - 1] = 1.0;
            }
        }
        else
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "A non-empty vector of coefficients must be supplied." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }

        return result;
    }

    /**
     * Compute the determinant of a square matrix
     */
    decay_type determinant(void) const
    {
        if (this->m_pLU_Solver == nullptr)
            this->m_pLU_Solver.reset(new decomposition::Doolittle_LU_Factor<Matrix<2, decay_type>>);

        Matrix<2, decay_type> LU(*this);
        auto result = std::numeric_limits<decay_type>::quiet_NaN();
        int iError = this->m_pLU_Solver->determinant(LU, result);
        if (iError < 0)
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Matrix is not square." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }

        return result;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return std::string("Matrix<2, ") + typeid(T).name() + ">";
    }

    /**
     * Compute the inverse of a square matrix
     */
    inline Matrix<2, decay_type> inverse(void) const
    {
        return Matrix<2, decay_type>(*this).invert();
    }

    /**
     * Invert the current object's square matrix
     */
    Matrix<2, decay_type> &invert(void)
    {
        if (this->m_pLU_Solver == nullptr)
            this->m_pLU_Solver.reset(new decomposition::Doolittle_LU_Factor<Matrix<2, decay_type>>);

        Matrix<2, decay_type> LU(*this);
        this->setDiagonal(1.0, 0.0);

        int iError = this->m_pLU_Solver->inverse(LU, *this);
        if (iError < 0)
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Matrix is not square." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }

        return *this;
    }

    /**
     * Compute the mean of elements along a given dimension
     * @param dimension the matrix dimension along which the calculation will occur
     */
    Matrix<2, decay_type> mean(const MatrixDimensionType &dimension = MatrixDimensionType::Enum::Rows) const
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
    Matrix<2, decay_type> standardDeviation(const MatrixDimensionType &dimension =
                                            MatrixDimensionType::Enum::Rows) const
    {
        auto &&result = variance(dimension);
        std::transform(result.cbegin(), result.cend(), result.begin(), std::sqrt);

        return result;
    }

    /**
     * Retrieve the symmetric part of a square matrix
     */
    Matrix<2, decay_type> symmetric(void) const
    {
        Matrix<2, decay_type> result;
        if (!this->isSquare())
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Matrix is not square." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }
        else
        {
            result.initialize(this->m_rows, this->m_columns);
            for (std::size_t i = 0, k = 0; i < this->m_rows; ++i)
            {
                auto offset = i;
                for (std::size_t j = 0; j < this->m_columns; ++j, ++k)
                {
                    result.m_vector[k] = 0.5 * (this->m_vector[k] + this->m_vector[offset]);
                    offset += this->m_rows;
                }
            }
        }

        return result;
    }

    /**
     * Matrix transpose
     */
    virtual void transpose(void) override
    {
        Matrix<2, T, NumericMatrix>::transpose();

        std::transform(this->cbegin(), this->cend(), this->begin(), number_systems::complex::Conjugate());
    }

    /**
     * Compute the variance of elements along rows or columns
     * @param dimension the matrix dimension along which the calculation will occur
     */
    Matrix<2, decay_type> variance(const MatrixDimensionType &dimension =
                                   MatrixDimensionType::Enum::Rows) const
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
