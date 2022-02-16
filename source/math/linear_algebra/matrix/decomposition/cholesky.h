#ifndef CHOLESKY_H
#define CHOLESKY_H

#include "linear_solver.h"
#include <cmath>
#include <iostream>
#include <memory>
#include <numeric>

namespace math
{

namespace linear_algebra
{

namespace matrix
{

namespace decomposition
{

/**
 * This class contains methods to compute the factorization of positive symmetric definite n x n matrices
 */
template<typename Matrix>
class Cholesky_Factor
: protected LinearSolver<Matrix>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Typedef declarations
     */
    typedef typename LinearSolver<Matrix>::T T;

    /**
     * Constructor
     */
    Cholesky_Factor(void)
    {

    }

    /**
     * Copy constructor
     */
    Cholesky_Factor(const Cholesky_Factor<Matrix> &solver)
    {
        operator = (solver);
    }

    /**
     * Move constructor
     */
    Cholesky_Factor(Cholesky_Factor<Matrix> &&solver)
    {
        operator = (std::move(solver));
    }

    /**
     * Destructor
     */
    virtual ~Cholesky_Factor(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Cholesky_Factor<Matrix> &operator = (const Cholesky_Factor<Matrix> &solver)
    {
        if (&solver != this)
        {
            LinearSolver<Matrix>::operator = (solver);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Cholesky_Factor<Matrix> &operator = (Cholesky_Factor<Matrix> &&solver)
    {
        if (&solver != this)
        {
            LinearSolver<Matrix>::operator = (std::move(solver));
        }

        return *this;
    }

    /**
     * Compute the determinant of a positive symmetric definite matrix using Cholesky factorization
     * @param[in]  A   the positive symmetric definite matrix to be factored
     * @param[out] A   the resultant Cholesky factorization
     * @param[out] det if successful, contains the determinant
     * @return         an integer value according to the following:
     *                 >  0 if matrix is not positive semidefinite
     *                 =  0 success
     *                 = -1 if matrix isn't square
     */
    virtual int determinant(Matrix &A, T &det) override final
    {
        auto iError = factor(A);
        if (iError == 0)
        {
            det = 1.0; // initialize determinant
            auto np1 = A.columns() + 1;
            auto size = A.size();
            for (std::size_t inp1 = 0; inp1 < size; inp1 += np1)
                det *= A[inp1];
        }

        return iError;
    }

    /**
     * Cholesky factorization down-date. The following subroutine is a translation of the linpack procedure
     * dchdd, by G. W. Stewart, University of Maryland, Argonne National Lab. This subroutine down-dates a
     * Cholesky decomposition or the triangular factor of a qr decomposition.
     * @param[in]  L the lower triangular matrix that is to be down-dated. The part of L above the diagonal is
     *               not referenced.
     * @param      x the row vector that is to be removed from L (x is left unaltered).
     * @param[out] L the down-dated quantities.
     * @return       an integer value according to the following:
     *               =  0 if the entire down-dating was successful.
     *               = -1 if L could not be down-dated. In this case, all quantities are left unaltered.
     */
    inline virtual int downdate(Matrix &L, const std::vector<T> &x) final
    {
        auto p = L.rows();
        std::vector<T> c(p), s(p);

        return downdate(L, x, c, s);
    }

    /**
     * Cholesky factorization down-date. The following subroutine is a translation of the linpack procedure
     * dchdd, by G. W. Stewart, University of Maryland, Argonne National Lab. This subroutine down-dates a
     * Cholesky decomposition or the triangular factor of a qr decomposition.
     * @param[in]  L the lower triangular matrix that is to be down-dated. The part of L above the diagonal is
     *               not referenced.
     * @param      x the row vector that is to be removed from L (x is left unaltered).
     * @param[out] L the down-dated quantities.
     * @param[out] c the cosines of the transforming rotations.
     * @param[out] s the sines of the transforming rotations.
     * @return       an integer value according to the following:
     *               =  0 if the entire down-dating was successful.
     *               = -1 if L could not be down-dated. In this case, all quantities are left unaltered.
     */
    int virtual downdate(Matrix &L, const std::vector<T> &x, std::vector<T> &c, std::vector<T> &s) final
    {
        int iError = 0;
        auto p = L.rows();
        auto q = L.columns();
        if (p != q)
            iError = -1;
        else
        {
            // solve the system R' * a = V, placing the result in the array s.
            s[0] = x[0] / L[0];
            for (std::size_t j = 1, jp = p; j < p; ++j, jp += p)
            {
                s[j] = x[j];
                for (std::size_t i = 0, ip = 0; i < j; ++i, ip += p)
                    s[j] -= L[jp + i] * s[i];

                s[j] /= L[jp + j];
            }

            auto norm = sqrt(std::inner_product(s.cbegin(), s.cend(), s.cbegin(), 0.0));
            if (norm >= 1.0)
                iError = -1;
            else
            {
                // determine the transformations.
                auto alpha = sqrt(1.0 - norm * norm);
                for (int i = p - 1; i >= 0; --i)
                {
                    auto scale = alpha + std::abs(s[i]);
                    auto a = alpha / scale;
                    auto b = s[i] / scale;
                    norm = sqrt(a * a + b * b);
                    c[i] = a / norm;
                    s[i] = b / norm;
                    alpha = scale * norm;
                }

                // apply the transformations to L.
                for (std::size_t j = 0, jp = 0; j < p; ++j, jp += p)
                {
                    T xx(0.0);
                    for (int i = j, ip = jp; i >= 0; --i, ip -= p)
                    {
                        auto &rij = L[jp + i];
                        auto t = c[i] * xx + s[i] * rij;
                        rij = c[i] * rij - s[i] * xx;
                        xx = t;
                    }
                }
            }
        }

        return iError;
    }

    /**
     * Cholesky factorization down-date. The following subroutine is a translation of the linpack procedure
     * dchdd, by G. W. Stewart, University of Maryland, Argonne National Lab. This subroutine down-dates an
     * augmented Cholesky decomposition or the triangular factor of an augmented qr decomposition.
     * Specifically, given an lower triangular matrix L of order p, a row vector x, a column vector z, and a
     * scalar y, this function determines a unitary matrix u and a scalar zeta such that
       \verbatim
                         ( L   z  )     ( rr   zz )
                     u * (        )  =  (         ) ,
                         ( 0 zeta )     (  x    y )
       \endverbatim
     *
     * where rr is lower triangular. If L and z have been obtained from the factorization of a least squares
     * problem, then rr and zz are the factors corresponding to the problem with the observation (x, y) the
     * down-dated problem is sqrt(rho^2 - zeta^2). This subroutine will simultaneously down-date several
     * triplets (z, y, rho) along with L. The matrix u is determined as the product u(1) * ... * u(p) where
     * u(i) is a rotation in the (p + 1, i)-plane of the form
       \verbatim
                         ( c(i)    -s(i) )
                         (               )
                         ( s(i)     c(i) )
       \endverbatim
     *
     * The rotations are chosen so that c(i) is double precision. The user is warned that a given down-dating
     * problem may be impossible to accomplish or may produce inaccurate results. For example, this can happen
     * if x is near a vector whose removal will reduce the rank of L.
     * @param[in]  L         the lower triangular matrix that is to be down-dated. The part of L above the
     *                       diagonal is not referenced.
     * @param      x         the row vector that is to be removed from L (x is left unaltered).
     * @param[in]  z         an array of non-zero p-vectors which are to be down-dated along with L.
     * @param      y         the scalars for the down-dating of the vectors z (y is left unaltered).
     * @param[in]  rho       the norms of the residual vectors that are to be down-dated.
     * @param[out] L, z, rho contain the down-dated quantities.
     * @param[out] c         the cosines of the transforming rotations.
     * @param[out] s         the sines of the transforming rotations.
     * @return               an integer value according to the following:
     *                       =  1 if some rho could not be down-dated. The offending rhos are set to -1.
     *                       =  0 if the entire down-dating was successful.
     *                       = -1 if L could not be down-dated. In this case, all quantities are left
     *                       unaltered.
     */
    virtual int downdate(Matrix &L, const std::vector<T> &x, std::vector<T> &c,
                         std::vector<T> &s, Matrix &z, const std::vector<T> &y,
                         std::vector<T> &rho) final
    {
        // down-date matrix L
        auto iError = downdate(L, x, c, s);
        if (iError == 0)
        {
            // down-date z and rho.
            auto p = L.rows();
            auto q = z.columns();
            for (std::size_t j = 0; j < y.size(); ++j)
            {
                auto zeta = y[j];
                for (std::size_t i = 0, iq = 0; i < p; ++i, iq += q)
                {
                    auto &zij = z[iq + j];
                    zij = (zij - s[i] * zeta) / c[i];
                    zeta = c[i] * zeta - s[i] * zij;
                }

                auto azeta = std::abs(zeta);
                if (azeta > rho[j])
                {
                    iError = 1;
                    rho[j] = -1.0;
                }
                else
                    rho[j] *= sqrt(1.0 - azeta * azeta / rho[j] / rho[j]);
            }
        }

        return iError;
    }

    /**
     * Factor and solve the equation Ax = b using Cholesky decomposition, where X and B may be the same calling
     * argument. The factorization has the form A = LL', where L is lower triangular. The factorization is
     * performed in place, with A overwritten by the resultant factorization (L is 0 above the main diagonal)
     * @param[in]  A the positive symmetric definite matrix to be factored
     * @param      B a vector or matrix of coefficients corresponding to the equation LL'X = B
     * @param[out] A the matrix square root in lower triangular form
     * @param[out] X the solution produced by the solve() function corresponding to the equation LL'X = B
     * @return       an integer value according to the following:
     *               >  0 if matrix is not positive semidefinite
     *               =  0 success
     *               = -1 if matrix isn't square
     *               = -3 if the dimensions of the matrices are incompatible
     */
    inline virtual int factor(Matrix &A, Matrix &X, const Matrix &B) final
    {
        auto iError = factor(A);
        if (iError == 0)
            iError = solveLower(A, X, B);

        return iError;
    }

    /**
     * Factor the equation Ax = b using Cholesky decomposition. The factorization has the form A = LL', where L
     * is lower triangular. The factorization is performed in place, with A overwritten by the resultant
     * factorization (L is 0 above the main diagonal)
     * @param[in]  A the positive symmetric definite matrix to be factored
     * @param[out] A the matrix square root in lower triangular form
     * @return       an integer value according to the following:
     *               >  0 if matrix is not positive semidefinite
     *               =  0 success
     *               = -1 if matrix isn't square
     */
    int virtual factor(Matrix &A) final
    {
        int iError = 0;
        if (!A.isSquare())
            iError = -1;
        else
        {
            auto n = A.columns();
            for (std::size_t i = 0, in = 0; i < n; ++i, in += n)
            {
                T sum(0.0);
                for (std::size_t j = 0; j < i; ++j)
                    sum += A[in + j] * A[in + j];

                auto &Aii = A[in + i];
                if (Aii <= 0)
                {
                    iError = i + 1;

                    break;
                }
                else
                {
                    Aii = sqrt(Aii - sum);
                    auto jn = in + n;
                    for (auto j = i + 1; j < n; ++j, jn += n)
                    {
                        sum = 0.0;
                        for (std::size_t k = 0; k < i; ++k)
                            sum += A[jn + k] * A[in + k];

                        A[jn + i] = (A[jn + i] - sum) / Aii;
                        A[in + j] = 0.0;
                    }
                }
            }
        }

        return iError;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return std::string("Cholesky_Factor<") + typeid(Matrix).name() + ">";
    }

    /**
     * Compute the inverse of a positive symmetric definite matrix
     * @param[in]  A    the positive symmetric definite matrix to be inverted
     * @param[in]  Ainv must contain the identity matrix
     * @param[out] A    the matrix square root in lower triangular form
     * @param[out] Ainv the inverse of the original matrix
     * @return          an integer value according to the following:
     *                  >  0 if matrix is not positive semidefinite
     *                  =  0 success
     *                  = -1 if matrix isn't square
     */
    inline virtual int inverse(Matrix &A, Matrix &Ainv) override final
    {
        int iError = 0;
        if (!A.isSquare())
            iError = -1;
        else
            iError = factor(A, Ainv, Ainv);

        return iError;
    }

    /**
     * Solve the linear system LL'X = B, where X and B may be the same calling argument
     * @param      A the positive symmetric definite matrix to be factored
     * @param      B a matrix of coefficients corresponding to the equation LL'X = B
     * @param[in]  X storage to hold the solution to LL'X = B
     * @param[out] X the solution to the equation LL'X = B
     * @return       an integer value according to the following:
     *               >  0 if matrix is not positive semidefinite
     *               =  0 success
     *               = -1 if matrix isn't square
     *               = -3 if the dimensions of the matrices are incompatible
     */
    inline virtual int solve(const Matrix &A, Matrix &X, const Matrix &B) override final
    {
        Matrix L(A);
        Matrix Y(B);

        return factor(L, X, Y);
    }

    /**
     * Solve the linear system LL'x = b, where x and b are vector objects
     * @param      A the positive symmetric definite matrix to be factored
     * @param      b a vector of coefficients corresponding to the equation LL'x = b
     * @param[in]  x storage to hold the solution to LL'x = b
     * @param[out] x the solution to the equation LL'x = b
     * @return       an integer value according to the following:
     *               >  0 if matrix is not positive semidefinite
     *               =  0 success
     *               = -1 if matrix isn't square
     *               = -3 if the dimensions of the matrices are incompatible
     */
    virtual int solve(const Matrix &A, std::vector<T> &x, const std::vector<T> &b) override final
    {
        Matrix L(A);
        Matrix v(x);
        Matrix y(b);

        auto iError = factor(L, v, y);
        if (iError >= 0)
        {
            auto size = v.size();
            for (std::size_t i = 0; i < size; ++i)
                x[i] = v[i];
        }

        return iError;
    }

    /**
     * Solve the linear system LL'X = B, where X and B may be the same calling argument
     * @param      L the lower triangular matrix produced by the factor() function.
     * @param      B a vector or matrix of coefficients corresponding to the equation LL'X = B
     * @param[out] X the solution produced by the solve() function corresponding to the equation LL'X = B
     * @return       an integer value according to the following:
     *               =  0 success
     *               = -1 if matrix isn't square
     *               = -3 if the dimensions of the matrices are incompatible
     */
    virtual int solveLower(const Matrix &L, Matrix &X, const Matrix &B) final
    {
        int iError = 0;
        if (!this->isCompatible(L, X, B))
            iError = -3;
        else if (!L.isSquare())
            iError = -1;
        else
        {
            // solve L * Y = B
            auto m = L.rows();
            auto n = L.columns();
            auto p = X.columns();
            auto size = m * n;
            std::size_t in = 0, ip = 0;
            for (std::size_t i = 0; i < n; ++i, in += n, ip += p)
            {
                for (std::size_t j = 0; j < p; ++j)
                {
                    T sum(0.0);
                    for (std::size_t k = 0, kp = 0; k < i; ++k, kp += p)
                        sum += L[in + k] * X[kp + j];

                    X[ip + j] = (B[ip + j] - sum) / L[in + i];
                }
            }

            // now solve L * X = Y
            for (int i = n - 1; i >= 0; --i)
            {
                in -= n, ip -= p;
                for (std::size_t j = 0; j < p; ++j)
                {
                    T sum(0.0);
                    auto kp = ip + p;
                    for (auto kn = in + n; kn < size; kn += n, kp += p)
                        sum += L[kn + i] * X[kp + j];

                    X[ip + j] = (X[ip + j] - sum) / L[in + i];
                }
            }
        }

        return iError;
    }

    /**
     * Cholesky factorization update. The following subroutine is a translation of the linpack procedure dchud,
     * by G. W. Stewart, University of Maryland, Argonne National Lab. This subroutine updates a Cholesky
     * decomposition or the triangular factor of a qr decomposition.
     * @param[in]  L the lower triangular matrix that is to be updated. The part of L above the diagonal is not
     *               referenced.
     * @param      x the row vector that is to be added to L (x is left unaltered).
     * @param[out] L the updated quantities.
     */
    inline virtual void update(Matrix &L, const std::vector<T> &x) final
    {
        auto p = L.rows();
        std::vector<T> c(p), s(p);

        update(L, x, c, s);
    }

    /**
     * Cholesky factorization update. The following subroutine is a translation of the linpack procedure dchud,
     * by G. W. Stewart, University of Maryland, Argonne National Lab. This subroutine updates a Cholesky
     * decomposition or the triangular factor of a qr decomposition.
     * @param[in]  L the lower triangular matrix that is to be updated. The part of L above the diagonal is not
     *               referenced.
     * @param      x the row vector that is to be added to L (x is left unaltered).
     * @param[out] L the updated quantities.
     * @param[out] c the cosines of the transforming rotations.
     * @param[out] s the sines of the transforming rotations.
     */
    virtual void update(Matrix &L, const std::vector<T> &x, std::vector<T> &c, std::vector<T> &s) final
    {
        // update L
        auto p = L.rows();
        auto &&signum = [] (auto &&x) { return x < 0.0 ? -1.0 : 1.0; };
        for (std::size_t j = 0, jp = 0; j < p; ++j, jp += p)
        {
            // apply the previous rotations
            auto xj = x[j];
            for (std::size_t i = 0, ip = 0; i < j; ++i, ip += p)
            {
                auto &rij = L[jp + i];
                auto t = c[i] * rij + s[i] * xj;
                xj = c[i] * xj - s[i] * rij;
                rij = t;
            }

            // compute the next rotation
            auto &rjj = L[jp + j];
            if (xj == 0)
            {
                c[j] = signum(rjj);
                s[j] = 0;
                rjj = std::abs(rjj);
            }
            else if (rjj == 0)
            {
                c[j] = 0;
                s[j] = signum(xj);
                rjj = std::abs(xj);
            }
            else if (std::abs(xj) > std::abs(rjj))
            {
                auto t = rjj / xj;
                auto u = signum(xj) * sqrt(1 + t * t);
                s[j] = 1 / u;
                c[j] = s[j] * t;
                rjj = xj * u;
            }
            else
            {
                auto t = xj / rjj;
                auto u = signum(rjj) * sqrt(1 + t * t);
                c[j] = 1 / u;
                s[j] = c[j] * t;
                rjj = rjj * u;
            }
        }
    }

    /**
     * Cholesky factorization update. The following subroutine is a translation of the linpack procedure dchud,
     * by G. W. Stewart, University of Maryland, Argonne National Lab. This subroutine updates a Cholesky
     * decomposition or the triangular factor of a qr decomposition. Specifically, given an lower triangular
     * matrix L of order p, a row vector x, a column vector z, and a scalar y, this function determines a
     * unitary matrix u and a scalar zeta such that
       \verbatim
                        (L  z)     (rr   zz )
                   u  * (    )  =  (        ) ,
                        (x  y)     ( 0  zeta)
       \endverbatim
     *
     * where rr is lower triangular. If L and z have been obtained from the factorization of a least squares
     * problem, then rr and zz are the factors corresponding to the problem with the observation (x, y)
     * removed. In this case, if rho is the norm of the residual vector, then the norm of the residual vector
     * of the updated problem is sqrt(rho^2 + zeta^2). This subroutine will simultaneously update several
     * triplets (z, y, rho). The matrix u is determined as the product u(p) * ... * u(1) where u(i) is a
     * rotation in the (i, p + 1) plane of the form
       \verbatim
                        (  c(i)    s(i) )
                        (               )
                        ( -s(i)    c(i) )
       \endverbatim
     *
     * The rotations are chosen so that c(i) is double precision.
     * @param[in]  L         the lower triangular matrix that is to be updated. The part of L above the
     *                       diagonal is not referenced.
     * @param      x         the row vector that is to be added to L (x is left unaltered).
     * @param[in]  z         an array containing non-zero p-vectors to be updated with L.
     * @param      y         the scalars for updating the z vectors (y is left unaltered).
     * @param[in]  rho       the norms of the residual vectors that are to be updated. If rho(j) is negative,
     *                       it is left unaltered.
     * @param[out] L, z, rho contain the down-dated quantities.
     * @param[out] c         the cosines of the transforming rotations.
     * @param[out] s         the sines of the transforming rotations.
     */
    virtual void update(Matrix &L, const std::vector<T> &x, std::vector<T> &c, std::vector<T> &s,
                        Matrix &z, const std::vector<T> &y, std::vector<T> &rho) final
    {
        // update matrix L
        update(L, x, c, s);

        // update z and rho
        auto p = L.rows();
        auto q = z.columns();
        for (std::size_t j = 0; j < y.size(); ++j)
        {
            auto zeta = y[j];
            for (std::size_t i = 0, iq = 0; i < p; ++i, iq += q)
            {
                auto &zij = z[iq + j];
                auto t = c[i] * zij + s[i] * zeta;
                zeta = c[i] * zeta - s[i] * zij;
                zij = t;
            }

            auto azeta = std::abs(zeta);
            if (azeta != 0.0 && rho[j] >= 0.0)
            {
                auto scale = azeta + rho[j];
                rho[j] = (azeta * azeta + rho[j] * rho[j]) / (scale * scale);
                rho[j] = scale * sqrt(rho[j]);
            }
        }
    }
};

}

}

}

}

#endif
