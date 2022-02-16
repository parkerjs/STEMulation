#ifndef REFERENCE_MATRIX_ND_H
#define REFERENCE_MATRIX_ND_H

#include "forward_matrix.h"
#include "reference_matrix.h"
#include "reflective.h"
#include "subscript.h"
#include "variable_wrapper.h"

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * This class specialization implements an N-dimensional matrix whose elements are stored by reference
 */
template<std::size_t N, typename T>
class Matrix<N, T &, ReferenceMatrix>
: public Matrix<N, T &, typename MatrixClass<T>::type>,
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
     * @param matrix    the reference matrix object
     * @param subscript a reference to a Subscript object
     */
    template<typename U, typename V = T, typename std::enable_if<!std::is_reference<V>::value, int>::type = 0>
    Matrix(Matrix<N, T, U> &matrix, const sets::Subscript &subscript)
    {
        auto &&size = subscript.cardinality();
        std::vector<T *> vector(size);

        matrix.subscript().getVectorSubset(matrix, vector, subscript, [] (auto &arg) { return &arg; });

        this->m_vector.resize(vector.size());
        for (std::size_t i = 0; i < vector.size(); ++i)
            this->m_vector[i] = functional::VariableWrapper<T &>(*vector[i]);

        sets::Subscript::generate(subscript.cardinalities(), this->m_subscript);
    }

private:

    /**
     * Copy constructor
     */
    Matrix(Matrix<N, T &> &matrix)
    {
        operator = (matrix);
    }

public:

    /**
     * Move constructor
     */
    Matrix(Matrix<N, T &> &&matrix)
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
    Matrix<N, T &> &operator = (const Matrix<N, T &> &matrix)
    {
        if (&matrix != this)
        {
            Matrix<N, T &, typename MatrixClass<T>::type>::operator = (matrix);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Matrix<N, T &> &operator = (Matrix<N, T &> &&matrix)
    {
        if (&matrix != this)
        {
            Matrix<N, T &, typename MatrixClass<T>::type>::operator = (std::move(matrix));
        }

        return *this;
    }

    /**
     * Assignment operator
     */
    bool operator = (const Matrix<N, T> &matrix)
    {
        bool bSuccess = (this->dimensions() == matrix.dimensions());
        if (bSuccess)
            std::copy(matrix.cbegin(), matrix.cend(), this->begin());

        return bSuccess;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "Matrix<" + std::to_string(N) + ", " + typeid(T).name() + ">";
    }
};

}

}

}

#endif
