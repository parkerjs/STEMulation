#ifndef NUMERIC_MATRIX_2D_H
#define NUMERIC_MATRIX_2D_H

#include "arithmetic_attributes.h"
#include "arithmetic_matrix_operations.h"
#include "doolittle_lu.h"
#include "general_matrix_2d.h"
#include "numeric_matrix.h"

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * Two-dimensional numeric data type matrix class
 */
template<typename T>
class Matrix<2, T, NumericMatrix>
: public attributes::operators::arithmetic::ElementaryArithmeticAttributes<Matrix<2, T>>,
  public Matrix<2, T, GeneralMatrix>,
  public NumericMatrix,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Enumerations
     */
    enum class MultiplicationTransposeType { None,
                                             PostMultiplyByTranspose,
                                             PreMultiplyByTranspose,
                                             TransposeBoth };

    /**
     * Typedef declarations
     */
    typedef T data_type;
    typedef typename std::decay<T>::type decay_type;
    typedef NumericMatrix matrix_type;

    /**
     * Friend declarations
     */
    friend class Matrix<2, typename std::conditional<std::is_reference<T>::value,
                        decay_type, decay_type &>::type, NumericMatrix>;

    /**
     * Forwarding constructor
     */
    template<typename ... Args>
    Matrix(Args && ... args)
    : Matrix<2, T, GeneralMatrix>(std::forward<Args>(args) ...),
      m_pLU_Solver(nullptr)
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
        Matrix<2, T, GeneralMatrix>::operator = (std::forward<Arg>(arg));

        return *this;
    }

    /**
     * Unary minus operator
     */
    Matrix<2, decay_type> operator - () const
    {
        Matrix<2, decay_type> result(this->m_rows, this->m_columns);
        std::transform(this->cbegin(), this->cend(), result.begin(), std::negate<decay_type>());

        return result;
    }

    /**
     * Addition assignment operator
     */
    template<typename U, typename V = Matrix<2, decay_type>,
             typename Result = typename std::conditional<std::is_reference<T>::value, V, V &>::type>
    Result operator += (const Matrix<2, U> &rhs)
    {
        typedef Matrix<2, typename std::decay<U>::type> X;
        typename std::conditional<std::is_reference<U>::value, X, const X &>::type right(rhs);

        Result left(static_cast<Matrix<2, T> &>(*this));
        if (left.empty())
            left.initialize(right.rows(), right.columns());

        if (left.rows() != right.rows() && left.columns() != right.columns())
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Matrix addition failed." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }
        else
            std::transform(left.cbegin(), left.cend(), right.cbegin(), left.begin(), std::plus<decay_type>());

        return left;
    }

    /**
     * Subtraction assignment operator
     */
    template<typename U, typename V = Matrix<2, decay_type>,
             typename Result = typename std::conditional<std::is_reference<T>::value, V, V &>::type>
    Result operator -= (const Matrix<2, U> &rhs)
    {
        typedef Matrix<2, typename std::decay<U>::type> X;
        typename std::conditional<std::is_reference<U>::value, X, const X &>::type right(rhs);

        Result left(static_cast<Matrix<2, T> &>(*this));
        if (left.empty())
            left.initialize(right.rows(), right.columns());

        if (left.rows() != right.rows() && left.columns() != right.columns())
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Matrix subtraction failed." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }
        else
            std::transform(left.cbegin(), left.cend(), right.cbegin(), left.begin(), std::minus<decay_type>());

        return left;
    }

    /**
     * Multiplication assignment operator
     */
    template<typename U, typename V = Matrix<2, decay_type>,
             typename Result = typename std::conditional<std::is_reference<T>::value, V, V &>::type>
    Result operator *= (const Matrix<2, U> &rhs)
    {
        typedef Matrix<2, typename std::decay<U>::type> X;
        typename std::conditional<std::is_reference<U>::value, X, const X &>::type right(rhs);

        if (this->m_pTempMatrix == nullptr)
            this->m_pTempMatrix.reset(new Matrix<2, decay_type>);

        Result left(static_cast<Matrix<2, T> &>(*this));
        if (right.isSquare())
        {
            *this->m_pTempMatrix = left;
            multiply(*this->m_pTempMatrix, rhs, left);
        }
        else
        {
            Matrix<2, decay_type> result(left.rows(), right.columns());
            left.multiply(right, result);
            left = std::move(result);
        }

        return left;
    }

    /**
     * Division assignment operator
     */
    template<typename U, typename V = Matrix<2, decay_type>,
             typename Result = typename std::conditional<std::is_reference<T>::value, V, V &>::type>
    Result operator /= (const Matrix<2, U> &rhs)
    {
        typedef Matrix<2, typename std::decay<U>::type> X;
        typename std::conditional<std::is_reference<U>::value, X, const X &>::type right(rhs);
        Result left(static_cast<Matrix<2, T> &>(*this));

        if (m_pLU_Solver == nullptr)
            m_pLU_Solver.reset(new decomposition::Doolittle_LU_Factor<decay_type>);

        int iError = m_pLU_Solver->solve(left, left, right);
        if (iError < 0)
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Matrix division failed." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }

        return left;
    }

    /**
     * Construct an n x n identity matrix
     */
    static Matrix<2, decay_type> createIdentity(std::size_t n)
    {
        Matrix<2, decay_type> result(n, n);
        auto size = result.m_vector.size();
        auto np1 = n + 1;
        for (std::size_t offset = 0; offset < size; offset += np1)
            result.m_vector[offset] = 1.0;

        return result;
    }

    /**
     * Create a Vandermonde matrix from the input vector
     */
    static Matrix<2, decay_type> createVandermonde(const std::vector<decay_type> &vector)
    {
        auto n = vector.size();
        Matrix<2, decay_type> result(n, n);
        for (std::size_t i = 0, offset = 0; i < n; ++i)
        {
            for (std::size_t j = 0; j < n; ++j)
            {
                result.m_vector[offset + j] = 1.0;
                for (std::size_t k = 0; k < j; ++k)
                    result.m_vector[offset + j] *= vector[i];
            }

            offset += vector.size();
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

protected:

    /**
     * Map the elements along the specified matrix dimension(s) to (a) single value(s) via function object
     * @param dimension the matrix dimensions along which the function object will be applied
     * @param function  a binary function object which accepts a vector of indices along a dimension as its
     *                  first argument and the index of the associated dimension as its second argument
     */
    template<typename Function>
    Matrix<2, decay_type> map(const MatrixDimensionType &dimension, Function &&function) const
    {
        Matrix<2, decay_type> result;
        switch (dimension)
        {
            default:
            case MatrixDimensionType::Enum::Columns:
            case MatrixDimensionType::Enum::Unknown:
            break;

            case MatrixDimensionType::Enum::Rows:
            case MatrixDimensionType::Enum::RowsAndColumns:
            result = Matrix<2, T, GeneralMatrix>::map(dimension, std::forward<Function>(function));
            break;
        }

        const Matrix<2, decay_type> *pResult = &result;
        switch (dimension)
        {
            case MatrixDimensionType::Enum::Columns:
            pResult = static_cast<const Matrix<2, decay_type> *>(this);
            case MatrixDimensionType::Enum::RowsAndColumns:
            result = pResult->Matrix<2, T, GeneralMatrix>::map(dimension, std::forward<Function>(function));
            case MatrixDimensionType::Enum::Rows:
            case MatrixDimensionType::Enum::Unknown:
            default:
            break;
        }

        return result;
    }

public:

    /**
     * Find the maximum elements along a given dimension
     * @param dimension the matrix dimension along which the calculation will occur
     */
    template<typename U = decay_type, typename std::enable_if<std::is_arithmetic<U>::value, int>::type = 0>
    Matrix<2, decay_type> max(const MatrixDimensionType &dimension = MatrixDimensionType::Enum::Rows) const
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
    Matrix<2, decay_type> minimum(const MatrixDimensionType &dimension = MatrixDimensionType::Enum::Rows) const
    {
        auto &&function = [] (auto &&vector, auto && /* not used */)
                          {
                              return *std::min_element(vector.cbegin(), vector.cend());
                          };

        return this->map(dimension, function);
    }

    /**
     * Multiply the current object by the rhs argument
     */
    Matrix<2, decay_type> multiply(const Matrix<2, decay_type> &rhs)
    {
        if (this->m_pTempMatrix == nullptr)
            this->m_pTempMatrix.reset(new Matrix<2, decay_type>);

        multiply(rhs, *this->m_pTempMatrix);

        return *this->m_pTempMatrix;
    }

    /**
     * Multiply the current object by the rhs argument and store the product in the result argument
     */
    void multiply(const Matrix<2, decay_type> &rhs, Matrix<2, decay_type> &result)
    {
        multiply(*this, rhs, result);
    }

protected:

    /**
     * Multiply the lhs argument by the rhs argument and store the product in the result argument
     * @param      lhs                         the left-hand side matrix
     * @param      rhs                         the right-hand side matrix
     * @param      multiplicationTransposeType the type of transposition to be performed during the
     *                                         multiplication
     * @param[out] result                      if successful, will be populated by the product of the two
     *                                         matrices
     */
    static void multiply(const Matrix<2, decay_type> &lhs, const Matrix<2, decay_type> &rhs,
                         Matrix<2, decay_type> &result, const MultiplicationTransposeType &
                         multiplicationTransposeType = MultiplicationTransposeType::None)
    {
        auto lhsCols = lhs.m_columns;
        auto lhsRows = lhs.m_rows;
        auto rhsCols = rhs.m_columns;
        auto rhsRows = rhs.m_rows;

        std::size_t i, j, k;
        auto *pLhsColIndex = &k;
        auto *pLhsRowIndex = &i;
        auto *pRhsColIndex = &j;
        auto *pRhsRowIndex = &k;
        auto lhsOffset = lhsCols;
        auto rhsOffset = rhsCols;
        switch (multiplicationTransposeType)
        {
            default:
            case MultiplicationTransposeType::None:
            // do nothing
            break;

            case MultiplicationTransposeType::PostMultiplyByTranspose:
            std::swap(rhsCols, rhsRows);
            std::swap(pRhsColIndex, pRhsRowIndex);
            rhsOffset = rhsRows;
            break;

            case MultiplicationTransposeType::PreMultiplyByTranspose:
            std::swap(lhsCols, lhsRows);
            std::swap(pLhsColIndex, pLhsRowIndex);
            lhsOffset = lhsRows;
            break;

            case MultiplicationTransposeType::TransposeBoth:
            std::swap(lhsCols, lhsRows);
            std::swap(pLhsColIndex, pLhsRowIndex);
            std::swap(rhsCols, rhsRows);
            std::swap(pRhsColIndex, pRhsRowIndex);
            lhsOffset = lhsRows;
            rhsOffset = rhsRows;
            break;
        }

        if (lhsCols == rhsRows)
        {
            auto *pResult = &result;
            std::shared_ptr<Matrix<2, decay_type>> pTempResult(nullptr);
            if (pResult == &lhs || pResult == &rhs)
            {
                pResult = rhs.m_pTempMatrix.get();
                if (pResult == nullptr)
                {
                    pTempResult.reset(new Matrix<2, decay_type>);
                    pResult = pTempResult.get();
                }
            }

            pResult->resize(lhsRows, rhsCols);
            for (i = 0; i < lhsRows; ++i)
            {
                for (j = 0; j < rhsCols; ++j)
                {
                    auto &result_ij = pResult->m_vector[i * rhsCols + j];
                    result_ij = 0.0;
                    for (k = 0; k < lhsCols; ++k)
                        result_ij += lhs.m_vector[*pLhsRowIndex * lhsOffset + *pLhsColIndex]
                                   * rhs.m_vector[*pRhsRowIndex * rhsOffset + *pRhsColIndex];
                }
            }

            if (pResult != &result)
                result = *pResult;
        }
        else
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Warning from "
                      << getQualifiedMethodName(__func__, std::string("Matrix<2, ") + typeid(T).name() + ">")
                      << ": " << "Matrix dimensions do not agree." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }
    }

public:

    /**
     * Post-multiply the current object by the transpose of the rhs argument
     */
    Matrix<2, decay_type> postMultiplyTranspose(const Matrix<2, decay_type> &rhs)
    {
        if (this->m_pTempMatrix == nullptr)
            this->m_pTempMatrix.reset(new Matrix<2, decay_type>);

        multiply(*this, rhs, *this->m_pTempMatrix, MultiplicationTransposeType::PostMultiplyByTranspose);

        return *this->m_pTempMatrix;
    }

    /**
     * Post-multiply the current object by the transpose of the rhs argument and store the product in the
     * result argument
     */
    void postMultiplyTranspose(const Matrix<2, decay_type> &rhs, Matrix<2, decay_type> &result)
    {
        multiply(*this, rhs, result, MultiplicationTransposeType::PostMultiplyByTranspose);
    }

    /**
     * Post-multiply the lhs argument by the transpose of the rhs argument and store the product in the result
     * argument
     */
    static void postMultiplyTranspose(const Matrix<2, decay_type> &lhs, const Matrix<2, decay_type> &rhs,
                                      Matrix<2, decay_type> &result)
    {
        multiply(lhs, rhs, result, MultiplicationTransposeType::PostMultiplyByTranspose);
    }

    /**
     * Pre-multiply the current object by the transpose of the lhs argument
     */
    Matrix<2, decay_type> preMultiplyTranspose(const Matrix<2, decay_type> &lhs)
    {
        if (this->m_pTempMatrix == nullptr)
            this->m_pTempMatrix.reset(new Matrix<2, decay_type>);

        multiply(lhs, *this, *this->m_pTempMatrix, MultiplicationTransposeType::PreMultiplyByTranspose);

        return *this->m_pTempMatrix;
    }

    /**
     * Pre-multiply the current object by the transpose of the lhs argument and store the product in the result
     * argument
     */
    void preMultiplyTranspose(const Matrix<2, decay_type> &lhs, Matrix<2, decay_type> &result)
    {
        multiply(lhs, *this, result, MultiplicationTransposeType::PreMultiplyByTranspose);
    }

    /**
     * Pre-multiply the rhs argument by the transpose of the lhs argument and store the product in the result
     * argument
     */
    static void preMultiplyTranspose(const Matrix<2, decay_type> &lhs, const Matrix<2, decay_type> &rhs,
                                     Matrix<2, decay_type> &result)
    {
        multiply(lhs, rhs, result, MultiplicationTransposeType::PreMultiplyByTranspose);
    }

    /**
     * Compute the product of elements along a given dimension
     * @param dimension the matrix dimension along which the calculation will occur
     */
    Matrix<2, decay_type> product(const MatrixDimensionType &dimension =
                                  MatrixDimensionType::Enum::Rows) const
    {
        auto &&function = [] (auto &&vector, auto && /* not used */)
                          {
                              return std::accumulate(vector.cbegin(), vector.cend(), (T)1,
                                                     std::multiplies<T>());
                          };

        return this->map(dimension, function);
    }

    /**
     * Compute the sum of elements along a given dimension
     * @param dimension the matrix dimension along which the calculation will occur
     */
    Matrix<2, decay_type> sum(const MatrixDimensionType &dimension =
                              MatrixDimensionType::Enum::Rows) const
    {
        auto &&function = [] (auto &&vector, auto && /* not used */)
                          {
                              return std::accumulate(vector.cbegin(), vector.cend(), (T)0);
                          };

        return this->map(dimension, function);
    }

    /**
     * Calculate the trace
     */
    decay_type trace(void) const
    {
        if (this->isSquare())
        {
            decay_type result = 0;
            for (std::size_t i = 0, offset = 0; i < this->m_rows; ++i)
            {
                result += this->m_vector[offset + i];
                offset += this->m_rows;
            }

            return result;
        }
        else
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Could not compute trace, matrix is not square." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }

        return std::numeric_limits<decay_type>::quiet_NaN();
    }

protected:

    /**
     * pointer to LU solver
     */
    mutable std::shared_ptr<decomposition::LU_Factor<Matrix<2, decay_type>>> m_pLU_Solver;
};

}

}

}

#endif
