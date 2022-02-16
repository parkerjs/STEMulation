#ifndef GENERIC_MATRIX_2D_H
#define GENERIC_MATRIX_2D_H

#include "general_matrix.h"
#include "inherited_iterator.h"
#include "iterable.h"
#include "matrix_dimension_type.h"
#include "permutator.h"
#include "reference_matrix_2d.h"
#include "reflective.h"
#include "reverse_iterable.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include "tokenizer.h"
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <vector>

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * Two-dimensional generic data type matrix class
 */
template<typename T>
class Matrix<2, T, GeneralMatrix>
: public GeneralMatrix,
  public attributes::abstract::Iterable<iterators::Iterator,
                                        std::vector<typename std::conditional<std::is_reference<T>::value,
                                        functional::VariableWrapper<T &>, T>::type>,
                                        iterators::inherited_iterator_tag>,
  virtual private attributes::abstract::Reflective,
  public attributes::abstract::ReverseIterable<iterators::Iterator,
                                               std::vector<typename std::conditional<std::is_reference<T>::value,
                                               functional::VariableWrapper<T &>, T>::type>,
                                               iterators::inherited_iterator_tag>,
  public attributes::concrete::StaticMutexMappable<Matrix<2, T>, int, std::mutex *>,
  public attributes::concrete::StaticSynchronizable<Matrix<2, T>>,
  public attributes::concrete::Streamable<Matrix<2, T>>
{
public:

    /**
     * Typedef declarations
     */
    typedef T data_type;
    typedef typename std::decay<T>::type decay_type;
    typedef GeneralMatrix matrix_type;
    typedef typename std::conditional<std::is_reference<T>::value,
                     functional::VariableWrapper<decay_type &>, decay_type>::type vector_type;
    typedef typename attributes::abstract::Iterable<iterators::Iterator, std::vector<vector_type>,
                     iterators::inherited_iterator_tag>::const_iterator const_iterator;
    typedef typename attributes::abstract::ReverseIterable<iterators::Iterator, std::vector<vector_type>,
                     iterators::inherited_iterator_tag>::const_reverse_iterator const_reverse_iterator;
    typedef typename attributes::abstract::Iterable<iterators::Iterator, std::vector<vector_type>,
                     iterators::inherited_iterator_tag>::iterator iterator;
    typedef typename attributes::abstract::ReverseIterable<iterators::Iterator, std::vector<vector_type>,
                     iterators::inherited_iterator_tag>::reverse_iterator reverse_iterator;

    /**
     * Constructor
     */
    Matrix(void)
    : m_columns(0),
      m_pTempMatrix(nullptr),
      m_rows(0)
    {

    }

    /**
     * Constructor
     * @param rows    the size along the first dimension
     * @param columns the size along the second dimension
     */
    Matrix(std::size_t rows, std::size_t columns)
    : m_pTempMatrix(nullptr)
    {
        initialize(rows, columns);
    }

    /**
     * Constructor
     * @param data a raw array used to initialize the object as an M x 1 column matrix
     */
    template<std::size_t M>
    Matrix(const typename std::remove_reference<decay_type>::type (&data)[M])
    : m_pTempMatrix(nullptr)
    {
        initialize(M, 1);

        std::copy(data, data + M, begin());
    }

    /**
     * Constructor
     * @param vector used to initialize the object as a column matrix
     */
    template<typename Vector,
             typename std::enable_if<std::is_same<typename std::decay<Vector>::type,
                      std::vector<decay_type>>::value, int>::type = 0>
    Matrix(Vector &&vector)
    : m_pTempMatrix(nullptr)
    {
        initialize(vector.size(), 1);

        std::copy(vector.cbegin(), vector.cend(), begin());
    }

    /**
     * Constructor
     * @param vector a two-dimensional vector used to initialize the object; if the rows have unequal length,
     *               the row with the fewest elements will dictate the number of columns in the resultant
     *               matrix
     */
    template<typename Vector, typename std::enable_if<std::is_same<
             typename std::decay<Vector>::type, std::vector<std::vector<decay_type>>>::value, int>::type = 0>
    Matrix(Vector &&vector)
    : m_pTempMatrix(nullptr)
    {
        if (!vector.empty())
        {
            std::size_t columns = 0;
            auto &&itVector = vector.cbegin();
            for (columns = itVector->size(); itVector != vector.cend(); ++itVector)
                columns = std::min(columns, itVector->size());

            initialize(vector.size(), columns);

            for (std::size_t i = 0, offset = 0; i < m_rows; ++i)
            {
                for (std::size_t j = 0; j < m_columns; ++j)
                    m_vector[offset + j] = vector[i][j];

                offset += columns;
            }
        }
    }

    /**
     * Constructor
     * @param data an stl-like container of elements that will be used to populate the constructed matrix
     */
    template<typename Data>
    Matrix(Data &&data, std::size_t rows, std::size_t columns)
    : m_pTempMatrix(nullptr)
    {
        initialize(rows, columns);

        std::copy(std::forward<Data>(data).cbegin(),
                  std::forward<Data>(data).cbegin() + size(), begin());
    }

    /**
     * Constructor
     * @param data a two-dimensional array used to initialize the object
     */
    template<std::size_t Rows, std::size_t Columns>
    Matrix(const typename std::remove_reference<decay_type>::type (&data)[Rows][Columns])
    : m_pTempMatrix(nullptr)
    {
        initialize(Rows, Columns);

        for (std::size_t i = 0, offset = 0; i < m_rows; ++i)
        {
            for (std::size_t j = 0; j < m_columns; ++j)
                m_vector[offset + j] = data[i][j];

            offset += m_columns;
        }
    }

    /**
     * Constructor
     * @param data       an stl-like container of elements that will be used to populate the constructed matrix
     * @param dimensions a 2-element vector of row and column dimension sizes
     */
    template<typename Data>
    Matrix(Data &&data, const std::vector<std::size_t> &dimensions)
    : m_pTempMatrix(nullptr)
    {
        initialize(dimensions[0], dimensions[1]);

        std::copy(std::forward<Data>(data).cbegin(),
                  std::forward<Data>(data).cbegin() + size(), begin());
    }

    /**
     * Constructor
     * @param value used to initialize the object as a 1 x 1 matrix
     */
    template<typename Value, typename std::enable_if<std::is_same<Value, decay_type>::value, int>::type = 0>
    Matrix(Value &&value)
    : m_pTempMatrix(nullptr)
    {
        initialize(1, 1);

        m_vector[0] = std::forward<Value>(value);
    }

    /**
     * Copy constructor
     */
    Matrix(const Matrix<2, T, GeneralMatrix> &matrix)
    : m_pTempMatrix(nullptr)
    {
        operator = (matrix);
    }

    /**
     * Copy constructor
     */
    template<typename U, typename V, typename std::enable_if<!std::is_same<U, T>::value, int>::type = 0>
    Matrix(const Matrix<2, U, V> &matrix)
    : m_pTempMatrix(nullptr)
    {
        operator = (matrix);
    }

    /**
     * Move constructor
     */
    Matrix(Matrix<2, T, GeneralMatrix> &&matrix)
    : m_pTempMatrix(nullptr)
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
    auto &operator = (const Matrix<2, T, GeneralMatrix> &matrix)
    {
        return copy(matrix);
    }

    /**
     * Copy assignment operator
     */
    template<typename U, typename V, typename std::enable_if<!std::is_same<U, T>::value, int>::type = 0>
    auto &operator = (const Matrix<2, U, V> &matrix)
    {
        return copy(matrix);
    }

    /**
     * Move assignment operator
     */
    auto &operator = (Matrix<2, T, GeneralMatrix> &&matrix)
    {
        if (dynamic_cast<const Matrix<2, T> *>(&matrix) != this)
        {
            m_columns = std::move(matrix.m_columns);
            m_pTempMatrix = std::move(matrix.m_pTempMatrix);
            m_rows = std::move(matrix.m_rows);
            m_vector = std::move(matrix.m_vector);
        }

        return *this;
    }

    /**
     * Assignment operator
     * @param value used to initialize all elements within the matrix
     */
    template<typename U, typename std::enable_if<std::is_same<U, decay_type>::value, int>::type = 0>
    auto &operator = (U &&value)
    {
        std::fill_n(begin(), size(), value);

        return *this;
    }

    /**
     * Assignment operator
     * @param vector used to initialize this object's vector of elements
     */
    template<typename Vector>
    typename std::enable_if<std::is_same<typename std::decay<Vector>::type,
                            std::vector<decay_type>>::value, Matrix<2, decay_type> &>::type
    operator = (Vector &&vector)
    {
        if (!vector.empty())
        {
            if (m_vector.empty() || size() == vector.size())
            {
                if (m_vector.empty())
                    initialize(vector.size(), 1);

                std::copy(vector.cbegin(), vector.cend(), begin());
            }
            else
            {
                attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
                std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                          << "Source and destination vectors are not the same length."
                          << std::endl << std::endl;
                attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
            }
        }
        else
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Input vector is empty." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }

        return *this;
    }

    /**
     * Return the first element of the matrix
     */
    template<typename U, typename std::enable_if<std::is_same<U, decay_type>::value, int>::type = 0>
    inline operator U (void) const &
    {
        return m_vector[0];
    }

    /**
     * Return a reference to the first element of the matrix
     */
    template<typename U, typename std::enable_if<std::is_same<U, decay_type>::value, int>::type = 0>
    inline operator U & (void) &
    {
        return m_vector[0];
    }

    /**
     * Return this object's vector of elements
     */
    template<typename U>
    inline operator std::vector<U> (void) const &
    {
        return std::vector<U>(cbegin(), cend());
    }

    /**
     * Return this object's vector of elements
     */
    template<typename U = T, typename std::enable_if<!std::is_reference<U>::value, int>::type = 0>
    inline operator std::vector<decay_type> && (void) &&
    {
        return std::move(m_vector);
    }

    /**
     * Return a reference to this object's vector of elements
     */
    template<typename U = T, typename std::enable_if<!std::is_reference<U>::value, int>::type = 0>
    inline operator std::vector<decay_type> & (void) &
    {
        return m_vector;
    }

    /**
     * Conversion operator
     */
    inline operator Matrix<2, T> & (void)
    {
        return static_cast<Matrix<2, T> &>(*this);
    }

    /**
     * Return this matrix object as a two-dimensional std::vector
     */
    operator std::vector<std::vector<decay_type>> (void) const
    {
        std::vector<std::vector<decay_type>> vector(m_rows, std::vector<decay_type>(m_columns));
        for (std::size_t i = 0, offset = 0; i < m_rows; ++i)
        {
            for (std::size_t j = 0; j < m_columns; ++j)
                vector[i][j] = m_vector[offset + j];

            offset += m_columns;
        }

        return vector;
    }

    /**
     * Subscript operator
     * @param index the linear index of the element to be addressed
     */
    inline decay_type &operator [] (std::size_t index)
    {
        return m_vector[index];
    }

    /**
     * Subscript operator
     * @param index the linear index of the element to be addressed
     */
    inline decay_type operator [] (std::size_t index) const
    {
        return m_vector[index];
    }

    /**
     * Function call operator
     * @param  rowIndices    a vector of row indices to be addressed
     * @param  columnIndices a vector of column indices to be addressed
     * @return               a matrix composed of references to elements addressed by the row and column index
     *                       vectors
     */
    inline Matrix<2, decay_type &> operator () (const std::vector<std::size_t> &rowIndices,
                                                const std::vector<std::size_t> &columnIndices)
    {
        return Matrix<2, decay_type &>(*this, rowIndices, columnIndices);
    }

    /**
     * Function call operator
     * @param i the row index
     * @param j the column index
     */
    inline decay_type &operator () (std::size_t i, std::size_t j = 0)
    {
        return m_vector[i * m_columns + j];
    }

    /**
     * Function call operator
     * @param i the row index
     * @param j the column index
     */
    inline decay_type operator () (std::size_t i, std::size_t j = 0) const
    {
        return m_vector[i * m_columns + j];
    }

    /**
     * Equality operator
     */
    inline bool operator == (const Matrix<2, decay_type> &matrix) const
    {
        return m_rows == matrix.m_rows && m_columns == matrix.m_columns && m_vector == matrix.m_vector;
    }

    /**
     * Inequality operator
     */
    inline bool operator != (const Matrix<2, decay_type> &matrix) const
    {
        return !operator == (matrix);
    }

    /**
     * Augment current matrix object with another matrix and assign to current object
     */
    Matrix<2, decay_type> &augment(const decay_type &value)
    {
        if (m_rows <= 1 || m_columns <= 1)
        {
            if (m_rows == 1 || m_vector.empty())
                ++m_columns;
            else
                ++m_rows;

            m_vector.insert(end(), value);
        }

        return *this;
    }

    /**
     * Row or column matrix augmentation. The function augments the input value according to the following:
     * 1) 1 x 1 matrix becomes a 1 x 2
     * 2) 1 x n matrix becomes a 1 x (n + 1)
     * 3) m x 1 matrix becomes an (m + 1) x 1
     */
    Matrix<2, decay_type> augment(const decay_type &value) const
    {
        Matrix<2, decay_type> result;
        if (m_rows <= 1 || m_columns <= 1)
        {
            std::size_t rows = 0, columns = 0;
            if (m_rows == 1 || m_vector.empty())
            {
                rows = 1;
                columns = m_columns + 1;
            }
            else if (m_columns == 1)
            {
                rows = m_rows + 1;
                columns = 1;
            }

            result.m_rows = rows;
            result.m_columns = columns;
            result.m_vector.insert(result.begin(), cbegin(), cend());
            result.m_vector.insert(result.end(), value);
        }

        return result;
    }

    /**
     * Augment the current matrix object with another matrix and assign to the current object
     */
    Matrix<2, decay_type> &augment(const Matrix<2, decay_type> &matrix)
    {
        if (m_vector.empty())
        {
            copy(matrix);
        }
        else if (m_rows == matrix.m_rows)
        {
            auto offset = m_columns;
            for (auto &&itMatrix = matrix.cbegin(); itMatrix != matrix.cend(); offset += m_columns)
            {
                m_vector.insert(begin() + offset, itMatrix, itMatrix + matrix.m_columns);
                offset += matrix.m_columns;
                itMatrix += matrix.m_columns;
            }

            m_columns += matrix.m_columns;
        }

        return *this;
    }

    /**
     * Matrix augmentation
     */
    Matrix<2, decay_type> augment(const Matrix<2, decay_type> &matrix) const
    {
        Matrix<2, decay_type> result;
        if (m_vector.empty())
        {
            result.copy(matrix);
        }
        else if (m_rows == matrix.m_rows)
        {
            result.initialize(m_rows, m_columns + matrix.m_columns);

            auto &&itMatrix = matrix.cbegin();
            auto &&itResultMatrix = result.begin();
            auto &&itThisMatrix = cbegin();
            while (itResultMatrix != result.end())
            {
                std::copy(itThisMatrix, itThisMatrix + m_columns, itResultMatrix);
                std::copy(itMatrix, itMatrix + matrix.m_columns, itResultMatrix + m_columns);

                itMatrix += matrix.m_columns;
                itResultMatrix += result.m_columns;
                itThisMatrix += m_columns;
            }
        }

        return result;
    }

    /**
     * begin() overload
     */
    inline virtual iterator begin(void) override
    {
        return m_vector.begin();
    }

    /**
     * cbegin() overload
     */
    inline virtual const_iterator cbegin(void) const override
    {
        return m_vector.cbegin();
    }

    /**
     * cend() overload
     */
    inline virtual const_iterator cend(void) const override
    {
        return m_vector.cend();
    }

    /**
     * Clear this object
     */
    inline virtual void clear(void) final
    {
        m_columns = 0;
        m_rows = 0;
        m_vector.clear();
    }

    /**
     * Get the j-th column
     */
    inline Matrix<2, decay_type &> column(std::size_t j)
    {
        std::vector<std::size_t> rows(m_rows);
        std::iota(rows.begin(), rows.end(), 0);

        return operator () (rows, { j });
    }

    /**
     * Get the jth column and store it in the input column vector
     */
    bool column(std::size_t j, std::vector<decay_type> &column) const
    {
        bool bSuccess = (column.size() == m_rows && j < m_columns);
        if (bSuccess)
        {
            auto size = this->size();
            for (std::size_t i = 0, offset = j; offset < size; ++i, offset += m_columns)
                column[i] = m_vector[offset];
        }

        return bSuccess;
    }

    /**
     * Get the number of columns in the matrix
     */
    inline virtual std::size_t columns(void) const final
    {
        return m_columns;
    }

protected:

    /**
     * Copy function
     */
    template<typename U, typename V>
    auto &copy(const Matrix<2, U, V> &matrix)
    {
        if (dynamic_cast<const Matrix<2, T> *>(&matrix) != this)
        {
            initialize(matrix.rows(), matrix.columns());

//          m_pTempMatrix = matrix.m_pTempMatrix; // can't copy std::unique_ptr

            // intentionally not using std::copy here...
            auto size = m_vector.size();
            for (std::size_t i = 0; i < size; ++i)
                m_vector[i] = matrix[i];
        }

        return *this;
    }

public:

    /**
     * crbegin() overload
     */
    inline virtual const_reverse_iterator crbegin(void) const override
    {
        return m_vector.crbegin();
    }

    /**
     * Construct a diagonal matrix
     * @param vector a collection of elements that will populate the diagonal
     */
    static Matrix<2, decay_type> createDiagonalMatrix(const std::vector<decay_type> &vector)
    {
        Matrix<2, decay_type> result(vector.size(), vector.size());
        for (std::size_t i = 0, offset = 0; i < result.m_columns; ++i)
        {
            result.m_vector[offset] = vector[i];
            offset += result.m_columns + 1;
        }

        return result;
    }

    /**
     * crend() overload
     */
    inline virtual const_reverse_iterator crend(void) const override
    {
        return m_vector.crend();
    }

    /**
     * Return the dimensionality of this matrix object
     */
    inline virtual std::size_t dimension(void) const final
    {
        return 2;
    }

    /**
     * Return a vector containing this matrix object's dimensions
     */
    inline virtual std::vector<std::size_t> dimensions(void) const final
    {
        return { m_rows, m_columns };
    }

    /**
     * Test for empty matrix object
     */
    inline virtual bool empty(void) const final
    {
        return m_vector.empty();
    }

    /**
     * end() overload
     */
    inline virtual iterator end(void) override
    {
        return m_vector.end();
    }

    /**
     * Read data from the input stream into a matrix. If the input matrix is non-empty, the istream operator
     * will attempt to read in data from the current position in the stream, where the number of rows and
     * columns are defined by the dimensions of the input matrix. If the number of rows or columns of data at
     * the current position in the stream is less than the dimensions specified by the input matrix, a format
     * error message will be issued. Blank lines are skipped for non-empty matrix input. For empty matrices,
     * the istream operator will attempt to read multiple rows of data having the same number of numeric values
     * as the first row. Reading stops when a blank line is encountered. The input is tokenized by spaces,
     * tabs, and commas. The function will attempt to convert the tokens into numerical values, and tokens that
     * cannot be converted will be discarded. Hence, the input stream can contain strings of non-numeric data
     * interspersed with numeric data, so long as each line contains the same number of tokens that can be
     * successfully converted to numeric data. The istream operator works differently on data streams than the
     * load() command in that the load() command scans the entire stream, while the istream operator only scans
     * up to the point in the stream where conditions for extraction are no longer met
     */
    virtual std::istream &extract(std::istream &stream) override
    {
        std::size_t j = 0, rows = 0, columns = 0;
        char endLineDelimiters[] = { '\r', '\n', ';' }; // end of line delimiters
        utilities::Tokenizer tokenizer(" ", "\t", ","); // default delimiter is newline character
        std::streampos streamPosition = 0;
        try
        {
            auto &&is_space = [] (auto &&ch) { return !isspace(ch); };
            std::string string;
            while (stream.good())
            {
                // determine which end of line delimiter occurs first
                streamPosition = stream.tellg();
                std::size_t k = 0, l = std::string::npos;
                for (std::size_t i = 0; i < sizeof(endLineDelimiters); ++i)
                {
                    std::getline(stream, string, endLineDelimiters[i]);
                    if (string.size() < l)
                    {
                        k = i;
                        l = string.size();
                    }

                    stream.clear();
                    stream.seekg(streamPosition);
                }

                std::getline(stream, string, endLineDelimiters[k]);
                if (string.size() == 0)
                    continue;

                if (j == 0 && std::find_if(string.cbegin(), string.cend(), is_space) == string.cend())
                    continue; // blank line detected

                auto &&vector = tokenizer.parse<decay_type>(string);
                if (m_columns > 0) // a non-empty matrix was passed in
                {
                    if (rows >= m_rows)
                        throw 0;
                    else if (m_columns > vector.size())
                        throw -1;
                    else if (vector.empty())
                        continue;
                    else if (columns == 0)
                        columns = m_columns;

                    std::copy(vector.begin(), std::next(vector.begin(), m_columns), std::next(begin(), j));
                }
                else // an empty matrix was passed in
                {
                    if ((columns > 0 && vector.size() != columns) || vector.empty())
                        throw 0;
                    else if (columns == 0)
                        columns = vector.size();

                    m_vector.insert(std::next(begin(), j), vector.begin(), vector.end());
                }

                ++rows;
                j += columns;
            }
        }
        catch (int error)
        {
            if (error < 0)
            {
                attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
                std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                          << "Read error at stream position " << stream.tellg() << " of input file."
                          << std::endl << std::endl;
                attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
                stream.setstate(std::ios::failbit);
            }

            stream.seekg(streamPosition);
        }

        m_rows = (m_rows == 0) ? rows : m_rows;
        m_columns = (m_columns == 0) ? columns : m_columns;

        return stream;
    }

    /**
     * Return a matrix containing a specified number of diagonals above, below and including the main diagonal
     * from the current matrix object
     * @param above the number of diagonals above the main diagonal
     * @param below the number of diagonals below the main diagonal
     */
    inline Matrix<2, decay_type> getBand(int above = 1, int below = -1) const
    {
        Matrix<2, decay_type> matrix(*this);
        matrix.removeLowerAndUpperTriangles(above, below);

        return matrix;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return std::string("Matrix<2, ") + typeid(T).name() + ">";
    }

    /**
     * Return elements along the main diagonal (i == j)
     * @param[in]  diagonal storage to hold the diagonal elements
     * @param[out] diagonal the diagonal elements
     */
    void getDiagonal(std::vector<decay_type> &diagonal) const
    {
        std::size_t i = 0, offset = 0;
        while (i < m_columns && i < diagonal.size())
        {
            diagonal[i] = m_vector[offset];
            offset += m_columns + 1;
            ++i;
        }
    }

    /**
     * Retrieve a lower triangular matrix that includes elements below the kth diagonal with respect to the
     * main diagonal of the matrix
     * @param above = 0 is the main diagonal
     *              > 0 is above the main diagonal
     *              < 0 is below the main diagonal.
     */
    Matrix<2, decay_type> getLowerTriangle(int above = 0) const
    {
        auto below = 1 - int(m_rows);
        if (above < below)
            below = above;

        return getBand(above, below);
    }

    /**
     * Get the transpose matrix
     */
    Matrix<2, decay_type> getTranspose(void) const
    {
        Matrix<2, decay_type> matrix(*this);
        matrix.transpose();

        return matrix;
    }

    /**
     * Retrieve elements along the diagonal and off-diagonals of a matrix
     * @param[in]  upper    storage to hold n - 1 super-diagonal elements
     * @param[in]  diagonal storage to hold n diagonal elements
     * @param[in]  lower    storage to hold n - 1 sub-diagonal elements
     * @param[out] upper    the super-diagonal elements
     * @param[out] diagonal the diagonal elements
     * @param[out] lower    the sub-diagonal elements
     */
    void getTridiagonal(std::vector<decay_type> &upper, std::vector<decay_type> &diagonal,
                        std::vector<decay_type> &lower) const
    {
        for (std::size_t i = 0, offset = 0; i < m_rows; ++i)
        {
            if (i > 0 && i <= lower.size() && offset <= size())
                lower[i - 1] = m_vector[offset - 1]; // sub-diagonal element

            if (i < diagonal.size())
                diagonal[i] = m_vector[offset]; // diagonal element

            if (i < upper.size() && offset + 1 < size())
                upper[i] = m_vector[offset + 1]; // super-diagonal element

            offset += m_columns + 1;
        }
    }

    /**
     * Retrieve an upper triangular matrix that includes elements above the kth diagonal with respect to the
     * main diagonal of the matrix
     * @param below = 0 is the main diagonal
     *              > 0 is above the main diagonal
     *              < 0 is below the main diagonal
     */
    Matrix<2, decay_type> getUpperTriangle(int below = 0) const
    {
        auto above = int(m_columns) - 1;
        if (above < below)
            above = below;

        return getBand(above, below);
    }

    /**
     * Initialization function
     */
    virtual void initialize(std::size_t rows, std::size_t columns) final
    {
        m_columns = columns;
        m_rows = rows;

        auto size = m_rows * m_columns;
        if (this->size() != size)
            m_vector.resize(size);
    }

    /**
     * Insert column before the jth column
     */
    void insertColumn(std::size_t j, const std::vector<decay_type> &vector)
    {
        if (vector.size() != m_rows || j > m_columns)
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Column could not be inserted." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }
        else
        {
            for (std::size_t i = 0; i < m_rows; ++i)
            {
                m_vector.insert(begin() + j, vector[i]);
                j += m_columns + i;
            }

            ++m_columns;
        }
    }

    /**
     * Insert row before the ith row
     */
    void insertRow(std::size_t i, const std::vector<decay_type> &vector)
    {
        if (vector.size() != m_columns || i > m_rows)
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Row could not be inserted." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }
        else
        {
            m_vector.insert(begin() + i * m_columns, vector.cbegin(), vector.cend());
            ++m_rows;
        }
    }

    /**
     * Test for column matrix
     */
    inline virtual bool isColumnMatrix(void) const final
    {
        return m_columns == 1;
    }

    /**
     * Test for row matrix
     */
    inline virtual bool isRowMatrix(void) const final
    {
        return m_rows == 1;
    }

    /**
     * Test whether or not matrix is square
     */
    inline virtual bool isSquare(void) const final
    {
        return (m_rows == m_columns);
    }

    /**
     * Load tabular data from a file into a matrix. The input is tokenized by spaces, tabs, and commas. The
     * function will attempt to convert the tokens into numerical values; tokens that cannot be converted will
     * be discarded. Hence, the input stream can contain strings of non-numeric data interspersed with numeric
     * data, so long as each line contains the same number of tokens that can be successfully converted to
     * numeric data.
     * @param filename the name of the input file from which the data will be loaded
     */
    inline static Matrix<2, decay_type> load(const std::string &filename)
    {
        std::ifstream stream(filename.c_str(), std::ios_base::in | std::ios_base::binary);
        if (!stream)
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Error from " << getQualifiedMethodName(__func__, "Matrix<2, T, GenericMatrix>")
                      << ": " << "File does not exist or could not be opened." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }

        return load(stream, [] (auto &string) { return true; });
    }

    /**
     * Load tabular data from a file into a matrix. The input is tokenized by spaces, tabs, and commas. The
     * function will attempt to convert the tokens into numerical values; tokens that cannot be converted will
     * be discarded. Hence, the input stream can contain strings of non-numeric data interspersed with numeric
     * data, so long as each line contains the same number of tokens that can be successfully converted to
     * numeric data.
     * @param filename the name of the input file from which the data will be loaded
     * @param function a binary function object which is used to test and/or manipulate each line of input from
     *                 the input stream prior to tokenization. The function should return true if the line of
     *                 input should be tokenized and processed by the load() function, or false if it should be
     *                 ignored/skipped.
     */
    template<typename Function>
    inline static Matrix<2, decay_type> load(const std::string &filename, Function &&function)
    {
        std::ifstream stream(filename.c_str(), std::ios_base::in | std::ios_base::binary);
        if (!stream)
        {
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
            std::cout << "Error from " << getQualifiedMethodName(__func__) << ": "
                      << "File does not exist or could not be opened." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
        }

        return load(stream, std::forward<Function>(function));
    }

    /**
     * Load tabular data from an input stream into a matrix. The input is tokenized by spaces, tabs, and
     * commas. The function will attempt to convert the tokens into numerical values; tokens that cannot be
     * converted will be discarded. Hence, the input stream can contain strings of non-numeric data
     * interspersed with numeric data, so long as each line contains the same number of tokens that can be
     * successfully converted to numeric data.
     * @param filename the name of the input file from which the data will be loaded
     */
    template<typename Function>
    static Matrix<2, decay_type> load(std::istream &stream)
    {
        return load(stream, [] (auto &string) { return true; });
    }

    /**
     * Load tabular data from an input stream into a matrix. The input is tokenized by spaces, tabs, and
     * commas. The function will attempt to convert the tokens into numerical values; tokens that cannot be
     * converted will be discarded. Hence, the input stream can contain strings of non-numeric data
     * interspersed with numeric data, so long as each line contains the same number of tokens that can be
     * successfully converted to numeric data.
     * @param filename the name of the input file from which the data will be loaded
     * @param function a binary function object which is used to test and/or manipulate each line of input from
     *                 the input stream prior to tokenization. The function should return true if the line of
     *                 input should be tokenized and processed by the load() function, or false if it should be
     *                 ignored/skipped.
     */
    template<typename Function>
    static Matrix<2, decay_type> load(std::istream &stream, Function &&function)
    {
        char endLineDelimiters[] = { '\r', '\n' }; // end of line delimiters
        Matrix<2, decay_type> result;
        utilities::Tokenizer tokenizer(" ", "\t", ","); // default delimiter is newline character
        auto &&is_space = [] (auto &&ch) { return !isspace(ch); };
        std::string string;
        while (stream.good())
        {
            // determine which end of line delimiter occurs first
            auto &&streamPosition = stream.tellg();
            std::size_t j = std::string::npos, k = 0;
            for (std::size_t i = 0; i < sizeof(endLineDelimiters); ++i)
            {
                std::getline(stream, string, endLineDelimiters[i]);
                if (string.size() < j)
                {
                    j = string.size();
                    k = i;
                }

                stream.clear();
                stream.seekg(streamPosition);
            }

            std::getline(stream, string, endLineDelimiters[k]);
            if (string.size() == 0)
                continue;

            if (!function(string))
                continue;
            else if (std::find_if(string.cbegin(), string.cend(), is_space) == string.cend())
                continue; // blank line detected

            auto &&vector = tokenizer.parse<decay_type>(string);
            if (result.m_columns > 0 && vector.size() != result.m_columns)
            {
                attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
                std::cout << "Warning from " << getQualifiedMethodName(__func__, "Matrix<2, T, GeneralMatrix")
                          << ": Read error at stream position " << stream.tellg() << " of input file."
                          << std::endl << std::endl;
                attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);
                stream.setstate(std::ios::failbit);

                continue;
            }

            result.m_vector.insert(result.end(), vector.cbegin(), vector.cend());
            result.m_columns = (result.m_columns == 0) ? vector.size() : result.m_columns;
            ++result.m_rows;
        }

        return result;
    }

    /**
     * Map the elements along the specified matrix dimension(s) to (a) single value(s) via function object
     * @param dimension the matrix dimensions along which the function object will be applied
     * @param function  a binary function object which accepts a vector of indices along a dimension as its
     *                  first argument and the index of the associated dimension as its second argument
     */
    template<typename Function>
    Matrix<2, decay_type> map(const MatrixDimensionType &dimension, Function &&function) const
    {
        std::size_t dimensions[] = { m_rows, m_columns }, offset[] = { m_rows, 1 };
        std::vector<decay_type> u(size()), vector, w;
        std::vector<std::size_t> subscript(1, 1);
        std::copy(cbegin(), cend(), u.begin());

        if (dimension == MatrixDimensionType::Enum::Columns)
            subscript[0] = 0;
        else if (dimension == MatrixDimensionType::Enum::RowsAndColumns)
            subscript.resize(2, 0);

        for (std::size_t i = 0; i < subscript.size(); ++i)
        {
            auto h = subscript[i];
            auto k = dimensions[h];
            dimensions[h] = 1; // make this dimension a singleton
            vector.resize(k);
            w.resize(dimensions[0] * dimensions[1]);
            for (std::size_t j = 0; j < u.size(); ++j)
            {
                auto g = offset[h];
                vector[j % k] = u[w.size() * (j % g) + j / g];

                if ((j + 1) % k == 0)
                    w[j / k] = std::forward<Function>(function)(vector, h);
            }

            u = std::move(w);
        }

        return Matrix<2, decay_type>(std::move(u), dimensions[0], dimensions[1]);
    }

    /**
     * Permute columns of the matrix
     * @param permutation the permutation vector
     */
    inline virtual void permuteColumns(const std::vector<std::size_t> &permutation) final
    {
        std::vector<std::size_t> relativePermutation(m_columns);
        permuteColumns(permutation, relativePermutation);
    }

    /**
     * Permute columns of the matrix
     * @param      permutation         the permutation vector
     * @param[in]  relativePermutation storage to hold relative permutation vector (will be resized if needed)
     * @param[out] relativePermutation the relative permutation vector
     */
    virtual void permuteColumns(const std::vector<std::size_t> &permutation,
                                std::vector<std::size_t> &relativePermutation) final
    {
        auto n = std::min(m_columns, permutation.size());
        for (std::size_t j = 0; j < n; ++j)
        {
            if (permutation[j] >= m_columns)
            {
                attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
                std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                          << "Column index out of bounds." << std::endl << std::endl;
                attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);

                break;
            }
            else if (j != permutation[j])
            {
                combinatorics::Permutator::computeRelativePermutationVector(permutation, relativePermutation);
                for (std::size_t i = 0, offset = 0; i < m_rows; ++i)
                {
                    std::swap(m_vector[offset + j], m_vector[offset + relativePermutation[j]]);
                    offset += m_columns;
                }
            }
        }
    }

    /**
     * Permute rows of the matrix
     * @param permutation the permutation vector
     */
    inline virtual void permuteRows(const std::vector<std::size_t> &permutation) final
    {
        std::vector<std::size_t> relativePermutation(m_rows);
        permuteRows(permutation, relativePermutation);
    }

    /**
     * Permute rows of the matrix
     * @param      permutation         the permutation vector
     * @param[in]  relativePermutation storage to hold relative permutation vector (will be resized if needed)
     * @param[out] relativePermutation the relative permutation vector
     */
    virtual void permuteRows(const std::vector<std::size_t> &permutation,
                             std::vector<std::size_t> &relativePermutation) final
    {
        auto m = std::min(m_rows, permutation.size());
        std::size_t offset[2] = { 0, 0 };
        for (std::size_t i = 0; i < m; ++i)
        {
            if (permutation[i] >= m_rows)
            {
                attributes::concrete::StaticSynchronizable<Matrix<2, T>>::lock(0);
                std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                          << "Row index out of bounds." << std::endl << std::endl;
                attributes::concrete::StaticSynchronizable<Matrix<2, T>>::unlock(0);

                break;
            }
            else if (i != permutation[i])
            {
                combinatorics::Permutator::computeRelativePermutationVector(permutation, relativePermutation);
                offset[1] = relativePermutation[i] * m_columns;

                for (std::size_t j = 0; j < m_columns; ++j)
                    std::swap(m_vector[offset[0] + j], m_vector[offset[1] + j]);
            }

            offset[0] += m_columns;
        }
    }

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    inline virtual std::ostream &print(std::ostream &stream) const override
    {
        auto &&function = [] (std::ostream &stream, auto &&value) -> std::ostream & { return stream << value; };

        return print(stream, function);
    }

    /**
     * Output stream print function
     * @param stream   a reference to an std::ostream object
     * @param function a binary function object which contains custom code to print and format individual
     *                 elements; it accepts a reference to an std::ostream as its first argument and a value of
     *                 decay_type as its second argument
     */
    template<typename Function>
    std::ostream &print(std::ostream &stream, Function &&function) const
    {
        stream << std::setw(0) << "[";
        auto width = stream.width();
        auto size = m_vector.size();
        for (std::size_t i = 0; i < size;)
        {
            if (i != 0 && i % m_columns == 0)
                stream << ";" << std::endl << " ";

            do
            {
                if (i % m_columns != 0)
                    stream << ",";

                if (width > 0)
                    stream << std::setw(width);

                auto value = m_vector[i];
                std::forward<Function>(function)(stream, value);
            }
            while (++i % m_columns != 0);
        }

        stream << "];";

        return stream;
    }

    /**
     * rbegin() overload
     */
    inline virtual reverse_iterator rbegin(void) override
    {
        return m_vector.rbegin();
    }

    /**
     * Remove the lower and upper triangular portions of the current object's matrix; what remains is a band
     * matrix that includes elements at or above the jth diagonal and at or below the kth diagonal
     * @param above the upper diagonal with respect to the main diagonal
     *              = 0 is the main diagonal
     *              > 0 is above the main diagonal
     *              < 0 is below the main diagonal.
     * @param below the lower diagonal with respect to the main diagonal
     *              = 0 is the main diagonal
     *              > 0 is above the main diagonal
     *              < 0 is below the main diagonal.
     */
    virtual void removeLowerAndUpperTriangles(int above = 1, int below = -1) final
    {
        if (below > above)
            std::swap(below, above);

        std::size_t offset = 0;
        auto size = this->size();
        for (int i = 0; offset < size; ++i, offset += m_columns)
        {
            auto minColumn = std::min(m_columns, static_cast<std::size_t>(std::max(0, i + below)));
            for (std::size_t j = 0; j < minColumn; ++j)
                m_vector[offset + j] = decay_type();

            auto maxColumn = std::min(m_columns, static_cast<std::size_t>(1 + i + above));
            for (auto j = maxColumn; j < m_columns; ++j)
                m_vector[offset + j] = decay_type();
        }
    }

    /**
     * Remove the lower triangular portion of the current object's matrix; what remains is an upper triangular
     * matrix that includes elements above the kth diagonal with respect to the main diagonal of the matrix
     * @param below = 0 is the main diagonal
     *              > 0 is above the main diagonal
     *              < 0 is below the main diagonal
     */
    virtual void removeLowerTriangle(int below = 0) final
    {
        auto above = int(m_columns) - 1;
        if (above < below)
            above = below;

        removeLowerAndUpperTriangles(above, below);
    }

    /**
     * Remove off diagonals from the current object's matrix
     */
    inline virtual void removeOffDiagonals(void) final
    {
        removeLowerAndUpperTriangles(0, 0);
    }

    /**
     * Remove the upper triangular portion of the current object's matrix; what remains is a lower triangular
     * matrix that includes elements below the kth diagonal with respect to the main diagonal of the matrix
     * @param above = 0 is the main diagonal
     *              > 0 is above the main diagonal
     *              < 0 is below the main diagonal
     */
    virtual void removeUpperTriangle(int above = 0) final
    {
        auto below = 1 - int(m_rows);
        if (above < below)
            below = above;

        removeLowerAndUpperTriangles(above, below);
    }

    /**
     * rend() overload
     */
    inline virtual reverse_iterator rend(void) override
    {
        return m_vector.rend();
    }

    /**
     * Resize the current matrix object (without data preservation)
     * @param value will be used to initialize all elements within the matrix
     */
    inline void resize(const decay_type &value, std::size_t rows, std::size_t columns)
    {
        resize(rows, columns, false);

        std::fill(begin(), end(), value);
    }

    /**
     * Resize the current matrix object
     * @param rows          the size along the first dimension
     * @param columns       the size along the second dimension
     * @param bPreserveData flag indicating whether or not to preserve the data in the matrix (optional,
     *                      defaults to true)
     */
    virtual void resize(std::size_t rows, std::size_t columns, bool bPreserveData = true) final
    {
        if (rows != m_rows || columns != m_columns)
        {
            if (!bPreserveData)
                initialize(rows, columns);
            else
            {
                auto minColumns = std::min(columns, m_columns);
                auto minRows = std::min(rows, m_rows);
                std::vector<decay_type> vector(size());
                std::copy(cbegin(), cend(), vector.begin());

                std::swap(columns, m_columns);
                initialize(rows, m_columns);

                std::size_t offset[] = { 0, 0 };
                for (std::size_t i = 0; i < minRows; ++i)
                {
                    for (std::size_t j = 0; j < minColumns; ++j)
                        m_vector[offset[0] + j] = vector[offset[1] + j];

                    if (minColumns < m_columns)
                    {
                        auto &&itOffset = std::next(begin(), offset[0]);
                        std::fill(itOffset + minColumns, itOffset + m_columns, decay_type());
                    }

                    offset[0] += m_columns;
                    offset[1] += columns;
                }
            }
        }
    }

    /**
     * Get the ith row
     */
    inline Matrix<2, decay_type &> row(std::size_t i)
    {
        std::vector<std::size_t> columns(m_columns);
        std::iota(columns.begin(), columns.end(), 0);

        return operator () ({ i }, columns);
    }

    /**
     * Get the ith row and store it in the input row vector
     */
    bool row(std::size_t i, std::vector<decay_type> &row) const
    {
        bool bSuccess = (row.size() == m_columns && i < m_rows);
        if (bSuccess)
        {
            auto offset = i * m_columns;
            for (std::size_t j = 0; j < m_columns; ++j)
                row[j] = m_vector[offset + j];
        }

        return bSuccess;
    }

    /**
     * Get the number of rows in the matrix
     */
    inline virtual std::size_t rows(void) const final
    {
        return m_rows;
    }

    /**
     * Set the jth column of the current object's matrix
     * @param j      the column index
     * @param vector a vector of elements used to initialize the jth column
     */
    bool setColumn(std::size_t j, const std::vector<decay_type> &vector)
    {
        bool bSuccess = (vector.size() == m_rows && j < m_columns);
        if (bSuccess)
        {
            auto size = this->size();
            for (std::size_t i = 0, offset = j; offset < size; offset += m_columns, ++i)
                m_vector[offset] = vector[i];
        }

        return bSuccess;
    }

    /**
     * Assign elements along the main diagonal
     * @param vector a vector of elements used to initialize the main diagonal
     */
    void setDiagonal(const std::vector<decay_type> &vector)
    {
        auto minDimension = std::min(m_columns, m_rows);
        for (std::size_t i = 0, offset = 0; i < minDimension; ++i)
        {
            if (i < vector.size())
                m_vector[offset] = vector[i];

            offset += m_columns + 1;
        }
    }

    /**
     * Assign elements to the main diagonal
     * @param value a constant that will be used to initialize the main diagonal
     */
    void setDiagonal(const decay_type &value)
    {
        auto size = this->size();
        std::size_t offset = 0;
        while (offset < size)
        {
            m_vector[offset] = value;
            offset += m_columns + 1;
        }
    }

    /**
     * Assign elements to the main diagonal and off-diagonals
     * @param diagonal    a constant that will be used to initialize the main diagonal
     * @param offDiagonal a constant that will be used to initialize elements off the main-diagonal
     */
    void setDiagonal(const decay_type &diagonal, const decay_type &offDiagonal)
    {
        auto size = this->size();
        for (std::size_t i = 0, j = 0; i < size; ++i)
        {
            if (i == j)
            {
                m_vector[i] = diagonal;
                j += m_columns + 1;
            }
            else
                m_vector[i] = offDiagonal;
        }
    }

    /**
     * Set the ith row of the current object's matrix
     * @param i      the row index
     * @param vector a vector of elements used to initialize the ith row
     */
    bool setRow(std::size_t i, const std::vector<decay_type> &vector)
    {
        bool bSuccess = (vector.size() == m_columns && i < m_rows);
        if (bSuccess)
        {
            auto offset = i * m_columns;
            for (std::size_t j = 0; j < m_columns; ++j)
                m_vector[offset + j] = vector[j];
        }

        return bSuccess;
    }

    /**
     * Set elements along super-, main-, and sub-diagonals of the current object's matrix
     * @param upper    the superdiagonal elements
     * @param diagonal the diagonal elements
     * @param lower    the subdiagonal elements
     */
    void setTridiagonal(const std::vector<decay_type> &upper, const std::vector<decay_type> &diagonal,
                        const std::vector<decay_type> &lower)
    {
        for (std::size_t i = 0, offset = 0; i < m_rows; ++i)
        {
            if (i > 0 && i <= lower.size() && offset <= size())
                m_vector[offset - 1] = lower[i - 1]; // sub-diagonal element

            if (i < diagonal.size())
                m_vector[offset] = diagonal[i]; // diagonal element

            if (i < upper.size() && offset + 1 < size())
                m_vector[offset + 1] = upper[i]; // super-diagonal element

            offset += m_columns + 1;
        }
    }

    /**
     * Return the number of elements in the matrix
     */
    inline virtual std::size_t size(void) const final
    {
        return m_vector.size();
    }

    /**
     * Swap the contents of the current object with that of another
     */
    bool swap(Matrix<2, decay_type> &matrix)
    {
        bool bSuccess = !matrix.m_vector.empty();
        if (bSuccess)
        {
            std::swap(m_columns, matrix.m_columns);
            std::swap(m_rows, matrix.m_rows);
            std::swap(m_vector, matrix.m_vector);
        }

        return bSuccess;
    }

    /**
     * Transpose the current object's matrix
     */
    virtual void transpose(void)
    {
        if (m_pTempMatrix == nullptr)
            m_pTempMatrix.reset(new Matrix<2, decay_type>);

        std::vector<decay_type> &vector = *m_pTempMatrix;
        vector.resize(m_vector.size());
        std::copy(m_vector.cbegin(), m_vector.cend(), vector.begin());

        std::size_t offset[] = { 0, 0 };
        for (std::size_t i = 0; i < m_rows; ++i, offset[0] += m_columns)
        {
            offset[1] = 0;
            for (std::size_t j = 0; j < m_columns; ++j, offset[1] += m_rows)
                m_vector[offset[1] + i] = vector[offset[0] + j];
        }

        std::swap(m_rows, m_columns);
    }

protected:

    /**
     * the number of columns in the matrix
     */
    std::size_t m_columns;

    /**
     * pointer to a temporary workspace matrix
     */
    mutable std::shared_ptr<Matrix<2, decay_type>> m_pTempMatrix;

    /**
     * the number of rows in the matrix
     */
    std::size_t m_rows;

    /**
     * element storage vector
     */
    std::vector<vector_type> m_vector;
};

}

}

}

#endif
