#ifndef CARTESIAN_PRODUCT_H
#define CARTESIAN_PRODUCT_H

#include "initializable.h"
#include "set.h"
#include <iomanip>
#include <numeric>
#include <vector>

namespace math
{

namespace sets
{

/**
 * This class implements methods to compute Cartesian products of sets
 */
template<typename T>
class CartesianProduct
: public Set<T>,
  public attributes::interfaces::Initializable,
  public attributes::concrete::OutputStreamable<CartesianProduct<T>>
{
public:

    /**
     * Using declarations
     */
    using Set<T>::addSubset;
    using Set<T>::cardinality;
    using typename Set<T>::iterator;
    using Set<T>::replace;
    using Set<T>::remove;
    using Set<T>::resize;

    /**
     * Constructor
     */
    CartesianProduct(void)
    : m_cardinality(0)
    {

    }

    /**
     * Constructs an object from the subset generated using criteria specified via function object
     * @param value the initial element in the subset
     * @param test  a function object defining the terminating condition
     * @param next  a function object that calculates the next element in the subset
     */
    template<class Test, class Next>
    CartesianProduct(const T &value, Test &&test, Next &&next)
    : Set<T>(value, std::forward<Test>(test), std::forward<Next>(next))
    {
        initialize();
    }

    /**
     * Constructs an object from a subset
     */
    CartesianProduct(const std::vector<T> &subset)
    : Set<T>(subset)
    {
        initialize();
    }

    /**
     * Constructs an object from multiple subsets
     */
    CartesianProduct(const std::vector<std::vector<T>> &subsets)
    : Set<T>(subsets)
    {
        initialize();
    }

    /**
     * Copy constructor
     */
    CartesianProduct(const CartesianProduct<T> &set)
    {
        operator = (set);
    }

    /**
     * Move constructor
     */
    CartesianProduct(CartesianProduct<T> &&set)
    {
        operator = (std::move(set));
    }

    /**
     * Destructor
     */
    virtual ~CartesianProduct(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    CartesianProduct<T> &operator = (const CartesianProduct<T> &set)
    {
        if (&set != this)
        {
            Set<T>::operator = (set);

            m_cardinality = set.m_cardinality;
            m_offset = set.m_offset;
            m_permutation = set.m_permutation;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    CartesianProduct<T> &operator = (CartesianProduct<T> &&set)
    {
        if (&set != this)
        {
            Set<T>::operator = (std::move(set));

            m_cardinality = std::move(set.m_cardinality);
            m_offset = std::move(set.m_offset);
            m_permutation = std::move(set.m_permutation);
        }

        return *this;
    }

    /**
     * Add a subset at the location specified by the input iterator
     * @param itSubset an iterator to the insertion location
     * @param subset   the subset to be inserted
     */
    inline virtual bool addSubset(const iterator &itSubset, const std::vector<T> &subset) override
    {
        bool bSuccess = Set<T>::addSubset(itSubset, subset);
        if (bSuccess)
            bSuccess = initialize();

        return bSuccess;
    }

    /**
     * Add a singleton subset at the location specified by the input iterator
     * @param itSubset an iterator to the insertion location
     * @param value    the value of the singleton
     */
    inline virtual bool addSubset(const iterator &itSubset, const T &value) override
    {
        bool bSuccess = Set<T>::addSubset(itSubset, value);
        if (bSuccess)
            bSuccess = initialize();

        return bSuccess;
    }

    /**
     * Return the cardinality of the Cartesian product
     */
    inline virtual std::size_t cardinality(void) const final
    {
        return m_cardinality;
    }

    /**
     * Clear this object
     */
    inline virtual void clear(void) override
    {
        Set<T>::clear();

        m_cardinality = 0;
        m_offset.clear();
        m_permutation.clear();
    }

    /**
     * Generate the Cartesian product from this object's subsets and populate the input argument with the
     * resulting tuples
     */
    virtual void generateTuples(std::vector<std::vector<T>> &tuples) const final
    {
        if (m_permutation.size() != this->m_subsets.size())
        {
            // generate the permutation vector
            m_permutation.resize(this->m_subsets.size());
            std::iota(m_permutation.begin(), m_permutation.end(), 0);
        }

        generateTuples(tuples, m_permutation);
    }

    /**
     * Generate the Cartesian product from this object's subsets (permuted by the input permutation vector) and
     * populate the input argument with the resulting tuples
     * @param tuples      a vector of tuples which will be populated upon success
     * @param permutation the permutation vector
     */
    virtual void generateTuples(std::vector<std::vector<T>> &tuples,
                                const std::vector<std::size_t> &permutation) const final
    {
        if (tuples.size() != m_cardinality)
            tuples.resize(m_cardinality, std::vector<T>(this->m_subsets.size()));

        for (std::size_t i = 0; i < m_cardinality; ++i)
        {
            for (std::size_t k = i, j = 0; j < this->m_subsets.size(); ++j)
            {
                tuples[i][permutation[j]] = this->m_subsets[j][k / m_offset[j]];
                k = i % m_offset[j];
            }
        }
    }

    /**
     * Get the ith tuple corresponding to the ith linear index
     * @param      index the linear index of the tuple in the Cartesian product
     * @param[in]  tuple storage to hold the output tuple
     * @param[out] tuple that which corresponds to the ith linear index
     */
    inline virtual void getTuple(const std::size_t &index, std::vector<T> &tuple) const final
    {
        m_permutation.clear();

        getTuple(index, tuple, m_permutation);
    }

    /**
     * Get the ith tuple corresponding to the ith linear index
     * @param      index       the linear index of the tuple in the Cartesian product
     * @param[in]  tuple       storage to hold the output tuple
     * @param      permutation if non-empty, contains a permutation sequence to be applied to the tuple
     * @param[out] tuple       the tuple corresponding to the ith linear index
     */
    virtual void getTuple(const std::size_t &index, std::vector<T> &tuple,
                          const std::vector<std::size_t> &permutation) const final
    {
        if (tuple.size() != this->m_subsets.size())
            tuple.resize(this->m_subsets.size());

        for (std::size_t k = index, j = 0; j < this->m_subsets.size(); ++j)
        {
            auto &&i = permutation.empty() ? j : permutation[j];
            tuple[i] = this->m_subsets[j][k / m_offset[j]];
            k = index % m_offset[j];
        }
    }

protected:

    /**
     * Initialization function
     */
    virtual bool initialize(void) override
    {
        if (!this->m_subsets.empty())
        {
            m_offset.resize(this->m_subsets.size());
            m_cardinality = this->m_subsets.front().size();

            auto &&itOffset = m_offset.rbegin();
            auto &&itSet = this->m_subsets.rbegin();
            for (*itOffset = 1; itSet != this->m_subsets.rend() - 1; ++itSet, ++itOffset)
            {
                m_cardinality *= itSet->size();
                *(itOffset + 1) = (*itOffset) * itSet->size();
            }
        }
        else
            m_cardinality = 0;

        return true;
    }

public:

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    virtual std::ostream &print(std::ostream &stream) const override
    {
        auto width = stream.width();
        auto prec = stream.precision();
        stream.width(0);

        for (auto &&itSet = this->cbegin(); itSet != this->cend(); itSet++)
        {
            stream << "{";
            for (auto &&itElement = itSet->cbegin(); itElement != itSet->cend(); ++itElement)
            {
                stream << std::setw(width) << std::setprecision(prec) << *itElement;
                if (itElement != std::prev(itSet->cend()))
                    stream << ",";
            }

            stream << "}" << std::endl;
        }

        return stream;
    }

    /**
     * Remove one or more consecutive subsets at the location specified by the input iterator
     * @param itSubset              an iterator to the removal location
     * @param numConsecutiveSubsets the number of consecutive subsets to be removed, including the subset to
     *                              which the input iterator refers
     */
    inline virtual bool remove(const iterator &itSubset, int numConsecutiveSubsets = 1) override
    {
        bool bSuccess = Set<T>::remove(itSubset, numConsecutiveSubsets);
        if (bSuccess)
            bSuccess = initialize();

        return bSuccess;
    }

    /**
     * Replace an existing subset at the location specified by the input iterator with a singleton
     * @param itSubset an iterator to the replacement location
     * @param value    the value of the singleton
     */
    inline virtual bool replace(const iterator &itSubset, const T &value) override
    {
        bool bSuccess = Set<T>::replace(itSubset, value);
        if (bSuccess)
            bSuccess = initialize();

        return bSuccess;
    }

    /**
     * Replace an existing subset at the location specified by the input iterator with another subset
     * @param  itSubset an iterator to the replacement location
     * @param  subset   the replacement subset
     * @return          an error code according to the following:
     *                  =  1 if subset was successfully replaced, but had to be resized
     *                  =  0 if subset was successfully replaced without resizing
     *                  = -1 if the iterator is invalid
     */
    inline virtual int replace(const iterator &itSubset, const std::vector<T> &subset) override
    {
        auto iError = Set<T>::replace(itSubset, subset);
        if (iError == 1)
            initialize();

        return iError;
    }

    /**
     * Resize this object's vector of subsets; if the new size is larger than the current size, the input
     * subset is used to initialize the additional subsets added
     */
    inline virtual void resize(const std::size_t &size, const std::vector<T> &subset) override
    {
        Set<T>::resize(size, subset);

        initialize();
    }

protected:

    /**
     * the size of the Cartesian product
     */
    mutable std::size_t m_cardinality;

    /**
     * workspace offset vector
     */
    mutable std::vector<std::size_t> m_offset;

    /**
     * workspace permutation vector
     */
    mutable std::vector<std::size_t> m_permutation;
};

}

}

#endif
