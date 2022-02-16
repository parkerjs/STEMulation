#ifndef PERMUTATOR_H
#define PERMUTATOR_H

#include "cloneable.h"
#include "swappable.h"
#include <vector>

namespace math
{

namespace combinatorics
{

/**
 * This class facilitates the application of permutation vectors to sequences of data
 */
class Permutator final
: public attributes::interfaces::Cloneable<Permutator>,
  public attributes::interfaces::Swappable<Permutator>
{
public:

    /**
     * Constructor
     */
    Permutator(void)
    {

    }

    /**
     * Constructor
     */
    Permutator(const std::vector<std::size_t> &permutation)
    {
        setPermutationVector(permutation);
    }

    /**
     * Copy constructor
     */
    Permutator(const Permutator &permutator)
    {
        operator = (permutator);
    }

    /**
     * Move constructor
     */
    Permutator(Permutator &&permutator)
    {
        permutator.swap(*this);
    }

    /**
     * Destructor
     */
    virtual ~Permutator(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Permutator &operator = (const Permutator &permutator)
    {
        if (&permutator != this)
        {
            m_permutation = permutator.m_permutation;
            m_relativePermutation = permutator.m_relativePermutation;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Permutator &operator = (Permutator &&permutator)
    {
        if (&permutator != this)
        {
            permutator.swap(*this);
        }

        return *this;
    }

    /**
     * Apply the permutation vector to the input data sequence
     * @param[in]  sequence that which is to be permuted
     * @param[out] sequence the result of the permutation
     */
    template<typename T>
    void applyToSequence(std::vector<T> &sequence) const
    {
        // apply relative permutation to sequence
        auto &&size = std::min(sequence.size(), m_relativePermutation.size());
        for (std::size_t i = 0; i < size; ++i)
            std::swap(sequence[m_relativePermutation[i]], sequence[i]);
    }

    /**
     * Apply the permutation vector to the input data sequence
     * @param[in]  sequence    that which is to be permuted
     * @param[in]  permutation the permutation vector
     * @param[out] sequence    the result of the permutation
     * @param[out] permutation vector has been destroyed
     */
    template<typename T>
    inline void applyToSequence(std::vector<T> &sequence, std::vector<std::size_t> &permutation) const
    {
        applyToSequence(sequence, permutation, m_relativePermutation);
    }

    /**
     * Apply the permutation to the input data sequence
     * @param[in]  sequence            that which is to be permuted
     * @param[in]  permutation         the permutation vector
     * @param[in]  relativePermutation storage to hold relative permutation vector (will be resized if needed)
     * @param[out] sequence            the result of the permutation
     * @param[out] permutation         vector has been destroyed
     * @param[out] relativePermutation the relative permutation vector
     */
    template<typename T>
    inline static void applyToSequence(std::vector<T> &sequence,
                                       std::vector<std::size_t> &permutation,
                                       std::vector<std::size_t> &relativePermutation)
    {
        computeRelativePermutationVector(permutation, relativePermutation);

        // apply inverse permutation to sequence
        auto &&size = std::min(sequence.size(), relativePermutation.size());
        for (std::size_t i = 0; i < size; ++i)
            std::swap(sequence[relativePermutation[i]], sequence[i]);
    }

    /**
     * clone() function
     */
    inline virtual Permutator *clone(void) const override
    {
        return new Permutator(*this);
    }

    /**
     * Compute the inverse permutation vector
     * @param[in]  permutation        the permutation vector
     * @param[in]  inversePermutation storage to hold inverse permutation vector (will be resized if needed)
     * @param[out] permutation        vector has been destroyed
     * @param[out] inversePermutation the inverse permutation
     */
    static void computeInversePermutationVector(std::vector<std::size_t> &permutation,
                                                std::vector<std::size_t> &inversePermutation)
    {
        auto &&size = permutation.size();
        if (inversePermutation.size() != size)
            inversePermutation.resize(size);

        for (std::size_t i = 0; i < size; ++i)
            inversePermutation[i] = i;

        // now determine inverse permutation vector
        for (std::size_t i = 0; i < size; ++i)
        {
            for (std::size_t j = i; j < size; ++j)
            {
                if (i == permutation[j])
                {
                    if (i != j)
                    {
                        std::swap(permutation[i], permutation[j]);
                        std::swap(inversePermutation[i], inversePermutation[j]);
                    }

                    break;
                }
            }
        }
    }

    /**
     * Compute inverse permutation vector
     * @param[in]  permutation the permutation vector
     * @param[out] permutation vector has been destroyed
     */
    inline static std::vector<std::size_t>
    computeInversePermutationVector(std::vector<std::size_t> &permutation)
    {
        std::vector<std::size_t> relativePermutation(permutation.size());

        computeInversePermutationVector(permutation, relativePermutation);

        return relativePermutation;
    }

    /**
     * Compute relative permutation vector. This function computes this object's relative permutation vector in
     * a "relative" sense, i.e, it is assumes that for the i-th element in a data sequence, the preceding
     * elements have been permuted and are in proper order. The remaining elements, however, may or may not be
     * in proper order. The i-th index in the relative permutation vector will therefore be computed so as to
     * define which of the j remaining elements is to be swapped with the i-th element.
     */
    void computeRelativePermutationVector(void)
    {
        computeRelativePermutationVector(m_permutation, m_relativePermutation);
    }

    /**
     * Compute relative permutation vector. This function computes a permutation vector in a "relative" sense,
     * i.e, it is assumes that for the i-th element in a data sequence, the preceding elements have been
     * permuted and are in proper order. The remaining elements, however, may or may not be in proper order.
     * The i-th index in the relative permutation vector will therefore be computed so as to define which of
     * the j remaining elements is to be swapped with the i-th element.
     * @param  permutation the permutation vector
     * @return             the relative permutation vector
     */
    inline static std::vector<std::size_t>
    computeRelativePermutationVector(const std::vector<std::size_t> &permutation)
    {
        std::vector<std::size_t> relativePermutation(permutation.size());

        computeRelativePermutationVector(permutation, relativePermutation);

        return relativePermutation;
    }

    /**
     * Compute relative permutation vector. This function computes a permutation vector in a "relative" sense,
     * i.e, it is assumes that for the i-th element in a data sequence, the preceding elements have been
     * permuted and are in proper order. The remaining elements, however, may or may not be in proper order.
     * The i-th index in the relative permutation vector will therefore be computed so as to define which of
     * the j remaining elements is to be swapped with the i-th element.
     * @param      permutation         the permutation vector
     * @param[in]  relativePermutation storage to hold relative permutation vector (will be resized if needed)
     * @param[out] relativePermutation the relative permutation vector
     */
    static void computeRelativePermutationVector(const std::vector<std::size_t> &permutation,
                                                 std::vector<std::size_t> &relativePermutation)
    {
        auto &&size = permutation.size();
        if (relativePermutation.size() != size)
            relativePermutation.resize(size);

        for (std::size_t i = 0; i < size; ++i)
            relativePermutation[i] = i;

        // now determine relative permutation vector
        for (std::size_t i = 0; i < size; ++i)
        {
            for (std::size_t j = i; j < size; ++j)
            {
                if (relativePermutation[j] == permutation[i])
                {
                    relativePermutation[j] = relativePermutation[i];
                    relativePermutation[i] = j;

                    break;
                }
            }
        }
    }

    /**
     * Get the permutation vector
     */
    inline virtual std::vector<std::size_t> &getPermutationVector(void) final
    {
        return m_permutation;
    }

    /**
     * Get the permutation vector
     */
    inline virtual std::vector<std::size_t> getPermutationVector(void) const final
    {
        return m_permutation;
    }

    /**
     * Get the relative permutation vector
     */
    inline virtual std::vector<std::size_t> &getRelativePermutationVector(void)final
    {
        return m_relativePermutation;
    }

    /**
     * Get the relative permutation vector
     */
    inline virtual std::vector<std::size_t> getRelativePermutationVector(void) const final
    {
        return m_relativePermutation;
    }

    /**
     * Set the permutation vector
     */
    inline virtual void setPermutationVector(const std::vector<std::size_t> &permutation) final
    {
        m_permutation = permutation;

        computeRelativePermutationVector(m_permutation, m_relativePermutation);
    }

    /**
     * Swap function
     */
    virtual void swap(Permutator &permutator) override final
    {
        m_permutation.swap(permutator.m_permutation);
        m_relativePermutation.swap(permutator.m_relativePermutation);
    }

private:

    /**
     * permutation vector
     */
    std::vector<std::size_t> m_permutation;

    /**
     * relative permutation vector
     */
    std::vector<std::size_t> m_relativePermutation;
};

}

}

#endif
