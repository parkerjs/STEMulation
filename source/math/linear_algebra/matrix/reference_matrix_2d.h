#ifndef REFERENCE_MATRIX_2D_H
#define REFERENCE_MATRIX_2D_H

#include "forward_matrix.h"
#include "reference_matrix.h"
#include "reflective.h"
#include "variable_wrapper.h"

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * This class specialization implements an 2-dimensional matrix whose elements are stored by reference
 */
template<typename T>
class Matrix<2, T &, ReferenceMatrix>
: public Matrix<2, T &, typename MatrixClass<T>::type>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Typedef declarations
     */
    typedef typename std::decay<T>::type decay_type;
    typedef T data_type;
    typedef typename MatrixClass<T>::type matrix_type;

    /**
     * Constructor
     * @param matrix        the reference matrix object
     * @param rowIndices    a vector of row indices to be addressed
     * @param columnIndices a vector of column indices to be addressed
     */
    template<typename U, typename V = T, typename std::enable_if<!std::is_reference<T>::value, int>::type = 0>
    Matrix(Matrix<2, T, U> &matrix,
           const std::vector<std::size_t> &rowIndices,
           const std::vector<std::size_t> &columnIndices)
    {
        this->m_rows = rowIndices.size();
        this->m_columns = columnIndices.size();
        this->m_vector.resize(this->m_rows * this->m_columns);

        for (std::size_t i = 0, k = 0; i < this->m_rows; ++i)
        {
            auto offset = rowIndices[i] * matrix.columns();
            for (std::size_t j = 0; j < this->m_columns; ++j, ++k)
                this->m_vector[k] = functional::VariableWrapper<T &>(matrix[offset + columnIndices[j]]);
        }
    }

private:

    /**
     * Copy constructor
     */
    Matrix(Matrix<2, T &> &matrix)
    {
        operator = (matrix);
    }

public:

    /**
     * Move constructor
     */
    Matrix(Matrix<2, T &> &&matrix)
    {
        operator = (std::move(matrix));
    }

    /**
     * Destructor
     */
    virtual ~Matrix(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Matrix<2, T &> &operator = (Matrix<2, T &> &matrix)
    {
        if (&matrix != this)
        {
            Matrix<2, T &, typename MatrixClass<T>::type>::operator = (matrix);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Matrix<2, T &> &operator = (Matrix<2, T &> &&matrix)
    {
        if (&matrix != this)
        {
            Matrix<2, T &, typename MatrixClass<T>::type>::operator = (std::move(matrix));
        }

        return *this;
    }

    /**
     * Assignment operator
     */
    bool operator = (const Matrix<2, T> &matrix)
    {
        bool bSuccess = (matrix.rows() == this->m_rows && matrix.columns() == this->m_columns);
        if (bSuccess)
            std::copy(matrix.cbegin(), matrix.cend(), this->begin());

        return bSuccess;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return std::string("Matrix<2, ") + typeid(T).name() + ">";
    }
};

}

}

}

#endif
