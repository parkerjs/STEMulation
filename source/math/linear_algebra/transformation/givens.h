#ifndef GIVENS_H
#define GIVENS_H

#include "forward_matrix.h"
#include "swappable.h"
#include <cmath>

namespace math
{

namespace linear_algebra
{

namespace transformation
{

/**
 * This class implements an algorithm to compute Givens rotations and includes methods to apply the resulting
 * transformation to matrices
 */
class Givens final
: public attributes::interfaces::Swappable<Givens>
{
public:

    /**
     * Constructor
     */
    Givens(void)
    : m_cosTht(1.0),
      m_r(0.0),
      m_sinTht(0.0)
    {

    }

    /**
     * Copy constructor
     */
    Givens(const Givens &givens)
    {
        operator = (givens);
    }

    /**
     * Move constructor
     */
    Givens(Givens &&givens)
    {
        givens.swap(*this);
    }

    /**
     * Destructor
     */
    ~Givens(void)
    {

    }

    /**
     * Copy assignment operator
     */
    Givens &operator = (const Givens &givens)
    {
        if (&givens != this)
        {
            m_cosTht = givens.m_cosTht;
            m_r = givens.m_r;
            m_sinTht = givens.m_sinTht;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Givens &operator = (Givens &&givens)
    {
        if (&givens != this)
        {
            givens.swap(*this);
        }

        return *this;
    }

    /**
     * Compute cos(theta) and sin(theta) given scalars a and b such that
       \verbatim
           [ c  -s ] [ a ] = [ r ]
           [ s   c ] [ b ]   [ 0 ]
       \endverbatim
     *
     * where theta is selected to zero-out the b coefficient; the subroutine returns r as indicated by the
     * matrix equation above
     * @param[in,out] a a coefficient indicated by the matrix equation above
     * @param[in,out] b a coefficient indicated by the matrix equation above
     */
    virtual void computeRotation(double &a, double &b) final
    {
        auto &&signum = [] (auto &&x) { return x < 0.0 ? -1.0 : 1.0; };
        if (b == 0.0)
        {
            m_cosTht = signum(a);
            m_sinTht = 0.0;
            a = m_r = std::abs(a);
        }
        else if (a == 0.0)
        {
            m_cosTht = 0.0;
            m_sinTht = -signum(b);
            m_r = std::abs(b);
            b = 0.0;
        }
        else if (std::abs(b) > std::abs(a))
        {
            m_cosTht = a / b;
            m_r = signum(b) * sqrt(1.0 + m_cosTht * m_cosTht);
            m_sinTht = 1.0 / -m_r;
            m_cosTht *= -m_sinTht;
            a = (m_r *= b);
            b = 0.0;
        }
        else
        {
            m_sinTht = b / a;
            m_r = signum(a) * sqrt(1.0 + m_sinTht * m_sinTht);
            m_cosTht = 1.0 / m_r;
            m_sinTht *= -m_cosTht;
            a = (m_r *= a);
            b = 0.0;
        }
    }

    /**
     * Post-multiply by the current object's Givens rotation matrix; note that only columns j and k are
     * affected
     * @param      j          the index of the first column
     * @param      k          the index of the second column
     * @param      i          the index of the starting row
     * @param      l          the index of the ending row
     * @param      bTranspose an optional argument which, if true, indicates that the matrix will be
     *                        post-multiplied by the transpose of the Givens rotation matrix
     * @param[out] A          modified after this object's Givens rotation matrix has been applied
     */
    template<typename T>
    void postMultiply(matrix::Matrix<2, T> &A, std::size_t j, std::size_t k, std::size_t i, std::size_t l,
                      bool bTranspose = false) const
    {
        // apply this rotation to Q
        auto s = bTranspose ? -m_sinTht : m_sinTht;
        auto &&n = A.columns();
        auto &&in = i * n, inpj = in + j, inpk = in + k;
        for (; i <= l; ++i, inpj += n, inpk += n)
        {
            auto a = A[inpj];
            auto b = A[inpk];

            A[inpj] = m_cosTht * a + s * b;
            A[inpk] = m_cosTht * b - s * a;
        }
    }

    /**
     * Pre-multiply by the current object's Givens rotation matrix; note that only rows i and k are affected
     * @param      in         the first row offset in the matrix (i * n), where n is the number of columns
     * @param      kn         the second row offset in the matrix (k * n), where n is the number of columns
     * @param      j          the index of the starting column
     * @param      l          the index of the ending column
     * @param      bTranspose an optional argument which, if true, indicates that the matrix will be
     *                        pre-multiplied by the transpose of the Givens rotation matrix
     * @param[out] A          modified after this object's Givens rotation matrix has been applied
     */
    template<typename T>
    void preMultiply(matrix::Matrix<2, T> &A, std::size_t in, std::size_t kn, std::size_t j, std::size_t l,
                     bool bTranspose = false) const
    {
        auto s = bTranspose ? -m_sinTht : m_sinTht;
        for (; j <= l; j++)
        {
            auto a = A[in + j];
            auto b = A[kn + j];

            A[in + j] = m_cosTht * a - s * b;
            A[kn + j] = s * a + m_cosTht * b;
        }
    }

    /**
     * Swap function
     */
    virtual void swap(Givens &givens) override final
    {
        std::swap(m_cosTht, givens.m_cosTht);
        std::swap(m_r, givens.m_r);
        std::swap(m_sinTht, givens.m_sinTht);
    }

protected:

    /**
     * cosine(theta)
     */
    double m_cosTht;

    /**
     * 2-norm of a and b
     */
    double m_r;

    /**
     * sine(theta)
     */
    double m_sinTht;
};

}

}

}

#endif
