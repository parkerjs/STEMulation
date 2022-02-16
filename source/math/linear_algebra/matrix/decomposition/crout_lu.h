#ifndef CROUT_LU_H
#define CROUT_LU_H

#include "lu.h"

namespace math
{

namespace linear_algebra
{

namespace matrix
{

namespace decomposition
{

/**
 * This class contains methods to compute LU factorizations of m x n matrices using Crout's method
 */
template<typename Matrix>
class Crout_LU_Factor
: public LU_Factor<Matrix>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Typedef declarations
     */
    typedef typename LU_Factor<Matrix>::T T;

    /**
     * Using declarations
     */
    using LU_Factor<Matrix>::factor;

    /**
     * Constructor
     */
    Crout_LU_Factor(void)
    : LU_Factor<Matrix>(TriangularMatrixType::Enum::Upper)
    {

    }

    /**
     * Copy constructor
     */
    Crout_LU_Factor(const Crout_LU_Factor<Matrix> &solver)
    : LU_Factor<Matrix>(TriangularMatrixType::Enum::Upper)
    {
        operator = (solver);
    }

    /**
     * Move constructor
     */
    Crout_LU_Factor(Crout_LU_Factor<Matrix> &&solver)
    : LU_Factor<Matrix>(TriangularMatrixType::Enum::Upper)
    {
        operator = (std::move(solver));
    }

    /**
     * Destructor
     */
    ~Crout_LU_Factor(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Crout_LU_Factor<Matrix> &operator = (const Crout_LU_Factor<Matrix> &solver)
    {
        if (&solver != this)
        {
            LU_Factor<Matrix>::operator = (solver);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Crout_LU_Factor<Matrix> &operator = (Crout_LU_Factor<Matrix> &&solver)
    {
        if (&solver != this)
        {
            LU_Factor<Matrix>::operator = (std::move(solver));
        }

        return *this;
    }

    /**
     * Factor the equation Ax = b using Crout LU decomposition. The factorization has the form PA = LU, where L
     * is lower triangular, U is unit upper triangular, A is the original matrix, and P is a row permutation
     * matrix. The resultant factorization is returned in a single matrix.
     * @param[in]  A the matrix to be factored
     * @param[out] A the resultant LU factorization
     * @return       an integer value according to the following:
     *               = 1 success, row pivoting was performed
     *               = 0 success, pivoting was not performed
     */
    virtual int factor(Matrix &A) override final
    {
        // initialize pivot vector
        auto m = A.rows();
        this->initialize(PivotType::Enum::Row, m);

        int iError = 0;
        auto n = A.columns();
        auto size = A.size();
        for (std::size_t i = 0, in = 0; i < m; ++i, in += n)
        {
            bool bSuccess = this->pivot(A, i, in, iError);
            if (!bSuccess)
                break;

            // compute the diagonal and lower triangular portions
            for (auto jn = in; jn < size; jn += n)
            {
                T sum(0.0);
                for (std::size_t k = 0, kn = 0; kn < in; ++k, kn += n)
                    sum += A[jn + k] * A[kn + i];

                A[jn + i] -= sum;
            }

            // compute the upper triangular portion
            for (auto j = i + 1; j < n; ++j)
            {
                T sum(0.0);
                for (std::size_t k = 0, kn = 0; kn < in; ++k, kn += n)
                    sum += A[in + k] * A[kn + j];

                A[in + j] = (A[in + j] - sum) / A[in + i];
            }
        }

        return iError;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return std::string("Crout_LU_Factor<") + typeid(Matrix).name() + ">";
    }

    /**
     * Get the lower triangle
     */
    inline virtual void getLowerTriangle(const Matrix &LU, Matrix &L) const override final
    {
        L = LU.getLowerTriangle();
    }

    /**
     * Get the upper triangle
     */
    inline virtual void getUpperTriangle(const Matrix &LU, Matrix &U) const override final
    {
        U = LU.getUpperTriangle(1);
        U.setDiagonal(1);
    }

    /**
     * Crout LU factorization update. This subroutine updates an LU decomposition (equivalent to performing
     * LU-factorization on the quantity A + xy', where x and y are column vectors); the subroutine returns true
     * if the factorization was updated.
     * @param[in]  LU   the factorization to be updated
     * @param[in]  x, y are column vectors used to form the product xy'. The resultant outer product is used to
     *                  update the factorization
     * @param[out] LU   the updated factorization
     * @param[out] x, y have been destroyed
     */
    inline virtual bool update(Matrix &LU, std::vector<T> &x, std::vector<T> &y) final
    {
        std::vector<std::size_t> p;

        return update(LU, x, y, p);
    }

    /**
     * Crout LU factorization update using the Bennett algorithm. This subroutine updates an LU decomposition
     * (equivalent to performing LU-factorization on the quantity A + xy', where x and y are column vectors);
     * the subroutine returns true if the factorization was updated.
     * @param[in]  LU   the factorization to be updated
     * @param[in]  x, y are column vectors used to form the product xy'. The resultant outer product is used to
     *                  update the factorization
     * @param[in]  p    a vector containing the row permutation order
     * @param[out] LU   the updated factorization
     * @param[out] x, y have been destroyed
     * @param[out] p    updated to reflect additional pivoting that may have occurred
     */
    virtual bool update(Matrix &LU, std::vector<T> &x, std::vector<T> &y,
                        const std::vector<std::size_t> &p) final
    {
        auto m = LU.rows();
        auto n = LU.columns();
        bool bPermute = !p.empty();
        for (std::size_t i = 0, in = 0; i < m; ++i, in += n)
        {
            // diagonal update
            auto dii = LU[in + i];
            auto xi = bPermute ? x[p[i]] : x[i];
            auto &LUii = LU[in + i];
            LUii += xi * y[i];
            y[i] /= LUii;

            // L update
            auto jn = in + n;
            for (auto j = i + 1; j < m; ++j, jn += n)
            {
                auto &LUji = LU[jn + i];
                auto &xj = bPermute ? x[p[j]] : x[j];
                auto beta = LUji / dii;
                xj -= xi * beta;
                LUji = LUii * (beta + y[i] * xj);
            }

            // U update
            for (auto j = i + 1; j < n; ++j)
            {
                auto &LUij = LU[in + j];
                auto beta = LUij * dii + xi * y[j];
                LUij = beta / LUii;
                y[j] -= y[i] * beta;
            }
        }

        return true;
    }
};

}

}

}

}

#endif
