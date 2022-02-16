#ifndef SUBSCRIPT_H
#define SUBSCRIPT_H

#include "cartesian_product.h"

namespace math
{

namespace sets
{

/**
 * This class generates a set of subscripts that can be used for array and/or matrix indexing. The class also
 * contains methods to convert input array subscript tuples to their corresponding linear index
 */
class Subscript
: public CartesianProduct<std::size_t>
{
private:

    /**
     * Enumerations
     */
    enum SubscriptApplicationType { ApplySubscriptToDestinationVector = 1, ApplySubscriptToSourceVector = 2 };

public:

    /**
     * Constructor
     */
    Subscript(void)
    {

    }

    /**
     * Constructs an object from the subset generated using criteria specified via function object
     * @param value the initial element in the subset
     * @param test  a function object defining the terminating condition
     * @param next  a function object that calculates the next element in the subset
     */
    template<class Test, class Next>
    Subscript(const std::size_t &value, Test &&test, Next &&next)
    : CartesianProduct<std::size_t>(value, std::forward<Test>(test), std::forward<Next>(next))
    {

    }

    /**
     * Construct an object from a dimension vector
     */
    Subscript(const std::vector<std::size_t> &dimension)
    {
        Subscript::generate(dimension, *this);
    }

    /**
     * Construct an object from multiple dimension vectors
     */
    Subscript(const std::vector<std::vector<std::size_t>> &dimensions)
    : CartesianProduct<std::size_t>(dimensions)
    {

    }

    /**
     * Copy constructor
     */
    Subscript(const Subscript &subscript)
    {
        operator = (subscript);
    }

    /**
     * Move constructor
     */
    Subscript(Subscript &&subscript)
    {
        operator = (std::move(subscript));
    }

    /**
     * Destructor
     */
    virtual ~Subscript(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Subscript &operator = (const Subscript &subscript)
    {
        if (&subscript != this)
        {
            CartesianProduct<std::size_t>::operator = (subscript);

            m_tuple = subscript.m_tuple;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Subscript &operator = (Subscript &&subscript)
    {
        if (&subscript != this)
        {
            CartesianProduct<std::size_t>::operator = (std::move(subscript));

            m_tuple = std::move(subscript.m_tuple);
        }

        return *this;
    }

    /**
     * Subscript operator
     * @param  index the linear index
     * @return       the corresponding subscript tuple for a given linear index
     */
    inline virtual std::vector<std::size_t> &operator [] (const std::size_t &index) const final
    {
        getTuple(index, m_tuple);

        return m_tuple;
    }

    /**
     * Function call operator
     * @param  tuple a vector of subscripts
     * @return       the corresponding linear index for a given subscript tuple
     */
    inline virtual std::size_t operator () (const std::vector<std::size_t> &tuple) const final
    {
        return toIndex(tuple);
    }

    /**
     * Clear the object
     */
    inline virtual void clear(void) override
    {
        CartesianProduct<std::size_t>::clear();

        m_tuple.clear();
    }

    /**
     * Generate a subscript object from a vector of dimension sizes. Indices are generated for each dimension
     * ranging from 0 to size - 1 in increments of one.
     * @param      dimensions the size of each dimension
     * @param[in]  subscript  the object for which indices will be generated
     * @param[out] subscript  updated with the generated indices
     */
    static void generate(const std::vector<std::size_t> &dimensions, Subscript &subscript)
    {
        if (!subscript.empty())
            subscript.clear();

        for (std::size_t i = 0; i < dimensions.size(); ++i)
        {
            auto &&test = [&dimensions, &i] (auto &&index) { return index < dimensions[i]; };
            auto &&next = [] (auto &&n) { return n + 1; };
            auto &&indices = Set<std::size_t>::generate(0, test, next);
            subscript.m_subsets.push_back(indices);
        }

        subscript.initialize();
    }

    /**
     * Generate a subscript object from a vector of dimension sizes. Indices are generated for each dimension
     * ranging from 0 to size - 1 in increments of one.
     * @param dimensions the size of each dimension
     */
    inline static Subscript generate(const std::vector<std::size_t> &dims)
    {
        Subscript subscript;

        generate(dims, subscript);

        return subscript;
    }

    /**
     * Get this object's index offset vector
     */
    inline virtual std::vector<std::size_t> getOffsetVector(void) const final
    {
        return m_offset;
    }

    /**
     * This function uses the input subscript object to index a subset of elements in the source vector;
     * Elements from the source are then assigned to the destination vector accordingly. Note: The input
     * subscript object should have the same dimension as the current object
     * @param      source      the source data vector
     * @param[in]  destination storage to hold the desired data
     * @param      subscript   a reference to a Subscript object used to index the source vector
     * @param      functor     a unary function object which performs a user-defined operation on each of the
     *                         elements in the source vector subset upon assignment to the destination vector
     * @param[out] destination modified to contain the desired subset
     */
    template<typename Source, typename Destination, typename UnaryFunctor>
    inline void getVectorSubset(Source &&source, Destination &&destination, const Subscript &subscript,
                                UnaryFunctor &&functor) const
    {
        vectorSubset(std::forward<Source>(source),
                     std::forward<Destination>(destination),
                     subscript,
                     ApplySubscriptToSourceVector,
                     std::forward<UnaryFunctor>(functor));
    }

    /**
     * Initialization function
     */
    virtual bool initialize(void) override
    {
        bool bSuccess = CartesianProduct<std::size_t>::initialize();
        if (bSuccess && !this->m_subsets.empty())
        {
            m_tuple.resize(this->m_subsets.size());
        }

        return bSuccess;
    }

    /**
     * This function uses the input subscript object to index a subset of elements in the destination vector;
     * Elements from the source are then assigned to the destination vector accordingly. Note: The input
     * subscript object should have the same dimension as the current object
     * @param      source      the source data vector
     * @param[in]  destination storage to hold the desired data
     * @param      subscript   a reference to a Subscript object used to index the source vector
     * @param      functor     a unary function object which performs a user-defined operation on each of the
     *                         elements in the source vector subset upon assignment to the destination vector
     * @param[out] destination vector, or a subset thereof, has been modified accordingly
     */
    template<typename Source, typename Destination, typename UnaryFunctor>
    inline void setVectorSubset(Source &&source, Destination &&destination, const Subscript &subscript,
                                UnaryFunctor &&functor) const
    {
        vectorSubset(std::forward<Source>(source),
                     std::forward<Destination>(destination),
                     subscript,
                     ApplySubscriptToDestinationVector,
                     std::forward<UnaryFunctor>(functor));
    }

    /**
     * This method returns the corresponding linear index for a given subscript tuple
     * @param tuple a vector of subscripts
     */
    inline virtual std::size_t toIndex(const std::vector<std::size_t> &tuple) const final
    {
        std::size_t offset = 0;
        for (std::size_t i = 0; i < m_subsets.size(); ++i)
            offset += tuple[i] * m_offset[i];

        return offset;
    }

    /**
     * This method returns a vector of linear indices corresponding to a vector of subscript tuples
     */
    virtual std::vector<std::size_t> &toIndex(const std::vector<std::vector<std::size_t>> &tuples) const final
    {
        for (std::size_t i = 0, j, k; i < tuples.size(); ++i)
        {
            for (j = k = 0; j < m_subsets.size(); ++j)
                k += tuples[i][j] * m_offset[j];

            m_tuple[i] = k;
        }

        return m_tuple;
    }

    /**
     * This method uses an input Subscript object to index the current object, and returns a corresponding
     * vector of linear indices that correspond to the tuples described by the input Subscript object
     */
    inline virtual std::vector<std::size_t> &toIndex(const Subscript &subscript) const final
    {
        return toIndex(subscript.m_subsets);
    }

private:

    /**
     * This function uses the input subscript object to index a subset of elements in the source/destination
     * vector; Elements from source are then assigned to destination. Note: The input subscript object should
     * have the same dimension as the current object
     * @param      source      the source data vector
     * @param[in]  destination storage to hold the desired data
     * @param      subscript   used to index the source/destination vector
     * @param      flags       an integer composed of logically or'd bit flags defined by the
     *                         SubscriptApplicationType enumeration
     * @param      functor     a unary function object which performs a user-defined operation on each of the
     *                         elements in the source vector subset upon assignment to the destination vector
     * @param[out] destination modified to contain the desired subset
     */
    template<typename Source, typename Destination, typename UnaryFunctor>
    void vectorSubset(Source &&source, Destination &&destination, const Subscript &subscript, long flags,
                      UnaryFunctor &&functor) const
    {
        auto &&cardinality = subscript.cardinality();
        std::size_t i, offset;
        auto *pDestOffset = (flags & ApplySubscriptToDestinationVector) ? &offset : &i;
        auto *pSrcOffset = (flags & ApplySubscriptToSourceVector) ? &offset : &i;

        // TODO: need to reverse traversal order to avoid overwriting if source and destination are the same
        for (i = 0; i < cardinality; ++i)
        {
            offset = 0;
            for (std::size_t j = 0, k = i; j < subscript.m_subsets.size(); ++j)
            {
                k = subscript.m_subsets[j][k / subscript.m_offset[j]];
                offset += k * m_offset[j];
                k = i % subscript.m_offset[j];
            }

            destination[*pDestOffset] = functor(source[*pSrcOffset]);
        }
    }

    /**
     * workspace vector to hold an index tuple
     */
    mutable std::vector<std::size_t> m_tuple;
};

}

}

#endif
