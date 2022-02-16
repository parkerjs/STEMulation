#ifndef DOOLITTLE_LU_H
#define DOOLITTLE_LU_H

#include "lu.h"
#include <cmath>

namespace math
{

namespace linear_algebra
{

namespace matrix
{

namespace decomposition
{

/**
 * This class contains methods to compute LU factorizations of m x n matrices using Doolittle's method
 */
template<typename Matrix>
class Doolittle_LU_Factor
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
    Doolittle_LU_Factor(void)
    : LU_Factor<Matrix>(TriangularMatrixType::Enum::Lower)
    {

    }

    /**
     * Copy constructor
     */
    Doolittle_LU_Factor(const Doolittle_LU_Factor<Matrix> &solver)
    : LU_Factor<Matrix>(TriangularMatrixType::Enum::Lower)
    {
        operator = (solver);
    }

    /**
     * Move constructor
     */
    Doolittle_LU_Factor(Doolittle_LU_Factor<Matrix> &&solver)
    : LU_Factor<Matrix>(TriangularMatrixType::Enum::Lower)
    {
        operator = (std::move(solver));
    }

    /**
     * Destructor
     */
    ~Doolittle_LU_Factor(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Doolittle_LU_Factor<Matrix> &operator = (const Doolittle_LU_Factor<Matrix> &solver)
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
    Doolittle_LU_Factor<Matrix> &operator = (Doolittle_LU_Factor<Matrix> &&solver)
    {
        if (&solver != this)
        {
            LU_Factor<Matrix>::operator = (std::move(solver));
        }

        return *this;
    }

    /**
     * Factor the equation Ax = b using Doolittle LU factorization. The factorization has the form PA = LU,
     * where L is lower triangular, U is upper triangular, A is the original matrix, and P is a row permutation
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

            // compute the diagonal and upper triangular portion
            for (auto j = i; j < n; ++j)
            {
                T sum(0);
                for (std::size_t k = 0, kn = 0; kn < in; ++k, kn += n)
                    sum += A[in + k] * A[kn + j];

                A[in + j] -= sum;
            }

            // compute the lower triangular portions
            for (auto jn = in + n; jn < size; jn += n)
            {
                T sum(0);
                for (std::size_t k = 0, kn = 0; kn < in; ++k, kn += n)
                    sum += A[jn + k] * A[kn + i];

                A[jn + i] = (A[jn + i] - sum) / A[in + i];
            }
        }

        return iError;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return std::string("Doolittle_LU_Factor<") + typeid(Matrix).name() + ">";
    }

    /**
     * Get the lower triangle
     */
    inline virtual void getLowerTriangle(const Matrix &LU, Matrix &L) const override final
    {
        L = LU.getLowerTriangle(-1);
        L.setDiagonal(1);
    }

    /**
     * Get the upper triangle
     */
    inline virtual void getUpperTriangle(const Matrix &LU, Matrix &U) const override final
    {
        U = LU.getUpperTriangle();
    }

    /**
     * Doolittle LU factorization update. This subroutine updates an LU decomposition (equivalent to performing
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
     * Doolittle LU factorization update using the Bennett algorithm. This subroutine updates an LU
     * decomposition (equivalent to performing LU-factorization on the quantity A + xy', where x and y are
     * column vectors); the subroutine returns true if the factorization was updated.
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
                xj -= xi * LUji;
                LUji += y[i] * xj;
            }

            // U update
            for (auto j = i + 1; j < n; ++j)
            {
                auto &LUij = LU[in + j];
                LUij += xi * y[j];
                y[j] -= y[i] * LUij;
            }
        }

        return true;
    }

    /**
     * Doolittle LU factorization update using the Schwetlick/Kielbasinski algorithm. This subroutine updates
     * an LU decomposition with pivoting (equivalent to performing LU-factorization on the quantity PA + xy',
     * where P is a permutation matrix, and x and y are column vectors); the subroutine returns true if the
     * factorization was updated.
     * @param[in]  L    the lower triangular factor
     * @param[in]  U    the upper triangular factor
     * @param[in]  x, y are column vectors used to form the product xy'. The resultant outer product is used to
     *                  update the factorization
     * @param[in]  p    a vector containing the row permutation order
     * @param[out] L    the updated lower triangular factor
     * @param[out] U    the updated upper triangular factor
     * @param[out] p    updated with the new permutation order
     * @param[out] x, y have been destroyed
     */
    virtual bool update(Matrix &L, Matrix &U, const std::vector<T> &x, const std::vector<T> &y,
                        std::vector<std::size_t> &p) final
    {
        auto m = L.rows(); // assumed to be square
        auto n = U.columns(); // may be rectangular
        if (L.columns() != U.rows() || x.size() != m || y.size() != n)
            return false;

        // initialize pivot vectors
        this->initialize(PivotType::Enum::Row, m, p);

        Matrix u(x);
        this->solveLower(L, u, p, x, true); // Solve the system L * u = P * x

        auto mn = m * n;
        auto mm = m * m;
        auto mm1 = m - 1;
        const T tau(0.1);
        auto i = mm1, im1 = i;
        auto im1n = mn - n, in = im1n;
        auto im1m = mm - m, im = im1m;
        for (; i > 0; i = im1, in = im1n, im = im1m)
        {
            --im1, im1n -= n, im1m -= m;

            // do u[i] and u[i - 1] need to be interchanged?
            if (std::abs(u[im1]) < tau * std::abs(L[im + im1] * u[im1] + u[i]))
            {
                std::swap(u[i], u[im1]); // swap elements u[i] with u[i - 1]
                this->rowSwap(L, i, im1, im, im1m, 0, i + 1, false); // swap row L(i, :) with L(i - 1, :)
                this->columnSwap(L, i, im1, im1m, mm, false); // swap column L(:, i) with L(:, i - 1)
                this->rowSwap(U, i, im1, in, im1n, im1, n); // swap row U(i, :) with U(i - 1, :)

                // apply inverse transformation to L to remove super-diagonal element
                auto scale = L[im1m + i];
                for (auto km = im1m; km < mm; km += m)
                    L[km + i] -= scale * L[km + im1];

                // apply transformation to U
                for (auto j = im1; j < n; ++j)
                    U[im1n + j] += scale * U[im + j];

                u[im1] += scale * u[i]; // apply transformation to u
            }

            auto scale = -u[i] / u[im1]; // now eliminate the i-th component of u
            u[i] = 0.0;

            // apply inverse transformation to L
            for (auto km = im; km < mm; km += m)
                L[km + im1] -= scale * L[km + i];

            // apply transformation to U
            for (auto j = im1; j < n; ++j)
                U[in + j] += scale * U[im1n + j];
        }

        // now add uy' to U
        for (std::size_t j = 0; j < n; ++j)
            U[j] += u[0] * y[j];

        // now upper triangularize upper-Hessenberg matrix U
        std::size_t ip1 = 0, ip1m = 0, ip1n = 0;
        for (i = in = im = 0; i < mm1; i = ip1, in = ip1n, im = ip1m)
        {
            ++ip1, ip1n += n, ip1m += m;

            // do rows i and i + 1 need to be interchanged?
            if (std::abs(U[in + i]) < tau * std::abs(L[ip1m + i] * U[in + i] + U[ip1n + i]))
            {
                this->rowSwap(L, i, ip1, im, ip1m, 0, ip1 + 1, false); // swap row L(i, :) with L(i + 1, :)
                this->columnSwap(L, i, ip1, im, mm, false); // swap column L(:, i) with L(:, i + 1)
                this->rowSwap(U, i, ip1, in, ip1n, i, n); // swap row U(i, :) with U(i + 1, :)

                // apply inverse transformation to L to remove super-diagonal element
                auto scale = L[im + ip1];
                for (auto km = im; km < mm; km += m)
                    L[km + ip1] -= scale * L[km + i];

                // apply transformation to U
                for (auto j = i; j < n; ++j)
                    U[im + j] += scale * U[ip1n + j];
            }

            // apply transformation to U to remove sub-diagonal element
            const auto scale = -U[ip1n + i] / U[in + i];
            U[ip1n + i] = 0.0;
            for (auto j = ip1; j < n; ++j)
                U[ip1n + j] += scale * U[in + j];

            // apply inverse transformation to L
            for (auto km = ip1m; km < mm; km += m)
                L[km + i] -= scale * L[km + ip1];
        }

        std::copy(this->m_p.begin(), this->m_p.end(), p.begin());

        return true;
    }
};

}

}

}

}

#endif
