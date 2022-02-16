#ifndef GENERIC_MATRIX_ND_H
#define GENERIC_MATRIX_ND_H

#include "general_matrix.h"
#include "inherited_iterator.h"
#include "iterable.h"
#include "output_streamable.h"
#include "permutator.h"
#include "reference_matrix_nd.h"
#include "reflective.h"
#include "reverse_iterable.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include <algorithm>
#include <vector>

namespace math
{

namespace linear_algebra
{

namespace matrix
{

/**
 * N-dimensional generic data type matrix class
 */
template<std::size_t N, typename T>
class Matrix<N, T, GeneralMatrix>
: public GeneralMatrix,
  public attributes::abstract::Iterable<iterators::Iterator,
                                        std::vector<typename std::conditional<std::is_reference<T>::value,
                                        functional::VariableWrapper<T &>, T>::type>,
                                        iterators::inherited_iterator_tag>,
  public attributes::concrete::OutputStreamable<Matrix<N, T>>,
  virtual private attributes::abstract::Reflective,
  public attributes::abstract::ReverseIterable<iterators::Iterator,
                                               std::vector<typename std::conditional<std::is_reference<T>::value,
                                               functional::VariableWrapper<T &>, T>::type>,
                                               iterators::inherited_iterator_tag>,
  public attributes::concrete::StaticMutexMappable<Matrix<N, T>, int, std::mutex *>,
  public attributes::concrete::StaticSynchronizable<Matrix<N, T>>
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
    {

    }

    /**
     * Constructor
     * @param subscript a reference to a Subscript object
     */
    Matrix(const sets::Subscript &subscript)
    {
        initialize(subscript.cardinalities());
    }

    /**
     * Constructor
     * @param m         the size of the first matrix dimension
     * @param n         the size of the second matrix dimension
     * @param p         the size of the third matrix dimension
     * @param dimension a variadic list of higher-order matrix dimension sizes
     */
    template<typename ... Dimension>
    Matrix(std::size_t m, std::size_t n, std::size_t p, Dimension ... dimension)
    {
        std::vector<std::size_t> dimensions = { m, n, p, static_cast<std::size_t>(dimension) ... };

        initialize(dimensions);
    }

    /**
     * Constructor
     * @param dimensions a vector of matrix dimension sizes
     */
    Matrix(const std::vector<std::size_t> &dimensions)
    {
        initialize(dimensions);
    }

    /**
     * Constructor
     * @param data      an stl-like container of elements that will be used to populate the constructed matrix
     * @param m         the size of the first matrix dimension
     * @param n         the size of the second matrix dimension
     * @param p         the size of the third matrix dimension
     * @param dimension a variadic list of higher-order matrix dimension sizes
     */
    template<typename Data, typename ... Dimension>
    inline Matrix(Data &&data,
                  typename std::enable_if<!std::is_fundamental<Data>::value, std::size_t>::type m,
                  std::size_t n, std::size_t p, Dimension ... dimension)
    {
        std::vector<std::size_t> dimensions = { m, n, p, dimension ... };

        initialize(dimensions);

        std::copy(std::forward<Data>(data).cbegin(),
                  std::forward<Data>(data).cbegin() + m_vector.size(), begin());
    }

    /**
     * Constructor
     * @param data       an stl-like container of elements that will be used to populate the constructed matrix
     * @param dimensions a vector of matrix dimension sizes
     */
    template<typename Data>
    Matrix(Data &&data, const std::vector<std::size_t> &dimensions)
    {
        initialize(dimensions);

        std::copy(std::forward<Data>(data).cbegin(),
                  std::forward<Data>(data).cbegin() + m_vector.size(), begin());
    }

    /**
     * Copy constructor
     */
    Matrix(const Matrix<N, T, GeneralMatrix> &matrix)
    {
        operator = (matrix);
    }

    /**
     * Copy constructor
     */
    template<typename U, typename V, typename std::enable_if<!std::is_same<U, T>::value, int>::type = 0>
    Matrix(const Matrix<N, U, V> &matrix)
    {
        operator = (matrix);
    }

    /**
     * Move constructor
     */
    Matrix(Matrix<N, T, GeneralMatrix> &&matrix)
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
    auto &operator = (const Matrix<N, T, GeneralMatrix> &matrix)
    {
        return copy(matrix);
    }

    /**
     * Copy assignment operator
     */
    template<typename U, typename V, typename std::enable_if<!std::is_same<U, T>::value, int>::type = 0>
    auto &operator = (const Matrix<N, U, V> &matrix)
    {
        return copy(matrix);
    }

    /**
     * Move assignment operator
     */
    auto &operator = (Matrix<N, T, GeneralMatrix> &&matrix)
    {
        if (dynamic_cast<const Matrix<N, T> *>(&matrix) != this)
        {
            m_vector = std::move(matrix.m_vector);
            m_subscript = std::move(matrix.m_subscript);
        }

        return *this;
    }

    /**
     * Assignment operator
     * @param value used to initialize all elements within the matrix
     */
    template<typename Value, typename std::enable_if<std::is_same<Value, decay_type>::value, int>::type = 0>
    auto &operator = (Value &&value)
    {
        std::fill_n(begin(), size(), value);

        return *this;
    }

    /**
     * Assignment operator
     * @param vector used to initialize this object's vector of elements
     */
    template<typename Vector>
    inline typename std::enable_if<std::is_same<typename std::decay<Vector>::type,
                                   std::vector<decay_type>>::value, Matrix<N, decay_type> &>::type
    operator = (Vector &&vector)
    {
        if (m_vector.size() == vector.size())
            std::copy(vector.cbegin(), vector.cend(), begin());
        else
        {
            attributes::concrete::StaticSynchronizable<Matrix<N, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Source and destination vectors are not the same length." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<N, T>>::unlock(0);
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
    inline operator Matrix<N, T> & (void)
    {
        return static_cast<Matrix<N, T> &>(*this);
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
     * @param  subscript a reference to a Subscript object used to index a subset of this matrix object
     * @return           a matrix composed of references to elements addressed by using the indices described
     *                   by the input subscript object
     */
    inline Matrix<N, decay_type &> operator () (const sets::Subscript &subscript)
    {
        return Matrix<N, decay_type &>(*this, subscript);
    }

    /**
     * Function call operator
     * @param m         the first index in the indexing tuple
     * @param n         the second index in the indexing tuple
     * @param p         the third index in the indexing tuple
     * @param subscript a variadic list of higher-order indices in the indexing tuple
     */
    template<typename ... Subscript>
    inline decay_type &operator () (std::size_t m, std::size_t n, std::size_t p, Subscript ... subscript)
    {
        std::vector<std::size_t> indices = { m, n, p, static_cast<std::size_t>(subscript) ... };

        return m_vector[m_subscript(indices)];
    }

    /**
     * Function call operator
     * @param m         the first index in the indexing tuple
     * @param n         the second index in the indexing tuple
     * @param p         the third index in the indexing tuple
     * @param subscript a variadic list of higher-order indices in the indexing tuple
     */
    template<typename ... Subscript>
    inline auto operator () (std::size_t m, std::size_t n, std::size_t p, Subscript ... subscript) const
    {
        std::vector<std::size_t> indices = { m, n, p, subscript ... };

        return m_vector[m_subscript(indices)];
    }

    /**
     * Equality operator
     */
    inline bool operator == (const Matrix<N, decay_type> &matrix) const
    {
        return m_subscript == matrix.m_subscript && m_vector == matrix.m_vector;
    }

    /**
     * Inequality operator
     */
    inline bool operator != (const Matrix<N, decay_type> &matrix) const
    {
        return !operator == (matrix);
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
        m_vector.clear();
        m_subscript.clear();
    }

protected:

    /**
     * Copy function
     */
    template<typename U, typename V>
    auto &copy(const Matrix<N, U, V> &matrix)
    {
        if (dynamic_cast<const Matrix<N, T> *>(&matrix) != this)
        {
            m_subscript = matrix.subscript();

            auto &&size = m_subscript.cardinality();
            if (m_vector.size() != size)
                m_vector.resize(size);

            // intentionally not using std::copy here...
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
        return N;
    }

    /**
     * Return a vector containing this matrix object's dimensions
     */
    inline virtual std::vector<std::size_t> dimensions(void) const final
    {
        return m_subscript.cardinalities();
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
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "Matrix<" + std::to_string(N) + ", " + typeid(T).name() + ">";
    }

    /**
     * Initialization function
     */
    virtual void initialize(const std::vector<std::size_t> &dimensions) final
    {
        if (dimensions != this->dimensions())
            sets::Subscript::generate(dimensions, m_subscript);

        auto &&size = m_subscript.cardinality();
        if (m_vector.size() != size)
        {
            if (m_vector.size() < size)
                m_vector.clear();

            m_vector.resize(size);
        }
    }

    /**
     * Map the elements along the specified matrix dimension(s) to (a) single value(s) via function object
     * @param dimensions the matrix dimensions along which the function object will be applied
     * @param function   a binary function object which accepts a vector of indices along a dimension as its
     *                   first argument and the index of the associated dimension as its second argument
     */
    template<std::size_t M, typename Function>
    Matrix<M, decay_type> map(const std::vector<std::size_t> &dimensions, Function &&function) const
    {
        std::vector<decay_type> u(cbegin(), cend()), vector;
        sets::Subscript subscript(m_subscript);

        for (auto &&itDimension = dimensions.cbegin(); itDimension != dimensions.cend(); ++itDimension)
        {
            vector.resize(m_subscript.cardinality(*itDimension));
            subscript.replace(*itDimension, 0);

            auto cardinality = subscript.cardinality();
            for (std::size_t i = 0, j = 0; i < cardinality; ++i)
            {
                auto &tuple = subscript[i];
                for (auto k = vector.size(); k > 0; --k)
                {
                    tuple[*itDimension] = k - 1;
                    j = m_subscript(tuple);
                    vector[k - 1] = u[j];
                }

                u[j] = std::forward<Function>(function)(vector, *itDimension);
            }
        }

        m_subscript.getVectorSubset(u, u, subscript, [] (auto &&arg) { return arg; });
        u.resize(subscript.cardinality());

        return Matrix<N, decay_type>(u, subscript.cardinalities()).template squeeze<M>();
    }

    /**
     * Map the elements along the specified matrix dimension to a single value via function object
     * @param dimension the matrix dimension along which the function object will be applied
     * @param function  a binary function object which accepts a vector of indices along a dimension as its
     *                  first argument and the index of the associated dimension as its second argument
     */
    template<typename Function>
    inline Matrix<N - 1, decay_type> map(std::size_t dimension, Function &&function) const
    {
        return map<N - 1>(std::vector<std::size_t>{ dimension }, std::forward<Function>(function));
    }

    /**
     * Permute elements along a given dimension of this matrix
     * @param dimension   the dimension along which the permutation will be applied
     * @param permutation the permutation vector
     */
    virtual void permute(std::size_t dimension, const std::vector<std::size_t> &permutation) final
    {
        sets::Subscript subscript(m_subscript);
        subscript.replace(std::next(subscript.begin(), dimension), 0);

        if (dimension >= m_subscript.size())
        {
            attributes::concrete::StaticSynchronizable<Matrix<N, T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Invalid dimension specified." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<Matrix<N, T>>::unlock(0);
        }
        else
        {
            auto cardinality = subscript.cardinality();
            auto size = std::min(this->size(dimension), permutation.size());
            auto &&relativePermutation = combinatorics::Permutator::
                                         computeRelativePermutationVector(permutation);

            // now swap along the specified dimension
            for (std::size_t i = 0; i < cardinality; ++i)
            {
                auto &tuple = subscript[i];
                for (std::size_t j = 0; j + 1 < size; ++j)
                {
                    if (j != relativePermutation[j])
                    {
                        tuple[dimension] = j;
                        auto k_j = m_subscript(tuple);

                        tuple[dimension] = relativePermutation[j];
                        auto k_qj = m_subscript(tuple);

                        std::swap(m_vector[k_j], m_vector[k_qj]);
                    }
                }
            }
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
        auto width = stream.width();
        sets::Subscript subscript(m_subscript);
        std::vector<std::size_t> tuple(subscript.size());

        subscript.remove(subscript.begin(), 2);

        auto cardinality = subscript.cardinality();
        for (std::size_t i = 0; i < cardinality; ++i)
        {
            stream << std::setw(0) << "m(:,:";

            auto indices = subscript[i];
            for (std::size_t j = 2; j < N; ++j)
            {
                tuple[j] = indices[j - 2];
                stream << "," << tuple[j];
            }

            stream << ") = " << std::endl << std::endl << "[";

            for (std::size_t j = 0; j < size(0); ++j)
            {
                if (j != 0)
                    stream << ";" << std::endl << " ";

                for (std::size_t k = 0; k < size(1); ++k)
                {
                    tuple[0] = j;
                    tuple[1] = k;

                    if (k != 0)
                        stream << ",";

                    if (width > 0)
                        stream << std::setw(width);

                    auto value = m_vector[m_subscript(tuple)];
                    std::forward<Function>(function)(stream, value);
                }
            }

            stream << "];";

            if (i + 1 != cardinality)
                std::cout << std::endl << std::endl;
        }

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
     * rend() overload
     */
    inline virtual reverse_iterator rend(void) override
    {
        return m_vector.rend();
    }

    /**
     * Resize the current matrix object (without data preservation)
     * @param m         the size of the first matrix dimension
     * @param n         the size of the second matrix dimension
     * @param p         the size of the third matrix dimension
     * @param dimension a variadic list of higher-order matrix dimension sizes
     */
    template<typename ... Dimension>
    inline void resize(std::size_t m, std::size_t n, std::size_t p, Dimension ... dimension)
    {
        std::vector<std::size_t> dimensions = { m, n, p, dimension ... };
        if (dimensions != this->dimensions())
            initialize(dimensions);
    }

    /**
     * Resize the current matrix object (without data preservation)
     * @param dimension an array of sizes along each dimension of the resized matrix
     */
    inline virtual void resize(const std::size_t (&dimension)[N]) final
    {
        std::vector<std::size_t> dimensions(dimension, dimension + N);
        if (dimensions != this->dimensions())
            initialize(dimensions);
    }

    /**
     * Resize the current matrix object (without data preservation)
     * @param dimension a vector of sizes along each dimension of the resized matrix
     */
    inline virtual void resize(const std::vector<std::size_t> &dimensions) final
    {
        if (dimensions != this->dimensions())
            initialize(dimensions);
    }

    /**
     * Resize the current matrix object (without data preservation)
     * @param value     will be used to initialize all elements within the matrix
     * @param m         the size of the first matrix dimension
     * @param n         the size of the second matrix dimension
     * @param p         the size of the third matrix dimension
     * @param dimension a variadic list of higher-order matrix dimension sizes
     */
    template<typename ... Dimension>
    inline void resize(const decay_type &value, std::size_t m, std::size_t n, std::size_t p,
                       Dimension ... dimension)
    {
        std::vector<std::size_t> dimensions = { m, n, p, dimension ... };
        if (dimensions != this->dimensions())
            initialize(dimensions);

        std::fill(begin(), end(), value);
    }

    /**
     * Resize the current matrix object (without data preservation)
     * @param value     will be used to initialize all elements within the matrix
     * @param dimension an array of sizes along each dimension of the resized matrix
     */
    inline void resize(const decay_type &value, const std::size_t (&dimension)[N])
    {
        resize(dimension);

        std::fill(begin(), end(), value);
    }

    /**
     * Resize the current matrix object (without data preservation)
     * @param value     will be used to initialize all elements within the matrix
     * @param dimension a vector of sizes along each dimension of the resized matrix
     */
    inline void resize(const decay_type &value, const std::vector<std::size_t> &dimensions)
    {
        resize(dimensions);

        std::fill(begin(), end(), value);
    }

    /**
     * Resize the current matrix object (with data preservation)
     * @param m         the size of the first matrix dimension
     * @param n         the size of the second matrix dimension
     * @param p         the size of the third matrix dimension
     * @param dimension a variadic list of higher-order matrix dimension sizes
     */
    template<typename ... Dimension>
    inline void resizeAndPreserve(std::size_t m, std::size_t n, std::size_t p, Dimension ... dimension)
    {
        std::vector<std::size_t> dimensions = { m, n, p, dimension ... };

        resizeAndPreserve(dimensions);
    }

    /**
     * Resize the current matrix object (with data preservation)
     * @param dimension an array of sizes along each dimension of the resized matrix
     */
    inline virtual void resizeAndPreserve(const std::size_t (&dimension)[N]) final
    {
        std::vector<std::size_t> dimensions(dimension, dimension + N);

        resizeAndPreserve(dimensions);
    }

    /**
     * Resize the current matrix object (with data preservation)
     * @param dimension a vector of sizes along each dimension of the resized matrix
     */
    virtual void resizeAndPreserve(const std::vector<std::size_t> &dimensions) final
    {
        if (dimensions != this->dimensions())
        {
            sets::Subscript subscript(std::min(dimensions, this->dimensions()));
            std::vector<decay_type> vector(subscript.cardinality());

            auto assignmentFunction = [] (auto &&arg) { return arg; };
            m_subscript.getVectorSubset(m_vector, vector, subscript, assignmentFunction);

            initialize(dimensions);

            m_subscript.setVectorSubset(vector, m_vector, subscript, assignmentFunction);
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
     * Return size of the specified matrix dimension
     */
    inline virtual std::size_t size(std::size_t dimension) const final
    {
        auto &&itSubscript = std::next(m_subscript.cbegin(), dimension);

        return itSubscript->size();
    }

    /**
     * Return a copy of this matrix object with singleton dimensions removed
     */
    template<std::size_t M>
    Matrix<M, decay_type> squeeze(void) const
    {
        std::vector<std::size_t> dimensions(this->dimensions());
        auto size = std::min<std::size_t>(M, 2);
        std::vector<decay_type> vector(cbegin(), cend());

        for (;;)
        {
            auto itDimension = find_if(dimensions.cbegin(), dimensions.cend(),
                                       [] (auto &&x) { return x == 1; });
            if (itDimension != dimensions.end() && dimensions.size() > size)
                dimensions.erase(itDimension);
            else
                break;
        }

        return Matrix<M, decay_type>(vector, dimensions);
    }

    /**
     * Get this object's subscript object
     */
    inline virtual sets::Subscript subscript(void) const final
    {
        return m_subscript;
    }

    /**
     * Swap two matrices
     */
    bool swap(Matrix<N, decay_type> &matrix)
    {
        if (!matrix.m_vector.empty())
        {
            std::swap(m_subscript, matrix.m_subscript);
            std::swap(m_vector, matrix.m_vector);

            return true;
        }

        return false;
    }

protected:

    /**
     * subscript object
     */
    sets::Subscript m_subscript;

    /**
     * element storage vector
     */
    std::vector<vector_type> m_vector;
};

}

}

}

#endif
