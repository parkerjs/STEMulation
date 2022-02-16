#ifndef TRIDIAG_LU_H
#define TRIDIAG_LU_H

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
 * This class contains methods to compute LU factorizations of m x n tridiagonal matrices
 */
template<typename Matrix>
class Tridiag_LU_Factor
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
    Tridiag_LU_Factor(void)
    : LU_Factor<Matrix>(TriangularMatrixType::Enum::Upper)
    {

    }

    /**
     * Copy constructor
     */
    Tridiag_LU_Factor(const Tridiag_LU_Factor &solver)
    : LU_Factor<Matrix>(TriangularMatrixType::Enum::Upper)
    {
        operator = (solver);
    }

    /**
     * Move constructor
     */
    Tridiag_LU_Factor(Tridiag_LU_Factor &&solver)
    : LU_Factor<Matrix>(TriangularMatrixType::Enum::Upper)
    {
        operator = (std::move(solver));
    }

    /**
     * Destructor
     */
    ~Tridiag_LU_Factor(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Tridiag_LU_Factor<Matrix> &operator = (const Tridiag_LU_Factor<Matrix> &solver)
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
    Tridiag_LU_Factor<Matrix> &operator = (Tridiag_LU_Factor<Matrix> &&solver)
    {
        if (&solver != this)
        {
            LU_Factor<Matrix>::operator = (std::move(solver));
        }

        return *this;
    }

    /**
     * Factor the equation Ax = b using Tridiagonal LU factorization without pivoting. The factorization has
     * the form A = LU, where L is lower triangular, U is unit upper triangular, and A is the original
     * tridiagonal matrix. The resultant factorization is returned in a single matrix.
     * @param[in]  A the tridiagonal matrix to be factored
     * @param[out] A the resultant LU factorization
     * @return       an integer value according to the following:
     *               =  0 success
     *               = -1 if matrix isn't square
     *               = -2 if matrix is singular
     */
    virtual int factor(Matrix &A) override final
    {
        int iError = 0;
        if (!A.isSquare())
            iError = -1;
        else
        {
            auto m = A.rows();
            auto n = A.columns();
            auto np1 = n + 1;
            for (std::size_t i = 0, inp1 = 0; i < m; ++i, inp1 += np1)
            {
                if (i > 0)
                    A[inp1] -= A[inp1 - 1] * A[inp1 - n];

                if (A[inp1] == 0.0)
                {
                    iError = -2;
                    break;
                }

                if (i + 1 < n)
                    A[inp1 + 1] /= A[inp1];
            }
        }

        return iError;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return std::string("Tridiag_LU_Factor<") + typeid(Matrix).name() + ">";
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
     * Identify whether or not the LU product produces a singular matrix
     */
    inline virtual bool isProductNonsingular(const Matrix &LU) const override final
    {
        auto np1 = LU.columns() + 1;
        auto size = LU.size();
        for (std::size_t inp1 = 0; inp1 < size; inp1 += np1)
            if (LU[inp1] == 0.0)
                return false;

        return true;
    }

    /**
     * Make LU factorization lower-unit upper triangular
     * @param[in]  LU the LU factorization in the form L(DU)
     * @param[out] LU the LU factorization in the form (LD)U
     */
    virtual void makeLowerUnitUpper(Matrix &LU) const override final
    {
        // convert unit lower triangle to lower
        auto m = LU.rows();
        auto n = LU.columns();
        for (std::size_t i = 0, in = 0; i + 1 < m; ++i)
        {
            auto Dii = LU[in + i];
            in += n;
            LU[in + i] *= Dii;
        }

        // convert upper triangle to unit upper
        auto mnm1 = LU.size() - 1;
        for (std::size_t jnp1 = 0; jnp1 < mnm1; jnp1 += n + 1)
            LU[jnp1 + 1] /= LU[jnp1];
    }

    /**
     * Make LU factorization unit-lower-upper triangular
     * @param[in]  LU the LU factorization in the form (LD)U
     * @param[out] LU the LU factorization in the form L(DU)
     */
    virtual void makeUnitLowerUpper(Matrix &LU) const override final
    {
        // convert lower triangle to unit lower
        auto m = LU.rows();
        auto n = LU.columns();
        for (std::size_t i = 0, in = 0; i + 1 < m; ++i)
        {
            auto Dii = LU[in + i];
            in += n;
            LU[in + i] /= Dii;
        }

        // convert unit upper triangle to upper
        auto mnm1 = LU.size() - 1;
        for (std::size_t jnp1 = 0; jnp1 < mnm1; jnp1 += n + 1)
            LU[jnp1 + 1] *= LU[jnp1];
    }

protected:

    /**
     * Solve the lower triangular system L * Z = B, where Y and B may be the same calling argument
     * @param      L     a lower triangular matrix
     * @param      B     a matrix of coefficients corresponding to the equation L * Z = B
     * @param      bUnit flag indicating whether or not the input matrix is unit lower triangular
     * @param[out] Z     the solution to the lower triangular system
     */
    virtual void solveLower(const Matrix &L, Matrix &Z, const Matrix &B,
                            bool bUnit) const override final
    {
        auto q = Z.columns();
        for (std::size_t j = 0; j < q; ++j)
            Z[j] = B[j] / L[0];

        auto j = q;
        auto np1 = L.columns() + 1;
        auto size = L.size();
        for (auto inp1 = np1; inp1 < size; inp1 += np1)
        {
            do
            {
                Z[j] = (B[j] - L[inp1 - 1] * Z[j - q]);
                if (!bUnit)
                    Z[j] /= L[inp1];
            }
            while ((++j % q) != 0);
        }
    }

    /**
     * Solve the upper triangular system U * Y = Z, where Y and Z may be the same calling argument
     * @param      U     an upper triangular matrix
     * @param      Z     a matrix of coefficients corresponding to the equation U * Y = Z
     * @param      bUnit flag indicating whether or not the input matrix is unit upper triangular
     * @param[out] Y     the solution to the upper triangular system
     */
    virtual void solveUpper(const Matrix &U, Matrix &Y, const Matrix &Z,
                            bool bUnit) const override final
    {
        auto mp = Z.size();
        auto p = Z.columns();
        for (std::size_t j = 1; j <= p; ++j)
        {
            auto mpmj = mp - j;
            Y[mpmj] = Z[mpmj];
            if (!bUnit)
                Y[mpmj] /= Y[mp - 1];
        }

        auto m = U.rows();
        auto n = U.columns();
        auto im1p = mp - p;
        auto im1n = (m - 1) * n;
        auto ip = im1p;
        for (auto i = m - 1; i > 0; --i, ip -= p)
        {
            im1n -= n, im1p -= p;
            auto Uii = U[im1n + i - 1];
            for (std::size_t j = 0; j < p; ++j)
            {
                Y[im1p + j] = Z[im1p + j] - U[im1n + i] * Y[ip + j];
                if (!bUnit)
                    Y[im1p + j] /= Uii;
            }
        }
    }
};

}

}

}

}

#endif
