#ifndef LU_H
#define LU_H

#include "linear_solver.h"
#include "triangular_matrix_type.h"

namespace math
{

namespace linear_algebra
{

namespace matrix
{

namespace decomposition
{

/**
 * This class serves as an abstract base for subclasses that implement methods to compute LU factorizations of
 * m x n matrices
 */
template<typename Matrix>
class LU_Factor
: public LinearSolver<Matrix>,
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
    LU_Factor(const TriangularMatrixType &triangularMatrixType)
    : m_triangularMatrixType(triangularMatrixType)
    {

    }

    /**
     * Copy constructor
     */
    LU_Factor(const LU_Factor<Matrix> &solver)
    : m_triangularMatrixType(solver.m_triangularMatrixType)
    {
        operator = (solver);
    }

    /**
     * Move constructor
     */
    LU_Factor(LU_Factor<Matrix> &&solver)
    : m_triangularMatrixType(std::move(solver.m_triangularMatrixType))
    {
        operator = (std::move(solver));
    }

    /**
     * Destructor
     */
    virtual ~LU_Factor(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    LU_Factor<Matrix> &operator = (const LU_Factor<Matrix> &solver)
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
    LU_Factor<Matrix> &operator = (LU_Factor<Matrix> &&solver)
    {
        if (&solver != this)
        {
            LinearSolver<Matrix>::operator = (std::move(solver));
        }

        return *this;
    }

    /**
     * Compute the determinant of a matrix using LU factorization
     * @param[in]  A   the matrix to be factored
     * @param[in]  det storage to hold the determinant
     * @param[out] A   the resultant LU factorization
     * @param[out] det if successful, contains the determinant
     * @return         an integer value according to the following:
     *                 =  1 success, row pivoting was performed
     *                 =  0 success, pivoting was not performed
     *                 = -1 if matrix isn't square
     */
    virtual int determinant(Matrix &A, T &det) override final
    {
        int iError = 0;
        if (!A.isSquare())
            iError = -1;

        if (iError >= 0)
        {
            iError = factor(A);
            if (iError >= 0)
            {
                det = 1.0; // initialize determinant

                auto np1 = A.columns() + 1;
                auto size = A.size();
                for (std::size_t inp1 = 0; inp1 < size; inp1 += np1)
                    det *= A[inp1];

                if ((this->m_numColSwaps + this->m_numRowSwaps) % 2)
                    det = -det;
            }
        }

        return iError;
    }

    /**
     * Factor and solve the equation Ax = b using LU decomposition. The factorization has the form A = LU,
     * where L is lower triangular, U is upper triangular, and A is the original matrix. The resultant
     * factorization is returned in a single matrix.
     * @param[in]  A the matrix to be factored
     * @param[in]  B a matrix of coefficients corresponding to the equation LUX = B
     * @param[in]  X storage to hold the solution to LUX = B
     * @param[out] A the resultant LU factorization
     * @param[out] B possibly permuted by P and Q
     * @param[out] X the solution to the equation LUX = B
     * @return       an integer value according to the following:
     *               =  1 success, row pivoting was performed
     *               =  0 success, pivoting was not performed
     *               = -2 if matrix is singular
     *               = -3 if the dimensions of the matrices are incompatible
     */
    virtual int factor(Matrix &A, Matrix &X, Matrix &B) final
    {
        int iError = 0;
        if (!this->isCompatible(A, X, B))
            iError = -3;
        else
        {
            iError = factor(A);
            if (!isProductNonsingular(A))
                iError = -2;

            if (iError >= 0)
            {
                if (iError == 1 || iError == 3) // row pivoting was performed
                    B.permuteRows(this->m_p, this->m_w);

                // solve L * Z = P * B
                solveLower(A, X, B, m_triangularMatrixType == TriangularMatrixType::Enum::Lower);

                // solve U * Y = Z
                this->solveUpper(A, X, X, m_triangularMatrixType == TriangularMatrixType::Enum::Upper);
            }
        }

        return iError;
    }

    /**
     * Factor the equation Ax = b using LU decomposition. The factorization has the form A = LU, where L is
     * lower triangular, U is upper triangular, and A is the original matrix. The resultant factorization is
     * returned in a single matrix.
     * @param[in]  A the matrix to be factored
     * @param[out] A the resultant LU factorization
     * @return       an integer value according to the following:
     *               = 1 success, row pivoting was performed
     *               = 0 success, pivoting was not performed
     */
    virtual int factor(Matrix &A) = 0;

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return std::string("LU_Factor<") + typeid(Matrix).name() + ">";
    }

    /**
     * Get the lower triangle
     */
    inline virtual Matrix getLowerTriangle(const Matrix &LU) const final
    {
        Matrix L;

        getLowerTriangle(LU, L);

        return L;
    }

    /**
     * Get the lower triangle
     */
    virtual void getLowerTriangle(const Matrix &LU, Matrix &L) const = 0;

    /**
     * Get the upper triangle
     */
    inline virtual Matrix getUpperTriangle(const Matrix &LU) const final
    {
        Matrix U;

        getUpperTriangle(LU, U);

        return U;
    }

    /**
     * Get the upper triangle
     */
    virtual void getUpperTriangle(const Matrix &LU, Matrix &U) const = 0;

    /**
     * Compute the inverse of a matrix using LU factorization
     * @param[in]  A    the matrix to be inverted
     * @param[in]  Ainv must contain the identity matrix
     * @param[out] A    the resultant LU factorization
     * @param[out] Ainv the inverse of the original matrix
     * @return          an integer value according to the following:
     *                  =  1 success, row pivoting was performed
     *                  =  0 success
     *                  = -1 if matrix isn't square
     *                  = -2 if matrix is singular
     */
    virtual int inverse(Matrix &A, Matrix &Ainv) override final
    {
        int iError = 0;
        if (!A.isSquare())
            iError = -1;
        else
        {
            iError = factor(A);
            if (!isProductNonsingular(A))
                iError = -2;

            if (iError >= 0)
            {
                if (iError == 1 || iError == 3) // row pivoting was performed
                    this->getPermutationMatrix(PivotType::Enum::Row, Ainv);

                // solve L * Z = P * I
                solveLower(A, Ainv, Ainv, m_triangularMatrixType == TriangularMatrixType::Enum::Lower);

                // now solve U * Y = Z
                this->solveUpper(A, Ainv, Ainv, m_triangularMatrixType == TriangularMatrixType::Enum::Upper);
            }
        }

        return iError;
    }

    /**
     * Identify whether or not the LU product produces a singular matrix
     */
    virtual inline bool isProductNonsingular(const Matrix &LU) const
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
    virtual void makeLowerUnitUpper(Matrix &LU) const
    {
        // convert unit lower triangle to lower
        auto n = LU.columns();
        auto size = LU.size();
        for (std::size_t j = 0, jn = 0; j < n; ++j, jn += n)
            for (auto in = jn + n; in < size; in += n)
                LU[in + j] *= LU[jn + j];

        // convert upper triangle to unit upper
        auto m = LU.rows();
        for (std::size_t i = 0, in = 0; i < m; ++i, in += n)
            for (auto j = i + 1; j < n; ++j)
                LU[in + j] /= LU[in + i];
    }

    /**
     * Make LU factorization unit-lower-upper triangular
     * @param[in]  LU the LU factorization in the form (LD)U
     * @param[out] LU the LU factorization in the form L(DU)
     */
    virtual void makeUnitLowerUpper(Matrix &LU) const
    {
        // convert lower triangle to unit lower
        auto n = LU.columns();
        auto size = LU.size();
        for (std::size_t j = 0, jn = 0; j < n; ++j, jn += n)
            for (auto in = jn + n; in < size; in += n)
                LU[in + j] /= LU[jn + j];

        // convert unit upper triangle to upper
        auto m = LU.rows();
        for (std::size_t i = 0, in = 0; i < m; ++i, in += n)
            for (auto j = i + 1; j < n; ++j)
                LU[in + j] *= LU[in + i];
    }

    /**
     * Perform row pivoting
     * @param[in]  PA     the matrix upon which pivoting will be performed
     * @param      i      the index to the current row
     * @param      in     the product of the row index and the number of columns in PA
     * @param[in]  iError an error code that is modified as result of any pivoting that is performed
     * @param[out] PA     possibly further permuted by row pivoting
     * @param[out] iError an integer value that indicates what kind of pivoting, if any, has occurred:
     *                    = 1 row pivoting was performed
     *                    = 0 pivoting was not performed
     */
    virtual bool pivot(Matrix &PA, std::size_t i, std::size_t in, int &iError) override final
    {
        // find the pivot in ith column; only attempt row pivoting if there are at least two rows remaining...
        auto m = PA.rows();
        auto ip1 = i + 1;
        if (ip1 < m && this->rowPivot(PA, i, in))
            iError |= 1;

        return true;
    }

    /**
     * Solve the linear system LUX = B using LU factorization; note: X and B may be the same calling argument
     * @param      A the matrix to be factored
     * @param      B a matrix of coefficients corresponding to the equation LUX = B
     * @param[in]  X storage to hold the solution to LUX = B
     * @param[out] X the solution to the equation LUX = B
     * @return       an integer value according to the following:
     *               =  1 success, row pivoting was performed
     *               =  0 success, pivoting was not performed
     *               = -2 if matrix is singular
     *               = -3 if the dimensions of the matrices are incompatible
     */
    inline virtual int solve(const Matrix &A, Matrix &X, const Matrix &B) override final
    {
        Matrix LU(A);
        Matrix Y(B);

        return factor(LU, X, Y);
    }

    /**
     * Solve the linear system LUx = b using LU factorization; note: x and b may be the same calling argument
     * @param      A the matrix to be factored
     * @param      b a vector of coefficients corresponding to the equation LUx = b
     * @param[in]  x storage to hold the solution to LUx = b
     * @param[out] x the solution to the equation LUx = b
     * @return       an integer value according to the following:
     *               =  1 success, row pivoting was performed
     *               =  0 success, pivoting was not performed
     *               = -2 if matrix is singular
     *               = -3 if the dimensions of the matrices are incompatible
     */
    virtual int solve(const Matrix &A, std::vector<T> &x, const std::vector<T> &b) override final
    {
        Matrix LU(A);
        Matrix v(x);
        Matrix y(b);

        auto iError = factor(LU, v, y);
        if (iError >= 0)
        {
            auto size = v.size();
            for (std::size_t i = 0; i < size; ++i)
                x[i] = v[i];
        }

        return iError;
    }

protected:

    /**
     * Solve the lower triangular system L * Z = B; note: Y and B may be the same calling argument
     * @param      L     a lower triangular matrix
     * @param      B     a matrix of coefficients corresponding to the equation L * Z = B
     * @param      bUnit flag indicating whether or not the input matrix is lower triangular
     * @param[out] Z     the solution to the lower triangular system
     */
    inline virtual void solveLower(const Matrix &L, Matrix &Z, const Matrix &B,
                                   bool bUnit) const
    {
        std::vector<std::size_t> p;

        solveLower(L, Z, p, B, bUnit);
    }

    /**
     * Solve the lower triangular system L * Z = P * B; note: if p is non-empty, Y and B CANNOT be the same
     * calling argument!
     * @param      L     a lower triangular matrix
     * @param      p     a row permutation vector operating on matrix B
     * @param      B     a matrix of coefficients corresponding to the equation L * Z = P * B
     * @param      bUnit flag indicating whether or not the input matrix is lower triangular
     * @param[out] Z     the solution to the lower triangular system
     */
    virtual void solveLower(const Matrix &L, Matrix &Z, const std::vector<std::size_t> &p,
                            const Matrix &B, bool bUnit) const
    {
        auto m = L.rows();
        auto n = L.columns();
        auto q = Z.columns();
        bool bPermute = !p.empty();
        std::size_t in = 0, iq = 0;
        for (std::size_t i = 0; i < m; ++i, in += n, iq += q)
        {
            auto Lii = L[in + i];
            for (std::size_t j = 0; j < q; ++j)
            {
                T sum(0.0);
                for (std::size_t k = 0, kq = 0; k < i; ++k)
                {
                    sum += L[in + k] * Z[kq + j];
                    kq += q;
                }

                auto Bij = B[(bPermute ? p[i] * q : iq) + j];
                Z[iq + j] = Bij - sum;
                if (!bUnit)
                    Z[iq + j] /= Lii;
            }
        }
    }

public:

    /**
     * Solve the linear system LUX = B; note: X and B may be the same calling argument. Also, if row pivoting
     * was performed to obtain the input LU factorization, matrix B will need to be pre-multiplied by row
     * permutation matrix P.
     * @param      LU the LU matrix factorization produced by the factor() function.
     * @param      B  a matrix of coefficients corresponding to the equation LUX = B
     * @param[out] X  the solution to the equation LUX = B
     * @return        an integer value according to the following:
     *                =  0 success
     *                = -3 if the dimensions of the matrices are incompatible
     */
    virtual int solveLowerUpper(const Matrix &LU, Matrix &X, const Matrix &B) const final
    {
        int iError = 0;
        if (!this->isCompatible(LU, X, B))
            iError = -3;
        else
        {
            // solve L * Y = B
            solveLower(LU, X, B, m_triangularMatrixType == TriangularMatrixType::Enum::Lower);

            // now solve U * X = Y
            this->solveUpper(LU, X, X, m_triangularMatrixType == TriangularMatrixType::Enum::Upper);
        }

        return iError;
    }

protected:

    /**
     * indicates lower/upper factorization
     */
    const TriangularMatrixType m_triangularMatrixType;
};

}

}

}

}

#endif
