#ifndef MATRIX_2D_H
#define MATRIX_2D_H

#include "export_library.h"
#include "inherited_iterator.h"
#include "iterable.h"
#include "permutator.h"
#include "reflective.h"
#include "reverse_iterable.h"
#include "serializable.h"
#include "swappable.h"
#ifdef RAPID_XML
#include "xml_readable.h"
#include "xml_writable.h"
#endif
#include <memory>
#include <vector>

// forward declarations
#ifdef RAPID_XML
namespace rapidxml { template<class> class xml_node; }
#endif

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * This class implements a two-dimensional matrix class
 */
class Matrix2d
: public attributes::abstract::Iterable<iterators::Iterator, std::vector<double>,
                                        iterators::inherited_iterator_tag>,
  virtual private attributes::abstract::Reflective,
  public attributes::abstract::ReverseIterable<iterators::Iterator, std::vector<double>,
                                               iterators::inherited_iterator_tag>,
  public attributes::interfaces::Serializable,
#ifdef RAPID_XML
  public attributes::interfaces::Swappable<Matrix2d>,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>,
  public attributes::interfaces::XML_Writable<rapidxml::xml_node<char>>
#else
  public attributes::interfaces::Swappable<Matrix2d>
#endif
{
public:

    /**
     * Enumerations
     */
    enum MultiplicationTransposeType { None,
                                       PostMultiplyByTranspose,
                                       PreMultiplyByTranspose,
                                       TransposeBoth };

    /**
     * Typedef declarations
     */
    typedef typename attributes::abstract::Iterable<iterators::Iterator, std::vector<double>,
                     iterators::inherited_iterator_tag>::const_iterator const_iterator;
    typedef typename attributes::abstract::ReverseIterable<iterators::Iterator, std::vector<double>,
                     iterators::inherited_iterator_tag>::const_reverse_iterator const_reverse_iterator;
    typedef typename attributes::abstract::Iterable<iterators::Iterator, std::vector<double>,
                     iterators::inherited_iterator_tag>::iterator iterator;
    typedef typename attributes::abstract::ReverseIterable<iterators::Iterator, std::vector<double>,
                     iterators::inherited_iterator_tag>::reverse_iterator reverse_iterator;

    /**
     * Typedef declarations
     */
    typedef double decay_type;

    /**
     * Constructor
     */
    EXPORT_STEM Matrix2d(void);

    /**
     * Constructor
     */
    EXPORT_STEM Matrix2d(std::size_t rows,
                         std::size_t cols);

    /**
     * Implicit conversion from vector to column matrix
     */
    EXPORT_STEM Matrix2d(const std::vector<double> &vec);

    /**
     * Copy constructor
     */
    EXPORT_STEM Matrix2d(const Matrix2d &matrix);

    /**
     * Move constructor
     */
    EXPORT_STEM Matrix2d(Matrix2d &&matrix);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Matrix2d(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Matrix2d &operator = (const Matrix2d &matrix);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Matrix2d &operator = (Matrix2d &&matrix);

    /**
     * Return a vector of elements
     */
    EXPORT_STEM operator std::vector<double> (void) const;

    /**
     * Subscript operator
     */
    EXPORT_STEM double &operator [] (int index);

    /**
     * Subscript operator
     */
    EXPORT_STEM double operator [] (int index) const;

    /**
     * Function call operator
     */
    EXPORT_STEM double &operator () (std::size_t row,
                                     std::size_t col = 0);

    /**
     * Function call operator
     */
    EXPORT_STEM double operator () (std::size_t row,
                                    std::size_t col) const;

    /**
     * Equality operator
     */
    EXPORT_STEM bool operator == (const Matrix2d &matrix) const;

    /**
     * Inequality operator
     */
    EXPORT_STEM bool operator != (const Matrix2d &matrix) const;

    /**
     * Negation operator
     */
    EXPORT_STEM Matrix2d operator - () const;

    /**
     * Addition operator
     */
    EXPORT_STEM Matrix2d operator + (const Matrix2d &matrix) const;

    /**
     * Addition operator
     */
    EXPORT_STEM Matrix2d operator + (double value) const;

    /**
     * Addition operator
     */
    friend EXPORT_STEM Matrix2d operator + (double lhs,
                                            const Matrix2d &rhs);

    /**
     * Addition assignment operator
     */
    EXPORT_STEM Matrix2d &operator += (const Matrix2d &matrix);

    /**
     * Addition assignment operator
     */
    EXPORT_STEM Matrix2d &operator += (double value);

    /**
     * Subtraction operator
     */
    EXPORT_STEM Matrix2d operator - (const Matrix2d &matrix) const;

    /**
     * Subtraction operator
     */
    EXPORT_STEM Matrix2d operator - (double value) const;

    /**
     * Subtraction operator
     */
    friend EXPORT_STEM Matrix2d operator - (double lhs,
                                            const Matrix2d &rhs);

    /**
     * Subtraction assignment operator
     */
    EXPORT_STEM Matrix2d &operator -= (const Matrix2d &matrix);

    /**
     * Addition assignment operator
     */
    EXPORT_STEM Matrix2d &operator -= (double value);

    /**
     * Multiplication operator
     */
    EXPORT_STEM Matrix2d operator * (const Matrix2d &matrix) const;

    /**
     * Multiplication operator (post multiplication with column vector)
     */
    EXPORT_STEM std::vector<double> operator * (const std::vector<double> &vec) const;

    /**
     * Multiplication operator (pre-multiplication with row vector)
     */
    friend EXPORT_STEM std::vector<double> operator * (const std::vector<double> &lhs,
                                                       const Matrix2d &rhs);

    /**
     * Multiplication operator
     */
    EXPORT_STEM Matrix2d operator * (double value) const;

    /**
     * Multiplication operator
     */
    friend EXPORT_STEM Matrix2d operator * (double lhs,
                                            const Matrix2d &rhs);

    /**
     * Multiplication assignment operator
     */
    EXPORT_STEM Matrix2d &operator *= (const Matrix2d &matrix);

    /**
     * Multiplication assignment operator
     */
    EXPORT_STEM Matrix2d &operator *= (double value);

    /**
     * Division operator
     */
    EXPORT_STEM Matrix2d operator / (double value) const;

    /**
     * Division assignment operator
     */
    EXPORT_STEM Matrix2d &operator /= (double value);

    /**
     * Output stream operator
     */
    friend EXPORT_STEM std::ostream &operator << (std::ostream &stream,
                                                  const Matrix2d &matrix);

    /**
     * begin() overload
     */
    EXPORT_STEM virtual iterator begin(void) override;

    /**
     * Calculate the additive inverse
     */
    EXPORT_STEM Matrix2d calcAdditiveInverse(void) const;

    /**
     * Calculate the inverse matrix
     */
    EXPORT_STEM Matrix2d calcInverse(void) const;

    /**
     * Calculate the scaled matrix
     */
    EXPORT_STEM Matrix2d calcScaled(double value) const;

    /**
     * Calculate the transpose matrix
     */
    EXPORT_STEM Matrix2d calcTranspose(void) const;

    /**
     * cbegin() overload
     */
    EXPORT_STEM virtual const_iterator cbegin(void) const override;

    /**
     * cend() overload
     */
    EXPORT_STEM virtual const_iterator cend(void) const override;

    /**
     * Get the j-th column
     */
    EXPORT_STEM Matrix2d column(std::size_t j) const;

    /**
     * Get the j-th column
     * @param j            the index of the column to retrieve
     * @param columnMatrix upon success, will contain the result
     */
    EXPORT_STEM void column(std::size_t j,
                            Matrix2d &columnMatrix) const;

    /**
     * Get the j-th column
     * @param j      the index of the column to retrieve
     * @param vector upon success, will contain the result
     */
    EXPORT_STEM void column(std::size_t j,
                            std::vector<double> &vector) const;

    /**
     * Get the number of columns in the matrix
     */
    EXPORT_STEM std::size_t columns(void) const;

    /**
     * crbegin() overload
     */
    EXPORT_STEM virtual const_reverse_iterator crbegin(void) const override;

    /**
     * crend() overload
     */
    EXPORT_STEM virtual const_reverse_iterator crend(void) const override;

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) override;

    /**
     * Compute the determinant of this matrix
     */
    EXPORT_STEM double determinant(void) const;

    /**
     * Test for empty matrix
     */
    EXPORT_STEM bool empty(void) const;

    /**
     * end() overload
     */
    EXPORT_STEM virtual iterator end(void) override;

    /**
     * Return a matrix containing a specified number of diagonals above, below and including the main diagonal
     * from the current matrix object.
     * @param kAbove the number of diagonals above the main diagonal
     * @param kBelow the number of diagonals below the main diagonal
     */
    EXPORT_STEM Matrix2d getBand(int nAbove = 1,
                                 int nBelow = -1) const;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Retrieve a lower triangular matrix that includes elements below the k-th diagonal with respect to the
     * main diagonal of the matrix
     * @param  nAbove = 0 is the main diagonal
     *                > 0 is above the main diagonal
     *                < 0 is below the main diagonal.
     * @return        upon success, the lower triangular matrix
     */
    EXPORT_STEM Matrix2d getLowerTriangle(int nAbove = 0) const;

    /**
     * Retrieve an upper triangular matrix that includes elements above the k-th diagonal with respect to the
     * main diagonal of the matrix
     * @param  nBelow = 0 is the main diagonal
     *                > 0 is above the main diagonal
     *                < 0 is below the main diagonal.
     * @return        upon success, the upper triangular matrix
     */
    EXPORT_STEM Matrix2d getUpperTriangle(int nBelow = 0) const;

    /**
     * Function to construct an n x n identity matrix
     */
    static EXPORT_STEM Matrix2d identity(std::size_t dimension);

    /**
     * Initialization function
     */
    EXPORT_STEM void initialize(std::size_t rows,
                                std::size_t cols);

    /**
     * Invert the current object's matrix and store in the current object
     */
    EXPORT_STEM void invert(void);

    /**
     * Test for column matrix
     */
    EXPORT_STEM bool isColumnMatrix(void) const;

    /**
     * Test for row matrix
     */
    EXPORT_STEM bool isRowMatrix(void) const;

    /**
     * Test whether or not matrix is square
     */
    EXPORT_STEM bool isSquare(void) const;

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
    EXPORT_STEM void makeBand(int nAbove = 1,
                              int nBelow = -1);

    /**
     * Morph the current object's matrix into a lower triangular matrix that includes elements below the k-th
     * diagonal with respect to the main diagonal of the matrix
     * @param nAbove = 0 is the main diagonal
     *               > 0 is above the main diagonal
     *               < 0 is below the main diagonal
     */
    EXPORT_STEM void makeLowerTriangular(int nAbove = 0);

    /**
     * Morph the current object's matrix into an upper triangular matrix that includes elements above the k-th
     * diagonal with respect to the main diagonal of the matrix
     * @param nBelow = 0 is the main diagonal
     *               > 0 is above the main diagonal
     *               < 0 is below the main diagonal
     */
    EXPORT_STEM void makeUpperTriangular(int nBelow = 0);

    /**
     * Multiply the current object by the rhs argument
     */
    EXPORT_STEM Matrix2d multiply(const Matrix2d &matrix);

    /**
     * Multiply the current object by the rhs argument and store the product in the result argument
     */
    EXPORT_STEM void multiply(const Matrix2d &matrix,
                              Matrix2d &result);

    /**
     * Multiply the lhs argument by the rhs argument and store the product in the result argument
     * @param      lhs                         the left-hand side matrix
     * @param      rhs                         the right-hand side matrix
     * @param[out] result                      if successful, will be populated by the product of the two
     *                                         matrices
     * @param      multiplicationTransposeType enumeration specifying the type of matrix transposition applied
     *                                         in the product, if any
     */
    static EXPORT_STEM void multiply(const Matrix2d &lhs,
                                     const Matrix2d &rhs,
                                     Matrix2d &result,
                                     MultiplicationTransposeType multiplicationTransposeType = None);

    /**
     * Negate the matrix
     */
    EXPORT_STEM Matrix2d &negate(void);

    /**
     * Calculate the outer product of two vectors and store the result in the input matrix
     */
    static EXPORT_STEM void outerProduct(const std::vector<double> &a,
                                         const std::vector<double> &b,
                                         Matrix2d &result);

    /**
     * Permute columns of the matrix
     * @param permutation a permutation vector
     */
    EXPORT_STEM void permuteColumns(const std::vector<std::size_t> &permutation);

    /**
     * Permute columns of the matrix
     * @param      permutation a permutation vector
     * @param[in]  work        a workspace vector
     * @param[out] work        has been destroyed
     */
    EXPORT_STEM void permuteColumns(const std::vector<std::size_t> &permutation,
                                    std::vector<std::size_t> &work);

    /**
     * Permute rows of the matrix
     * @param permutation a permutation vector
     */
    EXPORT_STEM void permuteRows(const std::vector<std::size_t> &permutation);

    /**
     * Permute rows of the matrix
     * @param      permutation a permutation vector
     * @param[in]  work        a workspace vector
     * @param[out] work        has been destroyed
     */
    EXPORT_STEM void permuteRows(const std::vector<std::size_t> &permutation,
                                 std::vector<std::size_t> &work);

    /**
     * Post-multiply the current object by the transpose of the rhs argument
     */
    EXPORT_STEM Matrix2d postMultiplyTranspose(const Matrix2d &matrix);

    /**
     * Post-multiply the current object by the transpose of the rhs argument and store the product in the
     * result argument
     */
    EXPORT_STEM void postMultiplyTranspose(const Matrix2d &matrix,
                                           Matrix2d &result);

    /**
     * Post-multiply the lhs argument by the transpose of the rhs argument and store the product in the result
     * argument
     */
    friend EXPORT_STEM void postMultiplyTranspose(const Matrix2d &lhs,
                                                  const Matrix2d &rhs,
                                                  Matrix2d &result);

    /**
     * Pre-multiply the current object by the transpose of the lhs argument
     */
    EXPORT_STEM Matrix2d preMultiplyTranspose(const Matrix2d &lhs);

    /**
     * Pre-multiply the current object by the transpose of the lhs argument and store the product in the result
     * argument
     */
    EXPORT_STEM void preMultiplyTranspose(const Matrix2d &lhs,
                                          Matrix2d &result);

    /**
     * Pre-multiply the rhs argument by the transpose of the lhs argument and store the product in the result
     * argument
     */
    friend EXPORT_STEM void preMultiplyTranspose(const Matrix2d &lhs,
                                                 const Matrix2d &rhs,
                                                 Matrix2d &result);

    /**
     * rbegin() overload
     */
    EXPORT_STEM virtual reverse_iterator rbegin(void) override;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * rend() overload
     */
    EXPORT_STEM virtual reverse_iterator rend(void) override;

    /**
     * Resize the current matrix object
     * @param rows          the number of rows in the matrix
     * @param cols          the number of columns in the matrix
     * @param bPreserveData flag indicating whether or not to preserve the data in the matrix (optional,
     *                      defaults to true)
     */
    EXPORT_STEM void resize(std::size_t rows,
                            std::size_t cols,
                            bool bPreserveData = true);

    /**
     * Get the i-th row
     */
    EXPORT_STEM Matrix2d row(std::size_t i) const;

    /**
     * Get the i-th row
     * @param i         the index of the row to retrieve
     * @param rowMatrix upon success, will contain the result
     */
    EXPORT_STEM void row(std::size_t j,
                         Matrix2d &rowMatrix) const;

    /**
     * Get the i-th row
     * @param i      the index of the row to retrieve
     * @param vector upon success, will contain the result
     */
    EXPORT_STEM void row(std::size_t j,
                         std::vector<double> &vector) const;

    /**
     * Get the number of rows in the matrix
     */
    EXPORT_STEM std::size_t rows(void) const;

    /**
     * Scale the matrix and store in the current object
     */
    EXPORT_STEM Matrix2d &scale(double value);

    /**
     * Function to serialize this object's data
     */
    EXPORT_STEM virtual std::ostream &serialize(std::ostream &stream) const override;

    /**
     * Assign elements along the main diagonal (i == j)
     * @param diagonal the diagonal elements
     */
    EXPORT_STEM void setDiagonal(const std::vector<double> &diagonal);

    /**
     * Assign elements along the main diagonal (i == j)
     * @param diagonal the desired diagonal element assignment
     */
    EXPORT_STEM void setDiagonal(double diagonal);

    /**
     * Assign elements along the main diagonal (i == j)
     * @param diagonal    the desired diagonal element assignment
     * @param offDiagonal the desired off-diagonal element assignment
     */
    EXPORT_STEM void setDiagonal(double diagonal,
                                 double offDiagonal);

    /**
     * Return number of elements in matrix
     */
    EXPORT_STEM size_t size(void) const;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(Matrix2d &matrix) override final;

    /**
     * Compute the transpose of this matrix and store in the current object
     */
    EXPORT_STEM Matrix2d &transpose(void);
#ifdef RAPID_XML
    /**
     * Function to write data to XML
     */
    EXPORT_STEM virtual bool writeToXML(rapidxml::xml_node<char> *pNode) override;
#endif
private:

    /**
     * number of columns in the matrix
     */
    std::size_t m_columns;

    /**
     * pointer to a temporary workspace matrix
     */
    mutable Matrix2d *m_pTempMatrix;

    /**
     * number of rows in the matrix
     */
    std::size_t m_rows;

    /**
     * data vector
     */
    std::vector<double> m_vector;
};

}

}

}

#endif
