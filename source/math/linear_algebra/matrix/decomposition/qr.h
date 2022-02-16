#ifndef QR_H
#define QR_H

#include "givens.h"
#include "linear_solver.h"
#include <algorithm>

namespace math
{

namespace linear_algebra
{

namespace matrix
{

namespace decomposition
{

/**
 * This class contains methods to compute QR factorizations of m x n matrices
 */
template<typename Matrix>
class QR_Factor
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
    QR_Factor(void)
    {

    }

    /**
     * Copy constructor
     */
    QR_Factor(const QR_Factor<Matrix> &solver)
    {
        operator = (solver);
    }

    /**
     * Move constructor
     */
    QR_Factor(QR_Factor<Matrix> &&solver)
    {
        operator = (std::move(solver));
    }

    /**
     * Destructor
     */
    ~QR_Factor(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    QR_Factor<Matrix> &operator = (const QR_Factor<Matrix> &solver)
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
    QR_Factor<Matrix> &operator = (QR_Factor<Matrix> &&solver)
    {
        if (&solver != this)
        {
            LinearSolver<Matrix>::operator = (std::move(solver));
        }

        return *this;
    }

    /**
     * Compute the determinant of a matrix using QR factorization
     * @param[in]  QR  the matrix to be factored
     * @param[out] QR  the upper triangular 'R' factor stored above and including the main diagonal, and the
     *                 last n - 1 coefficients of each Householder reflection vector used to triangularize the
     *                 input matrix are stored as columns below the main diagonal.
     * @param[out] det if successful, contains the determinant
     * @return         an integer value according to the following:
     *                 =  2 success, column pivoting was performed
     *                 =  0 success
     *                 = -1 if QR matrix isn't square
     */
    virtual int determinant(Matrix &QR, T &det) override final
    {
        int iError = 0;
        if (!QR.isSquare())
            iError = -1;

        if (iError >= 0)
        {
            auto m = QR.rows();
            auto n = QR.columns();
            std::vector<T> u(m > n ? m : n);
            iError = factor(QR, u);
            if (iError >= 0)
            {
                det = n % 2 ? -1.0 : 1.0; // initialize determinant
                auto np1 = QR.columns() + 1;
                auto size = QR.size();
                for (std::size_t inp1 = 0; inp1 < size; inp1 += np1)
                    det *= QR[inp1];

                if (this->m_numColSwaps % 2)
                    det = -det;
            }
        }

        return iError;
    }

    /**
     * Factor and solve the equation Ax = b using QR decomposition. The factorization has the form A = QR,
     * where Q is an orthogonal matrix, R is an upper triangular matrix, and A is the original matrix
     * @param[in]  B  a matrix of coefficients corresponding to the equation QRX = B
     * @param[in]  QR the matrix to be factored
     * @param[in]  u  storage to hold the lead coefficients of each Householder reflection vector; u should be
     *                sized to the maximum dimension of the input matrix
     * @param[in]  X  storage to hold the solution to QRX = B
     * @param[out] QR the upper triangular 'R' factor stored above and including the main diagonal, and the
     *                last n - 1 coefficients of each Householder reflection vector used to triangularize the
     *                input matrix are stored as columns below the main diagonal
     * @param[out] u  the lead coefficients of each Householder reflection vector starting in the first p
     *                positions, where p is the minimum dimension of the input matrix
     * @param[out] X  the solution to the equation QRX = B
     * @return        an integer value according to the following:
     *                =  2 success, column pivoting was performed
     *                =  0 success
     *                = -2 if matrix is singular
     *                = -3 if the dimensions of the matrices are incompatible
     */
    virtual int factor(Matrix &QR, Matrix &X, const Matrix &B, std::vector<T> &u) final
    {
        auto iError = factor(QR, u);
        if (!isProductNonsingular(QR))
            iError = -2;
        else if (!this->isCompatible(QR, X, B))
            iError = -3;
        else
        {
            formQTBProduct(QR, B, X, u);
            this->solveUpper(QR, X, X, false);

            if (iError == 2) // column pivoting was performed, solve X = Q * Y
            {
                combinatorics::Permutator::computeInversePermutationVector(this->m_q, this->m_w);
                X.permuteRows(this->m_w, this->m_q);
            }
        }

        return iError;
    }

    /**
     * Factor the equation Ax = b using QR decomposition. The factorization has the form A = QR, where Q is an
     * orthogonal matrix, R is an upper triangular matrix, and A is the original matrix
     * @param[in]  QR the matrix to be factored
     * @param[in]  u  storage to hold the lead coefficients of each Householder reflection vector; u should be
     *                sized to the maximum dimension of the input matrix
     * @param[out] QR the upper triangular 'R' factor stored above and including the main diagonal, and the
     *                last n - 1 coefficients of each Householder reflection vector used to triangularize the
     *                input matrix are stored as columns below the main diagonal
     * @param[out] u  the lead coefficients of each Householder reflection vector starting in the first p
     *                positions, where p is the minimum dimension of the input matrix
     * @return        an integer value according to the following:
     *                =  2 success, column pivoting was performed
     *                =  0 success
     */
    virtual int factor(Matrix &QR, std::vector<T> &u) final
    {
        // initialize pivot vector
        auto n = QR.columns();
        this->initialize(PivotType::Enum::Column, n);

        // form the upper triangular matrix
        int iError = 0;
        auto m = QR.rows();
        auto minDim = m < n ? m : n;
        auto &&signum = [] (auto &&x) { return x < 0.0 ? -1.0 : 1.0; };
        for (std::size_t i = 0, in = 0; i < minDim; ++i, in += n)
        {
            bool bSuccess = pivot(QR, i, 0, iError);
            if (!bSuccess)
                break;

            T norm(0.0);
            auto inpi = in + i, j = i, jnpi = inpi;
            for (; j < m; ++j, jnpi += n)
            {
                u[j] = QR[jnpi];
                norm += u[j] * u[j];
            }

            auto y = signum(u[i]) * sqrt(norm);
            norm = sqrt(norm + y * (y + 2 * u[i]));
            u[i] += y;

            while (j > i)
                u[--j] /= norm;

            // perform Householder triangularization
            householderTransform(QR, i, in, u);

            // store Household reflectors in lower triangular portion of the QR matrix.
            // The leading entry of the j-th reflection vector is stored in the u vector.
            for (j = i + 1, jnpi = inpi; j < m; ++j)
            {
                jnpi += n;
                QR[jnpi] = u[j];
            }
        }

        return iError;
    }

    /**
     * Form the product of matrices Q' and B, the transpose of the orthogonal matrix in the QR factorization
     * and the coefficient matrix, respectively
     * @param      QR  the upper triangular 'R' factor stored above and including the main diagonal, and the
     *                 last n - 1 coefficients of each Householder reflection vector used to triangularize the
     *                 input matrix are stored as columns below the main diagonal
     * @param      B   a coefficient matrix corresponding to the equation AX = B
     * @param[in]  QTB storage to hold the product of matrices Q' and B
     * @param      u   the lead coefficients of each Householder reflection vector starting in the first p
     *                 positions, where p is the minimum dimension of the input matrix
     * @param[out] QTB the product of matrices Q' and B
     * @return         an integer value according to the following:
     *                 =  0 success
     *                 = -3 if the dimensions of the matrices are incompatible
     */
    virtual int formQTBProduct(const Matrix &QR, const Matrix &B, Matrix &QTB,
                               const std::vector<T> &u) const final
    {
        auto m = QR.rows();
        auto p = B.rows();
        auto q = B.columns();
        int iError = 0;
        if (m != p || p != QTB.rows() || q != QTB.columns())
            iError = -3;

        if (iError >= 0)
        {
            // unfortunately this is necessary...
            if (&B != &QTB)
            {
                auto size = B.size();
                for (std::size_t i = 0; i < size; ++i)
                    QTB[i] = B[i];
            }

            // now form the QTB product
            auto n = QR.columns();
            auto np1 = n + 1;
            auto mm1n = (m - 1) * n;
            std::size_t knpk = 0, kq = 0;
            for (std::size_t k = 0; k < n; ++k)
            {
                for (std::size_t j = 0; j < q; ++j)
                {
                    auto inpk = knpk, &&iqpj = kq + j;
                    auto y = u[k] * QTB[iqpj];
                    while (inpk < mm1n)
                    {
                        inpk += n, iqpj += q;
                        y += QR[inpk] * QTB[iqpj];
                    }

                    while (inpk > knpk)
                    {
                        QTB[iqpj] -= 2 * QR[inpk] * y;
                        inpk -= n, iqpj -= q;
                    }

                    QTB[iqpj] -= 2 * u[k] * y;
                }

                knpk += np1, kq += q;
            }
        }

        return iError;
    }

    /**
     * Form the product of matrices Q and X, the orthogonal matrix in the QR factorization and the solution
     * matrix corresponding to the linear equation AX = B, respectively
     * @param      QR the upper triangular 'R' factor stored above and including the main diagonal, and the
     *                last n - 1 coefficients of each Householder reflection vector used to triangularize the
     *                input matrix are stored as columns below the main diagonal
     * @param      X  the solution to AX = B
     * @param[in]  QX storage to hold the product of matrices Q and X
     * @param      u  the lead coefficients of each Householder reflection vector starting in the first p
     *                positions, where p is the minimum dimension of the input matrix
     * @param[out] QX the product of matrices Q and X
     * @return        an integer value according to the following:
     *                =  0 success
     *                = -3 if the dimensions of the matrices are incompatible
     */
    virtual int formQXProduct(const Matrix &QR, const Matrix &X, Matrix &QX,
                              const std::vector<T> &u) const final
    {
        auto m = QR.rows();
        auto p = X.rows();
        auto q = X.columns();
        int iError = 0;
        if (m != p || p != QX.rows() || q != QX.columns())
            iError = -3;

        if (iError >= 0)
        {
            // unfortunately this is necessary...
            if (&X != &QX)
            {
                auto size = X.size();
                for (std::size_t i = 0; i < size; ++i)
                    QX[i] = X[i];
            }

            // now form the QX product
            auto n = QR.columns();
            auto np1 = n + 1;
            auto mm1n = (m - 1) * n;
            auto minDim = std::min(m, n);
            auto minDimm1 = minDim - 1;
            auto knpk = minDimm1 * n + minDimm1, kq = minDim * q;
            for (auto k = minDim; k > 0; knpk -= np1)
            {
                --k, kq -= q;
                for (std::size_t j = 0; j < q; ++j)
                {
                    std::size_t inpk = knpk, &&iqpj = kq + j;
                    auto y = u[k] * QX[iqpj];
                    while (inpk < mm1n)
                    {
                        inpk += n, iqpj += q;
                        y += QR[inpk] * QX[iqpj];
                    }

                    while (inpk > knpk)
                    {
                        QX[iqpj] -= 2 * QR[inpk] * y;
                        inpk -= n, iqpj -= q;
                    }

                    QX[iqpj] -= 2 * u[k] * y;
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
        return std::string("QR_Factor<") + typeid(Matrix).name() + ">";
    }

    /**
     * Form the orthogonal matrix Q using the Householder reflection vectors
     * @param      QR the upper triangular 'R' factor stored above and including the main diagonal, and the
     *                last n - 1 coefficients of each Householder reflection vector used to triangularize the
     *                input matrix are stored as columns below the main diagonal
     * @param[in]  Q  must contain the identity matrix
     * @param      u  the lead coefficients of each Householder reflection vector starting in the first p
     *                positions, where p is the minimum dimension of the input matrix
     * @param[out] Q  transformed into an orthogonal matrix from the Householder reflection vectors such that A
     *                = QR
     * @return        an integer value according to the following:
     *                =  0 success
     *                = -1 if Q matrix isn't square
     *                = -3 if the dimensions of the matrices are incompatible
     */
    virtual int getOrthogonalMatrix(const Matrix &QR, Matrix &Q, const std::vector<T> &u) const final
    {
        int iError = 0;
        if (!Q.isSquare())
            iError = -1;
        else if (QR.rows() != Q.rows())
            iError = -3;
        else
            formQXProduct(QR, Q, Q, u);

        return iError;
    }

    /**
     * Get the upper triangle
     */
    inline virtual Matrix getUpperTriangle(const Matrix &QR) const final
    {
        return QR.getUpperTriangle();
    }

protected:

    /**
     * Perform Householder triangularization
     * @param[in]  QR the matrix upon which Householder triangularization will be performed
     * @param      i  the index to the current diagonal element
     * @param      in the product of the diagonal element index and the number of columns in QR
     * @param      u  the lead coefficients of each Householder reflection vector starting in the first p
     *                positions, where p is the minimum dimension of the input matrix
     * @param[out] QR the result of the transformation, where the elements in the column containing the i-th
     *                diagonal element have been zeroed, and the remaining n - i + 1 columns to the right of
     *                the i-th column have been modified by Householder reflectors. The i - 1 preceding columns
     *                are unmodified.
     */
    virtual void householderTransform(Matrix &QR, std::size_t i, std::size_t in,
                                      const std::vector<T> &u) const final
    {
        // perform householder triangularization
        auto m = QR.rows();
        auto n = QR.columns();
        for (auto j = i; j < n; ++j)
        {
            T y(0.0);
            auto k = i, &&knpj = in + j;
            for (; k < m; ++k, knpj += n)
                y += QR[knpj] * u[k];

            while (k > i)
            {
                --k, knpj -= n;
                QR[knpj] -= 2 * y * u[k];
            }
        }
    }

public:

    /**
     * Compute the inverse of a matrix using QR factorization
     * @param[in]  A    the matrix to be inverted
     * @param[in]  Ainv must contain the identity matrix
     * @param[out] A    the upper triangular 'R' factor stored above and including the main diagonal, and the
     *                  last n - 1 coefficients of each Householder reflection vector used to triangularize the
     *                  input matrix are stored as columns below the main diagonal
     * @param[out] Ainv the inverse of the original matrix
     * @return          an integer value according to the following:
     *                  =  2 success, column pivoting was performed
     *                  =  0 success
     *                  = -1 if matrix isn't square
     *                  = -2 if matrix is singular
     */
    inline virtual int inverse(Matrix &A, Matrix &Ainv) override final
    {
        std::vector<T> u(A.rows());

        auto iError = inverse(A, Ainv, u);

        return iError;
    }

    /**
     * Compute the inverse of a matrix using QR factorization
     * @param[in]  A    the matrix to be inverted
     * @param[in]  Ainv must contain the identity matrix
     * @param[in]  u    storage to hold the lead coefficients of each Householder reflection vector
     * @param[out] A    the upper triangular 'R' factor stored above and including the main diagonal, and the
     *                  last n - 1 coefficients of each Householder reflection vector used to triangularize the
     *                  input matrix are stored as columns below the main diagonal
     * @param[out] Ainv the inverse of the original matrix
     * @param[out] u    the lead coefficients of each Householder reflection vector
     * @return          an integer value according to the following:
     *                  =  2 success, column pivoting was performed
     *                  =  0 success
     *                  = -1 if matrix isn't square
     *                  = -2 if matrix is singular
     *                  = -3 if the dimensions of the matrices are incompatible
     */
    virtual int inverse(Matrix &A, Matrix &Ainv, std::vector<T> &u) final
    {
        int iError = 0;
        if (!A.isSquare() || !Ainv.isSquare())
            iError = -1;
        else
        {
            iError = factor(A, u);
            if (!isProductNonsingular(A))
                iError = -2;
            else if (Ainv.rows() != A.rows())
                iError = -3;
            else if (iError >= 0)
            {
                formQTBProduct(A, Ainv, Ainv, u);
                this->solveUpper(A, Ainv, Ainv, false);

                if (iError == 2) // column pivoting was performed, solve X = Q * Y
                {
                    combinatorics::Permutator::computeInversePermutationVector(this->m_q, this->m_w);
                    Ainv.permuteRows(this->m_w, this->m_q);
                }
            }
        }

        return iError;
    }

    /**
     * Identify whether or not the QR product produces a singular matrix
     * @param QR the upper triangular 'R' factor stored above and including the main diagonal, and the last n -
     *           1 coefficients of each Householder reflection vector used to triangularize the input matrix
     *           are stored as columns below the main diagonal
     */
    inline virtual bool isProductNonsingular(const Matrix &QR) const final
    {
        auto np1 = QR.columns() + 1;
        auto size = QR.size();
        for (std::size_t inp1 = 0; inp1 < size; inp1 += np1)
            if (QR[inp1] == 0.0)
                return false;

        return true;
    }

    /**
     * Perform column pivoting
     * @param[in]  AP     the matrix upon which pivoting will be performed
     * @param      i      the index to the current row/column
     * @param      in     the product of the row index and the number of columns in AP (not used)
     * @param[in]  iError an error code that is modified as result of any pivoting that is performed
     * @param[out] AP     possibly further permuted by column pivoting
     * @param[out] iError an integer value that indicates what kind of pivoting, if any, has occurred:
     *                    = 2 column pivoting was performed
     */
    inline bool pivot(Matrix &AP, std::size_t i, std::size_t, int &iError) override final
    {
        // permute column indexes based on the column having the maximum 2-norm;
        // only attempt column pivoting if there are at least two columns remaining...
        auto n = AP.columns();
        if (i + 1 < n && this->columnPivot(AP, i, 0, ColumnPivotStrategy::Enum::MaxNorm))
            iError = 2;

        return true;
    }

    /**
     * Solve the linear system AX = B using QR factorization, where X and B may be the same calling argument
     * @param      A the matrix to be factored
     * @param      B a matrix of coefficients corresponding to the equation AX = B
     * @param[in]  X storage to hold the solution to AX = B
     * @param[out] X the solution to the equation AX = B
     * @return       an integer value according to the following:
     *               =  3 success, complete pivoting was performed
     *               =  2 success, column pivoting was performed
     *               =  1 success, row pivoting was performed
     *               =  0 success, pivoting was not performed
     *               = -2 if matrix is singular
     *               = -3 if the dimensions of the matrices are incompatible
     */
    inline virtual int solve(const Matrix &A, Matrix &X, const Matrix &B) override final
    {
        auto m = A.rows();
        auto n = A.columns();
        std::vector<T> u(m > n ? m : n);
        Matrix QR(A), Y(B);

        return factor(QR, X, Y, u);
    }

    /**
     * Solve the linear system Ax = b using QR factorization, where x and b may be the same calling argument
     * @param      A the matrix to be factored
     * @param      b a vector of coefficients corresponding to the equation Ax = b
     * @param[in]  x storage to hold the solution to Ax = b
     * @param[out] x the solution to the equation Ax = b
     * @return       an integer value according to the following:
     *               =  3 success, complete pivoting was performed
     *               =  2 success, column pivoting was performed
     *               =  1 success, row pivoting was performed
     *               =  0 success, pivoting was not performed
     *               = -2 if matrix is singular
     *               = -3 if the dimensions of the matrices are incompatible
     */
    virtual int solve(const Matrix &A, std::vector<T> &x, const std::vector<T> &b) override final
    {
        auto m = A.rows();
        auto n = A.columns();
        std::vector<T> u(m > n ? m : n);
        Matrix QR(A), v(x), y(b);

        auto iError = factor(QR, v, y, u);
        if (iError >= 0)
        {
            auto size = v.size();
            for (std::size_t i = 0; i < size; ++i)
                x[i] = v[i];
        }

        return iError;
    }

    /**
     * This subroutine updates a QR decomposition (equivalent to performing QR-factorization on the quantity A
     * + x * y', where x and y are column vectors). The subroutine returns true if the factorization was
     * updated.
     * @param[in]  Q    the orthogonal matrix
     * @param[in]  R    the upper triangular matrix
     * @param[in]  x, y are column vectors used to form the product x * y'. The resultant outer product is used
     *                  to update the factorization
     * @param[out] Q    the updated orthogonal matrix
     * @param[out] R    the updated upper triangular matrix
     * @param[out] x    has been destroyed
     */
    inline virtual bool update(Matrix &Q, Matrix &R, std::vector<T> &x,
                               const std::vector<T> &y) final
    {
        std::vector<std::size_t> p;

        return update(Q, R, x, y, p);
    }

    /**
     * This subroutine performs a rank-one update to a QR decomposition (equivalent to performing
     * QR-factorization on the quantity AP + x * y'P, where x and y are column vectors and P is a column
     * permutation matrix). The subroutine returns true if the factorization was updated.
     * @param[in]  Q    the orthogonal matrix
     * @param[in]  R    the upper triangular matrix
     * @param[in]  x, y are column vectors used to form the product x * y'. The resultant outer product is used
     *                  to update the factorization
     * @param      p    a vector containing the column permutation order of the input factorization
     * @param[out] Q    the updated orthogonal matrix
     * @param[out] R    the updated upper triangular matrix
     * @param[out] x    has been destroyed
     */
    virtual bool update(Matrix &Q, Matrix &R, std::vector<T> &x, const std::vector<T> &y,
                        const std::vector<std::size_t> &p) final
    {
        if (!Q.isSquare())
            return false;

        // copy x[1] to x[m - 1] to first sub-diagonal column of R
        auto m = Q.rows();
        for (std::size_t i = 1; i < m; ++i)
            R(i, 0) = x[i];

        // first compute w = Q' * x
        auto x0 = x[0];
        for (std::size_t i = 0; i < m; ++i)
        {
            x[i] = Q(0, i) * x0;
            for (std::size_t j = 1; j < m; ++j)
                x[i] += Q(j, i) * R(j, 0);
        }

        // zero the first sub-diagonal column of R
        for (std::size_t i = 1; i < m; ++i)
            R(i, 0) = 0.0;

        // use givens rotations to zero the last m - 1 components of w from the bottom up
        transformation::Givens givens;
        auto n = R.columns();
        auto mm1 = m - 1, &&nm1 = n - 1;
        for (auto i = mm1, in = mm1 * n; i > 0; --i, in -= n)
        {
            givens.computeRotation(x[i - 1], x[i]);
            givens.preMultiply(R, in - n, in, i - 1, nm1); // apply this rotation to R
            givens.postMultiply(Q, i - 1, i, 0, mm1, true); // apply this rotation to Q
        }

        // Update the first row of R by applying e1 * v' to R
        for (std::size_t j = 0; j < n; ++j)
        {
            auto z = p.empty() ? y[j] : y[p[j]];
            R(0, j) += x[0] * z;
        }

        // R is now upper-Hessenberg - now use givens rotations to zero the sub-diagonal elements
        for (std::size_t i = 0, in = 0; i < nm1 && i < mm1; ++i, in += n)
        {
            givens.computeRotation(R[in + i], R[in + n + i]);
            givens.preMultiply(R, in, in + n, i + 1, nm1); // apply this rotation to R
            givens.postMultiply(Q, i, i + 1, 0, mm1, true); // apply this rotation to Q
        }

        return true;
    }
};

}

}

}

}

#endif
