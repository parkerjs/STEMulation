#ifndef SET_H
#define SET_H

#include "inherited_iterator.h"
#include "iterable.h"
#include "output_streamable.h"
#include "reverse_iterable.h"
#include <algorithm>
#include <iomanip>
#include <vector>

namespace math
{

namespace sets
{

/**
 * This class models a discrete mathematical set as a collection of multiple subsets
 */
template<typename T>
class Set
: public attributes::abstract::Iterable<iterators::Iterator, std::vector<std::vector<T>>,
                                        iterators::inherited_iterator_tag>,
  public attributes::concrete::OutputStreamable<Set<T>>,
  public attributes::abstract::ReverseIterable<iterators::Iterator, std::vector<std::vector<T>>,
                                               iterators::inherited_iterator_tag>
{
public:

    /**
     * Typedef declarations
     */
    typedef typename attributes::abstract::Iterable<iterators::Iterator, Set<T>, iterators
                               ::inherited_iterator_tag>::const_iterator const_iterator;
    typedef typename attributes::abstract::ReverseIterable<iterators::Iterator, Set<T>, iterators
                               ::inherited_iterator_tag>::const_reverse_iterator const_reverse_iterator;
    typedef typename attributes::abstract::Iterable<iterators::Iterator, Set<T>, iterators
                               ::inherited_iterator_tag>::iterator iterator;
    typedef typename attributes::abstract::ReverseIterable<iterators::Iterator, Set<T>, iterators
                               ::inherited_iterator_tag>::reverse_iterator reverse_iterator;

    /**
     * Constructor
     */
    Set(void)
    {

    }

    /**
     * Constructs an object from the subset generated using criteria specified via function object
     * @param value the initial element in the subset
     * @param test  a function object defining the terminating condition
     * @param next  a function object that calculates the next element in the subset
     */
    template<class Test, class Next>
    Set(const T &value, Test &&test, Next &&next)
    {
        m_subsets.emplace_back(generate(value, std::forward<Test>(test), std::forward<Next>(next)));
    }

    /**
     * Constructs an object from a subset
     */
    Set(const std::vector<T> &subset)
    : m_subsets(1, subset)
    {

    }

    /**
     * Constructs an object from multiple subsets
     */
    Set(const std::vector<std::vector<T>> &subsets)
    : m_subsets(subsets)
    {

    }

    /**
     * Copy constructor
     */
    Set(const Set<T> &set)
    {
        operator = (set);
    }

    /**
     * Move constructor
     */
    Set(Set<T> &&set)
    {
        operator = (std::move(set));
    }

    /**
     * Destructor
     */
    virtual ~Set(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Set<T> &operator = (const Set<T> &set)
    {
        if (&set != this)
        {
            m_cardinalities = set.m_cardinalities;
            m_subsets = set.m_subsets;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Set<T> &operator = (Set<T> &&set)
    {
        if (&set != this)
        {
            m_cardinalities = std::move(set.m_cardinalities);
            m_subsets = std::move(set.m_subsets);
        }

        return *this;
    }

    /**
     * Equality operator
     */
    virtual bool operator == (const Set<T> &set) const final
    {
        if (m_subsets.size() != set.m_subsets.size())
            return false;

        auto &&itSubset = set.cbegin();
        auto &&itThisSubset = this->cbegin();
        while (itSubset != set.cend() && itThisSubset != this->cend())
        {
            if (*itSubset != *itThisSubset)
                return false;

            ++itSubset, ++itThisSubset;
        }

        return true;
    }

    /**
     * Inequality operator
     */
    inline virtual bool operator != (const Set<T> &set) const final
    {
        return !operator == (set);
    }

    /**
     * Add a subset at the location specified by the input iterator
     * @param itSubset an iterator to the insertion location
     * @param subset   the subset to be inserted
     */
    virtual bool addSubset(const iterator &itSubset, const std::vector<T> &subset)
    {
        bool bSuccess = (itSubset == this->cend());
        for (auto &&itThisSubset = this->cbegin(); !bSuccess && itThisSubset != this->end(); ++itThisSubset)
        {
            if (itSubset == itThisSubset)
            {
                // iterator is valid
                bSuccess = true;
                break;
            }
        }

        if (bSuccess)
            m_subsets.insert(itSubset, subset);

        return bSuccess;
    }

    /**
     * Add a subset
     * @param subset the subset to be inserted
     */
    inline virtual void addSubset(const std::vector<T> &subset) final
    {
        addSubset(this->end(), subset);
    }

    /**
     * Add a subset generated from criteria specified via function object at the location specified by the
     * input iterator
     * @param itSubset an iterator to the insertion location
     * @param value    the initial element in the subset
     * @param test     a function object defining the terminating condition
     * @param next     a function object that calculates the next element in the subset
     */
    template<class Test, class Next>
    inline bool addSubset(const iterator &itSubset, const T &value, Test &&test, Next &&next)
    {
        return addSubset(itSubset, generate(value, std::forward<Test>(test), std::forward<Next>(next)));
    }

    /**
     * Add a subset generated from criteria specified via function object
     * @param value the initial element in the subset
     * @param test  a function object defining the terminating condition
     * @param next  a function object that calculates the next element in the subset
     */
    template<class Test, class Next>
    inline void addSubset(const T &value, Test &&test, Next &&next)
    {
        addSubset(this->end(), value, std::forward<Test>(test), std::forward<Next>(next));
    }

    /**
     * Add a singleton subset at the location specified by the input iterator
     * @param itSubset an iterator to the insertion location
     * @param value    the value of the singleton
     */
    virtual bool addSubset(const iterator &itSubset, const T &value)
    {
        bool bSuccess = (itSubset == this->end());
        for (auto &&itThisSubset = this->cbegin(); !bSuccess && itThisSubset != this->cend(); ++itThisSubset)
        {
            if (itSubset == itThisSubset)
            {
                // iterator is valid
                bSuccess = true;
                break;
            }
        }

        if (bSuccess)
            m_subsets.emplace(itSubset, std::vector<T>(1, value));

        return bSuccess;
    }

    /**
     * Add a singleton subset
     * @param value the value of the singleton
     */
    inline virtual void addSubset(const T &value) final
    {
        addSubset(this->end(), value);
    }

    /**
     * begin() overload
     */
    inline virtual iterator begin(void) override
    {
        return m_subsets.begin();
    }

    /**
     * Return a vector of cardinalities representing the sizes of each of this object's subsets
     */
    inline virtual std::vector<std::size_t> &cardinalities(void) const final
    {
        if (m_cardinalities.size() != m_subsets.size())
            m_cardinalities.resize(m_subsets.size());

        std::transform(this->cbegin(), this->cend(), m_cardinalities.begin(),
                       [] (auto &&subset) { return subset.size(); });

        return m_cardinalities;
    }

    /**
     * Return the cardinality of the subset at the given index
     */
    inline virtual std::size_t cardinality(const std::size_t &index) const final
    {
        return std::next(this->cbegin(), index)->size();
    }

    /**
     * cbegin() overload
     */
    inline virtual const_iterator cbegin(void) const override
    {
        return m_subsets.cbegin();
    }

    /**
     * cend() overload
     */
    inline virtual const_iterator cend(void) const override
    {
        return m_subsets.cend();
    }

    /**
     * Clear this object
     */
    inline virtual void clear(void)
    {
        m_subsets.clear();
    }

    /**
     * crbegin() overload
     */
    inline virtual const_reverse_iterator crbegin(void) const override
    {
        return m_subsets.crbegin();
    }

    /**
     * crend() overload
     */
    inline virtual const_reverse_iterator crend(void) const override
    {
        return m_subsets.crend();
    }

    /**
     * Test for empty object
     */
    inline virtual bool empty(void) const final
    {
        return m_subsets.empty();
    }

    /**
     * end() overload
     */
    inline virtual iterator end(void) override
    {
        return m_subsets.end();
    }

    /**
     * Generate a subset of elements using criteria specified via function object
     * @param value the initial element in the subset
     * @param test  a function object defining the terminating condition
     * @param next  a function object that calculates the next element in the subset
     */
    template<class Test, class Next>
    static std::vector<T> generate(const T &value, Test &&test, Next &&next)
    {
        std::vector<T> subset;
        auto element = value;
        while (std::forward<Test>(test)(element))
        {
            subset.push_back(element);
            element = std::forward<Next>(next)(element);
        }

        return subset;
    }

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    virtual std::ostream &print(std::ostream &stream) const override
    {
        auto width = stream.width();
        auto prec = stream.precision();
        for (std::size_t i = 0; i < m_subsets.size(); ++i)
        {
            for (std::size_t j = 0; j < m_subsets[i].size(); ++j)
                stream << std::setw(width) << std::setprecision(prec) << m_subsets[i][j];

            stream << std::endl;
        }

        stream << std::endl;

        return stream;
    }

    /**
     * rbegin() overload
     */
    inline virtual reverse_iterator rbegin(void) override
    {
        return m_subsets.rbegin();
    }

    /**
     * Remove one or more consecutive subsets at the location specified by the input iterator
     * @param itSubset              an iterator to the removal location
     * @param numConsecutiveSubsets the number of consecutive subsets to be removed, including the subset to
     *                              which the input iterator refers
     */
    inline virtual bool remove(const iterator &itSubset, int numConsecutiveSubsets = 1)
    {
        bool bSuccess = (itSubset == this->end());
        for (auto &&itThisSubset = this->cbegin(); !bSuccess && itThisSubset != this->cend(); ++itThisSubset)
        {
            if (itSubset == itThisSubset)
            {
                // iterator is valid
                bSuccess = true;
                break;
            }
        }

        if (bSuccess)
        {
            auto &&numRemainingConsecutiveSubsets = std::distance(itSubset, this->end());
            if (numConsecutiveSubsets > numRemainingConsecutiveSubsets)
                numConsecutiveSubsets = numRemainingConsecutiveSubsets;

            m_subsets.erase(itSubset, std::next(itSubset, numConsecutiveSubsets));
        }

        return bSuccess;
    }

    /**
     * Remove a subset at the location specified by the given index
     */
    inline virtual bool remove(const std::size_t &index) final
    {
        return remove(this->begin() + index);
    }

    /**
     * Remove one or more subsets at the index locations specified within the input vector of indices
     */
    inline virtual bool remove(const std::vector<std::size_t> &indices) final
    {
        bool bSuccess = true;
        for (std::size_t i = 0; i < indices.size(); ++i)
            bSuccess |= remove(this->begin() + indices[i] - i);

        return bSuccess;
    }

    /**
     * rend() overload
     */
    inline virtual reverse_iterator rend(void) override
    {
        return m_subsets.rend();
    }

    /**
     * Replace an existing subset at the location specified by the input iterator with a singleton
     * @param itSubset an iterator to the replacement location
     * @param value    the value of the singleton
     */
    virtual bool replace(const iterator &itSubset, const T &value)
    {
        bool bSuccess = (itSubset == this->end());
        for (auto &&itThisSubset = this->cbegin(); !bSuccess && itThisSubset != this->cend(); ++itThisSubset)
        {
            if (itSubset == itThisSubset)
            {
                // iterator is valid
                bSuccess = true;
                break;
            }
        }

        if (bSuccess)
            itSubset->resize(1, value);

        return bSuccess;
    }

    /**
     * Replace an existing subset at the location specified by the given index with a singleton
     * @param index the index associated with one of this object's subsets
     * @param value the value of the singleton
     */
    inline virtual bool replace(const std::size_t &index, const T &value) final
    {
        return replace(std::next(this->begin(), index), value);
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
    inline virtual int replace(const iterator &itSubset, const std::vector<T> &subset)
    {
        int iError = -1;
        for (auto &&itThisSubset = this->cbegin(); itThisSubset != this->cend(); ++itThisSubset)
        {
            if (itSubset == itThisSubset) // iterator is valid and points to a subset within this object
            {
                iError = 0;
                if (itSubset->size() != subset.size())
                {
                    itSubset->resize(subset.size());
                    iError = 1;
                }

                std::copy(subset.begin(), subset.end(), itSubset->begin());

                break;
            }
        }

        return iError;
    }

    /**
     * Replace an existing subset at the location specified by the input iterator with a subset of elements
     * generated using criteria specified via function object
     * @param itSubset an iterator to the replacement location
     * @param value    the initial element in the subset
     * @param test     a function object defining the terminating condition
     * @param next     a function object that calculates the next element in the subset
     */
    template<class Test, class Next>
    inline void replace(const iterator &itSubset, const T &value, Test &&test, Next &&next)
    {
        return replace(itSubset, generate(value, std::forward<Test>(test), std::forward<Next>(next)));
    }

    /**
     * Resize this object's vector of subsets; if the new size is larger than the current size, the input
     * subset is used to initialize the additional subsets added
     */
    inline virtual void resize(const std::size_t &size, const std::vector<T> &subset)
    {
        m_subsets.resize(size, subset);
    }

    /**
     * Return the number of subsets represented by this object
     */
    inline virtual std::size_t size(void) const final
    {
        return m_subsets.size();
    }

    /**
     * Return a reference to this object's subsets
     */
    inline virtual std::vector<std::vector<T>> &subsets(void) final
    {
        return m_subsets;
    }

protected:

    /**
     * vector of subset sizes
     */
    mutable std::vector<std::size_t> m_cardinalities;

    /**
     * vector of subsets
     */
    std::vector<std::vector<T>> m_subsets;
};

}

}

#endif
