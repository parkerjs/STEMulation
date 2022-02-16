#include "doolittle_lu.h"
#include "matrix2d.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include <algorithm>
#include <cstring>
#include <functional>
#include <iomanip>
#include <limits>
#include <mutex>

// using namespace declarations
using namespace attributes::concrete;
using namespace math::combinatorics;
using namespace math::linear_algebra::matrix::decomposition;
#ifdef RAPID_XML
using namespace rapidxml;
#endif

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * Constructor
 */
Matrix2d::Matrix2d(void)
: m_columns(0),
  m_pTempMatrix(nullptr),
  m_rows(0)
{

}

/**
 * Constructor
 */
Matrix2d::Matrix2d(std::size_t rows,
                   std::size_t numCols)
: m_pTempMatrix(nullptr)
{
    initialize(rows, numCols);
}

/**
 * Implicit conversion from vector to column matrix
 */
Matrix2d::Matrix2d(const std::vector<double> &vec)
: m_pTempMatrix(nullptr)
{
    auto rows = vec.size();
    std::size_t cols = 1;

    initialize(rows, cols);

    std::copy(vec.cbegin(), vec.cend(), m_vector.begin());
}

/**
 * Copy constructor
 */
Matrix2d::Matrix2d(const Matrix2d &matrix)
: m_pTempMatrix(nullptr)
{
    operator = (matrix);
}

/**
 * Move constructor
 */
Matrix2d::Matrix2d(Matrix2d &&matrix)
: m_pTempMatrix(nullptr)
{
    operator = (std::move(matrix));
}

/**
 * Destructor
 */
Matrix2d::~Matrix2d(void)
{
    if (m_pTempMatrix != nullptr)
        delete m_pTempMatrix;
}

/**
 * Copy assignment operator
 */
Matrix2d &Matrix2d::operator = (const Matrix2d &matrix)
{
    if (&matrix != this)
    {
        m_columns = matrix.m_columns;
        m_rows = matrix.m_rows;
        m_vector = matrix.m_vector;
    }

    return *this;
}

/**
 * Move assignment operator
 */
Matrix2d &Matrix2d::operator = (Matrix2d &&matrix)
{
    if (&matrix != this)
    {
        matrix.swap(*this);
    }

    return *this;
}

/**
 * Return a vector of elements
 */
Matrix2d::operator std::vector<double> (void) const
{
    return m_vector;
}

/**
 * Subscript operator
 */
double &Matrix2d::operator [] (int index)
{
    return m_vector[index];
}

/**
 * Subscript operator
 */
double Matrix2d::operator [] (int index) const
{
    return m_vector[index];
}

/**
 * Function call operator
 */
double &Matrix2d::operator () (std::size_t row,
                               std::size_t col)
{
    return m_vector[row * m_columns + col];
}

/**
 * Function call operator
 */
double Matrix2d::operator () (std::size_t row,
                              std::size_t col) const
{
    return m_vector[row * m_columns + col];
}

/**
 * Equality operator
 */
bool Matrix2d::operator == (const Matrix2d &matrix) const
{
    return (m_columns == matrix.m_columns && m_rows == matrix.m_rows && m_vector == matrix.m_vector);
}

/**
 * Inequality operator
 */
bool Matrix2d::operator != (const Matrix2d &matrix) const
{
    return !operator == (matrix);
}

/**
 * Negation operator
 */
Matrix2d Matrix2d::operator - () const
{
    Matrix2d result(*this);

    result.negate();

    return result;
}

/**
 * Addition operator
 */
Matrix2d Matrix2d::operator + (const Matrix2d &matrix) const
{
    Matrix2d result(*this);

    result += matrix;

    return result;
}

/**
 * Addition operator
 */
Matrix2d Matrix2d::operator + (double value) const
{
    Matrix2d result(*this);

    result += value;

    return result;
}

/**
 * Addition operator
 */
Matrix2d operator + (double lhs, const Matrix2d &rhs)
{
    Matrix2d result(rhs);

    result += lhs;

    return result;
}

/**
 * Addition assignment operator
 */
Matrix2d &Matrix2d::operator += (const Matrix2d &matrix)
{
    if (m_columns == matrix.m_columns && m_rows == matrix.m_rows)
    {
        std::transform(m_vector.begin(),
                       m_vector.end(),
                       matrix.m_vector.begin(),
                       m_vector.begin(),
                       std::plus<double>());

        return *this;
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "Matrix dimensions do not agree.");
}

/**
 * Addition assignment operator
 */
Matrix2d &Matrix2d::operator += (double value)
{
    std::transform(m_vector.begin(),
                   m_vector.end(),
                   m_vector.begin(),
                   std::bind1st(std::plus<double>(), value));

    return *this;
}

/**
 * Subtraction operator
 */
Matrix2d Matrix2d::operator - (const Matrix2d &matrix) const
{
    Matrix2d result(*this);

    result -= matrix;

    return result;
}

/**
 * Subtraction operator
 */
Matrix2d Matrix2d::operator - (double value) const
{
    Matrix2d result(*this);

    result -= value;

    return result;
}

/**
 * Subtraction operator
 */
Matrix2d operator - (double lhs, const Matrix2d &rhs)
{
    Matrix2d result(rhs);

    result.negate();
    result += lhs;

    return result;
}

/**
 * Subtraction assignment operator
 */
Matrix2d &Matrix2d::operator -= (const Matrix2d &matrix)
{
    if (m_columns == matrix.m_columns && m_rows == matrix.m_rows)
    {
        std::transform(m_vector.begin(),
                       m_vector.end(),
                       matrix.m_vector.begin(),
                       m_vector.begin(),
                       std::minus<double>());

        return *this;
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "Matrix dimensions do not agree.");

    return *this;
}

/**
 * Addition assignment operator
 */
Matrix2d &Matrix2d::operator -= (double value)
{
    std::transform(m_vector.begin(),
                   m_vector.end(),
                   m_vector.begin(),
                   std::bind1st(std::minus<double>(), value));

    return *this;
}

/**
 * Multiplication operator
 */
Matrix2d Matrix2d::operator * (const Matrix2d &matrix) const
{
    Matrix2d result(*this);

    result *= matrix;

    return result;
}

/**
 * Multiplication operator (post multiplication with column vector)
 */
std::vector<double> Matrix2d::operator * (const std::vector<double> &vec) const
{
    Matrix2d result(*this);
    Matrix2d right(vec);

    result *= right;

    return result.m_vector;
}

/**
 * Multiplication operator (pre-multiplication with row vector)
 */
std::vector<double> operator * (const std::vector<double> &lhs,
                                const Matrix2d &rhs)
{
    Matrix2d left(lhs);

    left.transpose();
    left *= rhs;

    return left.m_vector;
}

/**
 * Multiplication operator
 */
Matrix2d Matrix2d::operator * (double value) const
{
    Matrix2d result(*this);

    result.scale(value);

    return result;
}

/**
 * Multiplication operator
 */
Matrix2d operator * (double lhs,
                     const Matrix2d &rhs)
{
    Matrix2d result(rhs);

    result.scale(lhs);

    return result;
}

/**
 * Multiplication assignment operator
 */
Matrix2d &Matrix2d::operator *= (const Matrix2d &matrix)
{
    if (m_columns == matrix.m_rows)
    {
        if (m_pTempMatrix == nullptr)
            m_pTempMatrix = new Matrix2d;

        *m_pTempMatrix = *this;
        Matrix2d::multiply(*m_pTempMatrix, matrix, *this);

        return *this;
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                             "Matrix dimensions do not agree.");
}

/**
 * Multiplication assignment operator
 */
Matrix2d &Matrix2d::operator *= (double value)
{
    std::transform(m_vector.begin(),
                   m_vector.end(),
                   m_vector.begin(),
                   std::bind1st(std::multiplies<double>(), value));

    return *this;
}

/**
 * Division operator
 */
Matrix2d Matrix2d::operator / (double value) const
{
    Matrix2d result(*this);

    result /= value;

    return result;
}

/**
 * Division assignment operator
 */
Matrix2d &Matrix2d::operator /= (double value)
{
    std::transform(m_vector.begin(),
                   m_vector.end(),
                   m_vector.begin(),
                   std::bind2nd(std::divides<double>(), value));

    return *this;
}

/**
 * Output stream operator
 */
std::ostream &operator << (std::ostream &stream,
                           const Matrix2d &matrix)
{
    auto width = stream.width();

    stream << std::setw(0) << "[";

    auto size = matrix.m_vector.size();
    for (std::size_t i = 0; i < size;)
    {
        if (i != 0 && i % matrix.m_columns == 0)
            stream << ";" << std::endl << " ";

        do
        {
            if (i % matrix.m_columns != 0)
                stream << ",";

            if (width > 0)
                stream << std::setw((int)width);

            auto prec = stream.precision();
            if (prec > 0)
                std::setprecision((int)prec);

            auto value = matrix.m_vector[i];
            if (value == std::numeric_limits<double>::infinity())
                stream << "+Inf"; // positive infinity
            else if (value == -std::numeric_limits<double>::infinity())
                stream << "-Inf"; // negative infinity
            else if (value != value)
                stream << "NaN"; // NaN
            else
                stream << value; // valid floating point value
        }
        while (++i % matrix.m_columns != 0);
    }

    stream << "];";

    return stream;
}

/**
 * begin() overload
 */
Matrix2d::iterator Matrix2d::begin(void)
{
    return m_vector.begin();
}

/**
 * Calculate the additive inverse
 */
Matrix2d Matrix2d::calcAdditiveInverse(void) const
{
    Matrix2d result(*this);

    result.negate();

    return result;
}

/**
 * Calculate the inverse matrix
 */
Matrix2d Matrix2d::calcInverse(void) const
{
    Matrix2d result(*this);

    result.invert();

    return result;
}

/**
 * Calculate the scaled matrix
 */
Matrix2d Matrix2d::calcScaled(double value) const
{
    Matrix2d result(*this);

    result.scale(value);

    return result;
}

/**
 * Calculate the transpose matrix
 */
Matrix2d Matrix2d::calcTranspose(void) const
{
    Matrix2d result(*this);

    result.transpose();

    return result;
}

/**
 * cbegin() overload
 */
Matrix2d::const_iterator Matrix2d::cbegin(void) const
{
    return m_vector.cbegin();
}

/**
 * cend() overload
 */
Matrix2d::const_iterator Matrix2d::cend(void) const
{
    return m_vector.cend();
}

/**
 * Get the j-th column
 */
Matrix2d Matrix2d::column(std::size_t j) const
{
    Matrix2d columnMatrix(m_rows, 1);

    column(j, columnMatrix);

    return columnMatrix;
}

/**
 * Get the j-th column
 * @param j            the index of the column to retrieve
 * @param columnMatrix upon success, will contain the result
 */
void Matrix2d::column(std::size_t j,
                      Matrix2d &columnMatrix) const
{
    column(j, columnMatrix.m_vector);
}

/**
 * Get the j-th column
 * @param j      the index of the column to retrieve
 * @param vector upon success, will contain the result
 */
void Matrix2d::column(std::size_t j,
                      std::vector<double> &vector) const
{
    if (j < m_columns)
    {
        vector.resize(m_rows);
        auto &&itThisVector = std::next(m_vector.cbegin(), j);
        auto &&itVector = vector.begin();
        while (itVector != vector.end())
        {
            *itVector++ = *itThisVector;
            itThisVector += m_columns;
        }
    }
    else
    {
        // this should not happen...
        throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                                 "Column index out of range.");
    }
}

/**
 * Get the number of columns in the matrix
 */
std::size_t Matrix2d::columns(void) const
{
    return m_columns;
}

/**
 * crbegin() overload
 */
Matrix2d::const_reverse_iterator Matrix2d::crbegin(void) const
{
    return m_vector.crbegin();
}

/**
 * crend() overload
 */
Matrix2d::const_reverse_iterator Matrix2d::crend(void) const
{
    return m_vector.crend();
}

/**
 * Function to deserialize this object's data
 */
std::istream &Matrix2d::deserialize(std::istream &stream)
{
    if (stream)
    {
        stream.read((char *)&m_rows, sizeof(std::size_t));
        stream.read((char *)&m_columns, sizeof(std::size_t));
        m_vector.resize(m_rows * m_columns);

        for (std::size_t i = 0; i < m_vector.size(); ++i)
            stream.read((char *)&m_vector[i], sizeof(double));
    }

    return stream;
}

/**
 * Compute the determinant of this matrix
 */
double Matrix2d::determinant(void) const
{
    auto result = std::numeric_limits<double>::quiet_NaN();

    if (m_pTempMatrix == nullptr)
        m_pTempMatrix = new Matrix2d;

    *m_pTempMatrix = *this;

    Doolittle_LU_Factor<Matrix2d> solver;
    int iErr = solver.determinant(*m_pTempMatrix, result);
    if (iErr < 0)
    {
        // this should not happen...
        throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                                 "An error occurred.");
    }

    return result;
}

/**
 * Test for empty matrix
 */
bool Matrix2d::empty(void) const
{
    return m_vector.empty();
}

/**
 * end() overload
 */
Matrix2d::iterator Matrix2d::end(void)
{
    return m_vector.end();
}

/**
 * Return a matrix containing a specified number of diagonals above, below and including the main diagonal from
 * the current matrix object.
 * @param kAbove the number of diagonals above the main diagonal
 * @param kBelow the number of diagonals below the main diagonal
 */
Matrix2d Matrix2d::getBand(int nAbove,
                           int nBelow) const
{
    Matrix2d result(*this);

    result.makeBand(nAbove, nBelow);

    return result;
}

/**
 * Get the name of this class
 */
std::string Matrix2d::getClassName(void) const
{
    return "Matrix2d";
}

/**
 * Retrieve a lower triangular matrix that includes elements below the k-th diagonal with respect to the main
 * diagonal of the matrix
 * @param  nAbove = 0 is the main diagonal
 *                > 0 is above the main diagonal
 *                < 0 is below the main diagonal.
 * @return        upon success, the lower triangular matrix
 */
Matrix2d Matrix2d::getLowerTriangle(int nAbove) const
{
    auto nBelow = 1 - int(m_rows);
    if (nAbove < nBelow)
        nBelow = nAbove;

    return getBand(nAbove, nBelow);
}

/**
 * Retrieve an upper triangular matrix that includes elements above the k-th diagonal with respect to the main
 * diagonal of the matrix
 * @param  nBelow = 0 is the main diagonal
 *                > 0 is above the main diagonal
 *                < 0 is below the main diagonal.
 * @return        upon success, the upper triangular matrix
 */
Matrix2d Matrix2d::getUpperTriangle(int nBelow) const
{
    int nAbove = m_columns - 1;
    if (nAbove < nBelow)
        nAbove = nBelow;

    return getBand(nAbove, nBelow);
}

/**
 * Function to construct an n x n identity matrix
 */
Matrix2d Matrix2d::identity(std::size_t dimension)
{
    Matrix2d matrix(dimension, dimension);
    ++dimension;
    for (std::size_t i = 0; i < matrix.size(); i += dimension)
        matrix.m_vector[i] = 1.0;

    return matrix;
}

/**
 * Initialization function
 */
void Matrix2d::initialize(std::size_t rows,
                          std::size_t cols)
{
    m_columns = cols;
    m_rows = rows;

    auto size = m_rows * m_columns;
    if (this->size() != size)
        m_vector.resize(size);
}

/**
 * Invert the current object's matrix and store in the current object
 */
void Matrix2d::invert(void)
{
    if (m_pTempMatrix == nullptr)
        m_pTempMatrix = new Matrix2d;

    *m_pTempMatrix = *this;

    setDiagonal(1.0, 0.0);

    Doolittle_LU_Factor<Matrix2d> solver;
    auto iErr = solver.inverse(*m_pTempMatrix, *this);
    if (iErr < 0)
    {
        // this should not happen...
        throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                                 "An error occurred.");
    }
}

/**
 * Test for column matrix
 */
bool Matrix2d::isColumnMatrix(void) const
{
    return m_columns == 1;
}

/**
 * Test for row matrix
 */
bool Matrix2d::isRowMatrix(void) const
{
    return m_rows == 1;
}

/**
 * Test whether or not matrix is square
 */
bool Matrix2d::isSquare(void) const
{
    return m_rows == m_columns;
}

/**
 * Morph the current object's matrix into a band matrix
 * @param kAbove the upper diagonal with respect to the main diagonal
 *               = 0 is the main diagonal
 *               > 0 is above the main diagonal
 *               < 0 is below the main diagonal.
 * @param kBelow the lower diagonal with respect to the main diagonal
 *               = 0 is the main diagonal
 *               > 0 is above the main diagonal
 *               < 0 is below the main diagonal.
 */
void Matrix2d::makeBand(int nAbove,
                        int nBelow)
{
    if (nBelow > nAbove)
        std::swap(nBelow, nAbove);

    auto size = this->size();
    for (std::size_t i = 0, icol = 0; icol < size; i++, icol += m_columns)
    {
        std::size_t minCol = std::max(0, int(i) + nBelow);
        minCol = std::min(m_columns, minCol);
        for (std::size_t j = 0; j < minCol; j++)
            m_vector[icol + j] = 0.0;

        std::size_t maxCol = std::min(int(m_columns), 1 + int(i) + nAbove);
        for (std::size_t j = maxCol; j < m_columns; j++)
            m_vector[icol + j] = 0.0;
    }
}

/**
 * Morph the current object's matrix into a lower triangular matrix that includes elements below the k-th
 * diagonal with respect to the main diagonal of the matrix
 * @param nAbove = 0 is the main diagonal
 *               > 0 is above the main diagonal
 *               < 0 is below the main diagonal
 */
void Matrix2d::makeLowerTriangular(int nAbove)
{
    auto nBelow = 1 - int(m_rows);
    if (nAbove < nBelow)
        nBelow = nAbove;

    makeBand(nAbove, nBelow);
}

/**
 * Morph the current object's matrix into an upper triangular matrix that includes elements above the k-th
 * diagonal with respect to the main diagonal of the matrix
 * @param nBelow = 0 is the main diagonal
 *               > 0 is above the main diagonal
 *               < 0 is below the main diagonal
 */
void Matrix2d::makeUpperTriangular(int nBelow)
{
    int nAbove = m_columns - 1;
    if (nAbove < nBelow)
        nAbove = nBelow;

    makeBand(nAbove, nBelow);
}

/**
 * Multiply the current object by the rhs argument
 */
Matrix2d Matrix2d::multiply(const Matrix2d &matrix)
{
    if (m_pTempMatrix == nullptr)
        m_pTempMatrix = new Matrix2d;

    multiply(matrix, *m_pTempMatrix);

    return *m_pTempMatrix;
}

/**
 * Multiply the current object by the rhs argument and store the product in the result argument
 */
void Matrix2d::multiply(const Matrix2d &matrix,
                        Matrix2d &result)
{
    Matrix2d::multiply(*this, matrix, result);
}

/**
 * Multiply the lhs argument by the rhs argument and store the product in the result argument
 */
void Matrix2d::multiply(const Matrix2d &lhs,
                        const Matrix2d &rhs,
                        Matrix2d &result,
                        Matrix2d::MultiplicationTransposeType multiplicationTransposeType)
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
        case Matrix2d::None:
        // do nothing
        break;

        case Matrix2d::PostMultiplyByTranspose:
        std::swap(rhsCols, rhsRows);
        std::swap(pRhsColIndex, pRhsRowIndex);
        rhsOffset = rhsRows;
        break;

        case Matrix2d::PreMultiplyByTranspose:
        std::swap(lhsCols, lhsRows);
        std::swap(pLhsColIndex, pLhsRowIndex);
        lhsOffset = lhsRows;
        break;

        case Matrix2d::TransposeBoth:
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
        if (pResult == &lhs || pResult == &rhs)
        {
            pResult = rhs.m_pTempMatrix;
            if (pResult == nullptr)
                pResult = new Matrix2d;
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
        {
            result = *pResult;
            if (pResult != rhs.m_pTempMatrix)
                delete pResult;
        }
    }
    else
    {
        // this should not happen...
        throw std::runtime_error("Exception thrown from " + getQualifiedStaticMethodName(__func__, Matrix2d) +
                                 ": Matrix dimensions do not agree.");
    }
}

/**
 * Negate the matrix
 */
Matrix2d &Matrix2d::negate(void)
{
    std::transform(m_vector.begin(),
                   m_vector.end(),
                   m_vector.begin(),
                   std::negate<double>());

    return *this;
}

/**
 * Calculate the outer product of two vectors and store the result in the input matrix
 */
void Matrix2d::outerProduct(const std::vector<double> &a,
                            const std::vector<double> &b,
                            Matrix2d &result)
{
    auto rows = a.size();
    auto cols = b.size();
    result.resize(rows, cols);

    for (std::size_t i = 0, icol = 0; i < rows; ++i, icol += cols)
        for (std::size_t j = 0; j < cols; ++j)
            result[icol + j] = a[i] * b[j];
}

/**
 * Permute columns of the matrix
 * @param permutation a permutation vector
 */
void Matrix2d::permuteColumns(const std::vector<std::size_t> &permutation)
{
    std::vector<std::size_t> work(m_columns);
    permuteColumns(permutation, work);
}

/**
 * Permute columns of the matrix
 * @param      permutation a permutation vector
 * @param[in]  work        a workspace vector
 * @param[out] work        has been destroyed
 */
void Matrix2d::permuteColumns(const std::vector<std::size_t> &permutation,
                              std::vector<std::size_t> &work)
{
    auto n = std::min(m_columns, permutation.size());
    for (std::size_t j = 0; j < n; j++)
    {
        if (permutation[j] >= m_columns)
        {
            // this should not happen...
            throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                                     "Column index out of bounds.");
        }
        else if (j != permutation[j])
        {
            Permutator::computeRelativePermutationVector(permutation, work);
            for (std::size_t i = 0, icol = 0; i < m_rows; i++)
            {
                std::swap(m_vector[icol + j], m_vector[icol + work[j]]);
                icol += m_columns;
            }
        }
    }
}

/**
 * Permute rows of the matrix
 * @param permutation a permutation vector
 */
void Matrix2d::permuteRows(const std::vector<std::size_t> &permutation)
{
    std::vector<std::size_t> work(m_rows);
    permuteRows(permutation, work);
}

/**
 * Permute rows of the matrix
 * @param      permutation a permutation vector
 * @param[in]  work        a workspace vector
 * @param[out] work        has been destroyed
 */
void Matrix2d::permuteRows(const std::vector<std::size_t> &permutation,
                           std::vector<std::size_t> &work)
{
    auto m = std::min(m_rows, permutation.size());
    std::size_t offset[2] = { 0, 0 };

    for (std::size_t i = 0; i < m; i++)
    {
        if (permutation[i] >= m_rows)
        {
            // this should not happen...
            throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                                     "Row index out of bounds.");
        }
        else if (i != permutation[i])
        {
            Permutator::computeRelativePermutationVector(permutation, work);
            offset[1] = work[i] * m_columns;

            for (std::size_t j = 0; j < m_columns; j++)
                std::swap(m_vector[offset[0] + j], m_vector[offset[1] + j]);
        }

        offset[0] += m_columns;
    }
}

/**
 * Post-multiply the current object by the transpose of the rhs argument
 */
Matrix2d Matrix2d::postMultiplyTranspose(const Matrix2d &matrix)
{
    if (m_pTempMatrix == nullptr)
        m_pTempMatrix = new Matrix2d;

    Matrix2d::multiply(*this, matrix, *m_pTempMatrix, PostMultiplyByTranspose);

    return *m_pTempMatrix;
}

/**
 * Post-multiply the current object by the transpose of the rhs argument and store the product in the result
 * argument
 */
void Matrix2d::postMultiplyTranspose(const Matrix2d &matrix,
                                     Matrix2d &result)
{
    Matrix2d::multiply(*this, matrix, result, PostMultiplyByTranspose);
}

/**
 * Post-multiply the lhs argument by the transpose of the rhs argument and store the product in the result
 * argument
 */
void postMultiplyTranspose(const Matrix2d &lhs,
                           const Matrix2d &rhs,
                           Matrix2d &result)
{
    Matrix2d::multiply(lhs, rhs, result, Matrix2d::PostMultiplyByTranspose);
}

/**
 * Pre-multiply the current object by the transpose of the lhs argument
 */
Matrix2d Matrix2d::preMultiplyTranspose(const Matrix2d &lhs)
{
    if (m_pTempMatrix == nullptr)
        m_pTempMatrix = new Matrix2d;

    Matrix2d::multiply(lhs, *this, *m_pTempMatrix, PreMultiplyByTranspose);

    return *m_pTempMatrix;
}

/**
 * Pre-multiply the current object by the transpose of the lhs argument and store the product in the result
 * argument
 */
void Matrix2d::preMultiplyTranspose(const Matrix2d &lhs,
                                    Matrix2d &result)
{
    Matrix2d::multiply(lhs, *this, result, PreMultiplyByTranspose);
}

/**
 * Pre-multiply the rhs argument by the transpose of the lhs argument and store the product in the result
 * argument
 */
void preMultiplyTranspose(const Matrix2d &lhs,
                          const Matrix2d &rhs,
                          Matrix2d &result)
{
    Matrix2d::multiply(lhs, rhs, result, Matrix2d::PreMultiplyByTranspose);
}

/**
 * rbegin() overload
 */
Matrix2d::reverse_iterator Matrix2d::rbegin(void)
{
    return m_vector.rbegin();
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool Matrix2d::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        m_columns = 0, m_rows = 0;
        std::vector<std::vector<double>> matrix;
        auto *pRowNode = pNode->first_node();
        while (bSuccess && pRowNode != nullptr)
        {
            auto rowString = "row" + std::to_string(m_rows);
            bSuccess = (std::strcmp(pRowNode->name(), rowString.c_str()) == 0);
            if (bSuccess)
            {
                std::size_t col = 0;
                matrix.push_back(std::vector<double>());
                auto *pColumnNode = pRowNode->first_node();
                while (bSuccess && pColumnNode != nullptr)
                {
                    auto colString = "col" + std::to_string(col);
                    bSuccess = (std::strcmp(pColumnNode->name(), colString.c_str()) == 0);
                    if (bSuccess)
                    {
                        auto &&value = pColumnNode->value();
                        bSuccess = (std::strlen(value) > 0);
                        if (bSuccess)
                            matrix.back().push_back(std::stod(pColumnNode->value()));

                        pColumnNode = pColumnNode->next_sibling();
                        ++col;
                    }
                }

                // test for each row having the same number of elements
                if (m_columns == 0)
                    m_columns = col;
                else if (m_columns != col)
                    bSuccess = false;
            }

            ++m_rows;
            pRowNode = pRowNode->next_sibling();
        }

        if (bSuccess)
        {
            m_vector.resize(m_rows * m_columns);
            for (std::size_t i = 0, icol = 0; i < m_rows; ++i, icol += m_columns)
                for (std::size_t j = 0; j < m_columns; ++j)
                    m_vector[icol + j] = matrix[i][j];
        }
    }

    return bSuccess;
}
#endif
/**
 * rend() overload
 */
Matrix2d::reverse_iterator Matrix2d::rend(void)
{
    return m_vector.rend();
}

/**
 * Resize the current matrix object
 * @param rows          the number of rows in the matrix
 * @param cols          the number of columns in the matrix
 * @param bPreserveData flag indicating whether or not to preserve the data in the matrix (optional, defaults
 *                      to true)
 */
void Matrix2d::resize(std::size_t rows,
                      std::size_t cols,
                      bool bPreserveData)
{
    if (rows != m_rows || cols != m_columns)
    {
        if (!bPreserveData)
            initialize(rows, cols);
        else
        {
            auto minrows = std::min(rows, m_rows);
            auto mincols = std::min(cols, m_columns);

            std::vector<double> vec(size());
            std::copy(m_vector.begin(), m_vector.end(), vec.begin());

            std::swap(cols, m_columns);
            initialize(rows, m_columns);

            std::size_t i_col[2] = { 0, 0 };
            for (std::size_t i = 0; i < minrows; ++i)
            {
                for (std::size_t j = 0; j < mincols; ++j)
                    m_vector[i_col[0] + j] = vec[i_col[1] + j];

                if (mincols < m_columns)
                {
                    std::vector<double>::iterator it = m_vector.begin();
                    std::advance(it, i_col[0]);
                    std::fill(it + mincols, it + m_columns, 0.0);
                }

                i_col[0] += m_columns, i_col[1] += cols;
            }
        }
    }
}

/**
 * Get the i-th row
 */
Matrix2d Matrix2d::row(std::size_t i) const
{
    Matrix2d rowMatrix(1, m_columns);

    row(i, rowMatrix);

    return rowMatrix;
}

/**
 * Get the i-th row
 * @param i         the index of the row to retrieve
 * @param rowMatrix upon success, will contain the result
 */
void Matrix2d::row(std::size_t i,
                   Matrix2d &rowMatrix) const
{
    row(i, rowMatrix.m_vector);
}

/**
 * Get the i-th row
 * @param i      the index of the row to retrieve
 * @param vector upon success, will contain the result
 */
void Matrix2d::row(std::size_t i,
                   std::vector<double> &vector) const
{
    if (i < m_rows)
    {
        vector.resize(m_columns);
        auto &&itThisVector = std::next(m_vector.cbegin(), i * m_columns);
        auto &&itVector = vector.begin();
        while (itVector != vector.end())
        {
            *itVector++ = *itThisVector++;
        }
    }
    else
    {
        // this should not happen...
        throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                                 "Row index out of range.");
    }
}

/**
 * Get the number of rows in the matrix
 */
std::size_t Matrix2d::rows(void) const
{
    return m_rows;
}

/**
 * Scale the matrix and store in the current object
 */
Matrix2d &Matrix2d::scale(double value)
{
    std::transform(m_vector.begin(),
                   m_vector.end(),
                   m_vector.begin(),
                   std::bind1st(std::multiplies<double>(), value));

    return *this;
}

/**
 * Function to serialize this object's data
 */
std::ostream &Matrix2d::serialize(std::ostream &stream) const
{
    if (stream)
    {
        stream.write((const char *)&m_rows, sizeof(std::size_t));
        stream.write((const char *)&m_columns, sizeof(std::size_t));

        for (std::size_t i = 0; i < m_vector.size(); ++i)
            stream.write((const char *)&m_vector[i], sizeof(double));
    }

    return stream;
}

/**
 * Assign elements along the main diagonal (i == j)
 * @param diagonal the diagonal elements
 */
void Matrix2d::setDiagonal(const std::vector<double> &diagonal)
{
    auto minDim = std::min(m_columns, m_rows);
    for (std::size_t i = 0, icol_i = 0; i < minDim; i++)
    {
        if (i < diagonal.size()) // diagonal element
            m_vector[icol_i] = diagonal[i];

        icol_i += m_columns + 1;
    }
}

/**
 * Assign elements along the main diagonal (i == j)
 * @param diagonal the desired diagonal element assignment
 */
void Matrix2d::setDiagonal(double diagonal)
{
    auto np1 = m_columns + 1;
    auto size = this->size();
    for (std::size_t icol_i = 0; icol_i < size; icol_i += np1)
        m_vector[icol_i] = diagonal;
}

/**
 * Assign elements along the main diagonal (i == j)
 * @param diagonal    the desired diagonal element assignment
 * @param offDiagonal the desired off-diagonal element assignment
 */
void Matrix2d::setDiagonal(double diagonal,
                           double offDiagonal)
{
    auto np1 = m_columns + 1;
    auto size = this->size();
    for (std::size_t i = 0, j = 0; i < size; i++)
    {
        if (i == j)
        {
            m_vector[i] = diagonal;
            j += np1;
        }
        else
            m_vector[i] = offDiagonal;
    }
}

/**
 * Return number of elements in matrix
 */
size_t Matrix2d::size(void) const
{
    return m_vector.size();
}

/**
 * Swap function
 */
void Matrix2d::swap(Matrix2d &matrix)
{
    std::swap(m_columns, matrix.m_columns);
    std::swap(m_pTempMatrix, matrix.m_pTempMatrix);
    std::swap(m_rows, matrix.m_rows);
    m_vector.swap(matrix.m_vector);
}

/**
 * Compute the transpose of this matrix and store in the current object
 */
Matrix2d &Matrix2d::transpose(void)
{
    if (m_pTempMatrix == nullptr)
        m_pTempMatrix = new Matrix2d;

    auto &workspace = m_pTempMatrix->m_vector;
    workspace.resize(m_vector.size());
    std::copy(m_vector.begin(), m_vector.end(), workspace.begin());

    for (std::size_t i = 0, icol = 0; i < m_rows; ++i, icol += m_columns)
        for (std::size_t j = 0, jrow = 0; j < m_columns; ++j, jrow += m_rows)
            m_vector[jrow + i] = workspace[icol + j];

    std::swap(m_rows, m_columns);

    return *this;
}
#ifdef RAPID_XML
/**
 * Function to write data to XML
 */
bool Matrix2d::writeToXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        auto *pDocument = pNode->document();
        bSuccess = (pDocument != nullptr);
        if (bSuccess)
        {
            for (std::size_t i = 0, icol = 0; bSuccess && i < m_rows; ++i, icol += m_columns)
            {
                auto rowString = "row" + std::to_string(i);
                auto *pRowString = pDocument->allocate_string(rowString.c_str());
                auto *pRowNode = pDocument->allocate_node(node_element, pRowString);
                for (std::size_t j = 0; bSuccess && j < m_columns; ++j)
                {
                    auto colString = "col" + std::to_string(j);
                    auto *pColString = pDocument->allocate_string(colString.c_str());
                    auto *pColNode = pDocument->allocate_node(node_element, pColString);
                    auto *pDataString = pDocument->allocate_string(std::to_string(m_vector[icol + j]).c_str());
                    auto *pDataNode = pDocument->allocate_node(node_data, pDataString);
                    pColNode->append_node(pDataNode);
                    pRowNode->append_node(pColNode);
                }

                pNode->append_node(pRowNode);
            }
        }
    }

    return bSuccess;
}
#endif
}

}

}
