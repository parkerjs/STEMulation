#ifndef LINEAR_SOLVER_H
#define LINEAR_SOLVER_H

#include "column_pivot_strategy.h"
#include "pivot_type.h"
#include "reflective.h"
#include <cmath>
#include <complex>
#include <iostream>
#include <typeinfo>
#include <vector>

namespace math
{

namespace linear_algebra
{

namespace matrix
{

namespace decomposition
{

/**
 * This class serves as an abstract base for subclasses that implement linear solvers
 */
template<typename Matrix>
class LinearSolver
: virtual private attributes::abstract::Reflective
{
public:

    /**
     * Typedef declarations
     */
    typedef typename Matrix::decay_type T;

    /**
     * Constructor
     */
    LinearSolver(void)
    : m_numColSwaps(0),
      m_numRowSwaps(0)
    {

    }

    /**
     * Copy constructor
     */
    LinearSolver(const LinearSolver<Matrix> &solver)
    {
        operator = (solver);
    }

    /**
     * Move constructor
     */
    LinearSolver(LinearSolver<Matrix> &&solver)
    {
        operator = (std::move(solver));
    }

    /**
     * Destructor
     */
    virtual ~LinearSolver(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    LinearSolver<Matrix> &operator = (const LinearSolver<Matrix> &solver)
    {
        if (&solver != this)
        {
            m_numColSwaps = solver.m_numColSwaps;
            m_numRowSwaps = solver.m_numRowSwaps;
            m_p = solver.m_p;
            m_q = solver.m_q;
            m_w = solver.m_w;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    LinearSolver<Matrix> &operator = (LinearSolver<Matrix> &&solver)
    {
        if (&solver != this)
        {
            m_numColSwaps = std::move(solver.m_numColSwaps);
            m_numRowSwaps = std::move(solver.m_numRowSwaps);
            m_p = std::move(solver.m_p);
            m_q = std::move(solver.m_q);
            m_w = std::move(solver.m_w);
        }

        return *this;
    }

    /**
     * Perform column pivoting
     * @param[in]  PAQ                 the matrix upon which column pivoting will be performed
     * @param      j                   the index to the current column
     * @param      jn                  the product of the diagonal element index and the number of columns in
     *                                 PAQ
     * @param      columnPivotStrategy the type of column pivoting to be performed. If MaxElement is specified,
     *                                 then the algorithm will search for the largest element in the jth row;
     *                                 if NonZeroElement is specified, then the pivot will be the first
     *                                 non-zero element in the jth row
     * @param[out] PAQ                 possibly further permuted by column pivoting
     * @return                         true if column pivoting was performed
     */
    virtual bool columnPivot(Matrix &PAQ, std::size_t j, std::size_t jn,
                             const ColumnPivotStrategy &columnPivotStrategy) final
    {
        // find the pivot in ith row
        auto k = j;
        auto n = PAQ.columns();
        switch (columnPivotStrategy)
        {
            case ColumnPivotStrategy::Enum::MaxElement:
            for (auto i = j + 1; i < n; ++i)
                if (std::abs(PAQ[jn + k]) < std::abs(PAQ[jn + i]))
                    k = i;

            break;

            case ColumnPivotStrategy::Enum::MaxNorm:
            {
                // compute the norm square of each column, determine column with the largest square norm
                T maxNormSq(0), normSq(0);
                auto m = PAQ.rows();
                for (auto l = j; l < n; ++l)
                {
                    for (auto inpj = j; inpj < m; inpj += n)
                        normSq += PAQ[inpj] * PAQ[inpj];

                    if (normSq > maxNormSq)
                    {
                        maxNormSq = normSq;
                        k = l;
                    }
                }

                break;
            }

            case ColumnPivotStrategy::Enum::NonZeroElement:
            while ((k + 1) < n && PAQ[jn + k] == 0.0)
                ++k;

            break;

            default:
            // this should not happen...
            throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                                     "Unknown/unsupported column pivot strategy.");
        }

        // perform swap if necessary and return pivot flag
        return columnSwap(PAQ, j, k, 0, PAQ.size());
    }

    /**
     * Swap columns
     * @param[in]  PAQ                     the matrix upon which column pivoting will be performed
     * @param      j                       the index to the current column
     * @param      k                       the index of the column to be swapped with the jth column
     * @param      inStart                 the product of the starting row index and the number of columns
     * @param      ip1nEnd                 the product of the ending row index plus one and the number of
     *                                     columns
     * @param      updatePermutationVector a flag indicating whether or not to update the permutation index
     *                                     vector
     * @param[out] PAQ                     columns j and k of PAQ have been swapped
     */
    virtual bool columnSwap(Matrix &PAQ, std::size_t j, std::size_t k, std::size_t inStart,
                            std::size_t ip1nEnd, bool bUpdatePermutationVector = true) final
    {
        if (j != k)
        {
            // swap column indices
            ++m_numColSwaps;
            if (bUpdatePermutationVector)
                std::swap(m_q[k], m_q[j]);

            // swap columns
            auto n = PAQ.columns();
            for (auto in = inStart; in < ip1nEnd; in += n)
                std::swap(PAQ[in + j], PAQ[in + k]);

            return true;
        }

        return false;
    }

public:

    /**
     * Compute the determinant of a matrix. The subroutine returns an error code; see implementation for
     * details
     * @param[in]  A   the matrix to be factored
     * @param[out] A   see implementation for details regarding how A is modified by the subroutine
     * @param[out] det if successful, contains the determinant
     */
    virtual int determinant(Matrix &A, T &det) = 0;

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return std::string("LinearSolver<") + typeid(Matrix).name() + ">";
    }

    /**
     * Get the row or column permutation matrix
     * @param      pivotType the type of pivoting, row or column
     * @param[in]  P         storage to hold the permutation matrix; will be resized if necessary
     * @param[out] P         the specified permutation matrix
     */
    virtual void getPermutationMatrix(const PivotType &pivotType, Matrix &P) const final
    {
        switch (pivotType)
        {
            case PivotType::Enum::Column:
            {
                // resize P if necessary...
                auto n = m_q.size();
                if (P.rows() != n || P.columns() != n)
                    P.resize(n, n, false);

                for (std::size_t i = 0; i < n; ++i)
                    for (std::size_t j = 0, jn = 0; jn < P.size(); ++j, jn += n)
                        P[jn + i] = (m_q[i] == j) ? 1.0 : 0.0;

                break;
            }

            case PivotType::Enum::Row:
            {
                // resize P if necessary...
                auto n = m_p.size();
                if (P.rows() != n || P.columns() != n)
                    P.resize(n, n, false);

                for (std::size_t i = 0, in = 0; i < n; ++i, in += n)
                    for (std::size_t j = 0; j < n; ++j)
                        P[in + j] = (m_p[i] == j) ? 1.0 : 0.0;

                break;
            }

            default:
            // this should not happen...
            throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                                     "Unknown/unsupported pivot type.");
        }
    }

    /**
     * Get the row or column permutation vector; the subroutine returns the permutation vector is its return
     * value
     * @param[in]  p         storage to hold the permutation vector
     * @param      pivotType the type of pivoting, row or column
     * @param[out] p         the permutation vector
     */
    virtual void getPermutationVector(const PivotType &pivotType, std::vector<std::size_t> &p) const final
    {
        switch (pivotType)
        {
            case PivotType::Enum::Column:
            p = m_q;
            break;

            case PivotType::Enum::Row:
            p = m_p;
            break;

            default:
            // this should not happen...
            throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                                     "Unknown/unsupported pivot type.");
        }
    }

protected:

    /**
     * Initialization function
     * @param pivotType       the type of pivoting, row or column
     * @param pivotVectorSize the size of the pivot vector
     */
    inline virtual bool initialize(const PivotType &pivotType, std::size_t pivotVectorSize) final
    {
        return initialize(pivotType, pivotVectorSize, { });
    }

    /**
     * Initialization function
     * @param pivotType       the type of pivoting, row or column
     * @param pivotVectorSize the size of the pivot vector
     * @param pivot           an optional argument which is used to initialize the object's member pivot vector
     */
    virtual bool initialize(const PivotType &pivotType, std::size_t pivotVectorSize,
                            const std::vector<std::size_t> &pivot)
    {
        switch (pivotType)
        {
            case PivotType::Enum::Column:
            // initialize column pivot vector
            if (pivotVectorSize != m_q.size())
            {
                m_q.resize(pivotVectorSize);
                m_w.resize(pivotVectorSize);
            }

            if (pivot.empty())
            {
                for (std::size_t i = 0; i < m_q.size(); ++i)
                    m_q[i] = i;
            }
            else
                std::copy(pivot.begin(), pivot.end(), m_q.begin());

            break;

            case PivotType::Enum::Row:
            // may need to resize the pivot vector
            if (pivotVectorSize != m_p.size())
                m_p.resize(pivotVectorSize);

            if (pivot.empty())
            {
                for (std::size_t j = 0; j < m_p.size(); ++j)
                    m_p[j] = j;
            }
            else
                std::copy(pivot.begin(), pivot.end(), m_p.begin());

            break;

            default:
            // this should not happen...
            throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                                     "Unknown/unsupported pivot type.");

            return false;
        }

        m_numColSwaps = 0;
        m_numRowSwaps = 0;

        return true;
    }

public:

    /**
     * Compute the inverse of a matrix. The subroutine returns an error code; see implementation for details
     * @param[in]  A    the matrix to be inverted
     * @param[in]  Ainv must contain the identity matrix
     * @param[out] A    see implementation for details regarding how A is modified by the subroutine
     * @param[out] Ainv the inverse of the original matrix
     */
    virtual int inverse(Matrix &A, Matrix &Ainv) = 0;

protected:

    /**
     * Identify whether or not the matrices in the equation AX = B are compatible
     * @param A the matrix to be factored
     * @param B a matrix of coefficients corresponding to the equation AX = B
     * @param X storage to hold the solution to AX = B
     */
    inline virtual bool isCompatible(const Matrix &A, const Matrix &X, const Matrix &B) const final
    {
        return A.columns() == X.rows() && X.columns() == B.columns() && A.rows() == B.rows();
    }

    /**
     * Perform row and/or column pivoting
     * @param[in]  PAQ    the matrix upon which pivoting will be performed
     * @param      i      the index to the current row/column
     * @param      in     the product of the row index and the number of columns in PAQ
     * @param[in]  iError an error code that is modified as result of any pivoting that is performed
     * @param[out] PAQ    possibly further permuted by row/column pivoting
     * @param[out] iError an integer value that indicates what kind of pivoting, if any, has occurred:
     *                    = 3 success, complete pivoting was performed
     *                    = 2 success, column pivoting was performed
     *                    = 1 success, row pivoting was performed
     *                    = 0 pivoting was not performed
     */
    virtual bool pivot(Matrix &PAQ, std::size_t i, std::size_t in, int &iError)
    {
        // find the pivot in ith column
        if (rowPivot(PAQ, i, in))
            iError |= 1;

        // detect singularity - only attempt column pivoting if there are at least two columns remaining...
        auto &PAQii = PAQ[in + i];
        auto n = PAQ.columns();
        auto ip1 = i + 1;
        if (ip1 < n && PAQii == 0.0)
        {
            // permute column indices in order to find a non-zero pivot element
            if (columnPivot(PAQ, i, in, ColumnPivotStrategy::Enum::NonZeroElement))
                iError |= 2;

            // done if pivot is zero - no more factoring can be done
            if (PAQii == 0.0)
                return false;
        }

        return true;
    }

    /**
     * Perform row pivoting
     * @param[in]  PAQ the matrix upon which row pivoting will be performed
     * @param      i   the index to the current row
     * @param      in  the product of the diagonal element index and the number of columns in PAQ
     * @param[out] PAQ possibly further permuted by row pivoting
     * @return     true if row pivoting was performed
     */
    virtual bool rowPivot(Matrix &PAQ, std::size_t i, std::size_t in) final
    {
        // find the pivot in jth column
        auto m = PAQ.rows();
        auto n = PAQ.columns();
        std::size_t jn = in + n, k = i, kn = in;
        for (auto j = i + 1; j < m; ++j)
        {
            if (std::abs(PAQ[kn + i]) < std::abs(PAQ[jn + i]))
            {
                k = j;
                kn = jn;
            }

            jn += n;
        }

        // perform swap if necessary and return pivot flag
        return rowSwap(PAQ, i, k, in, kn, 0, n);
    }

    /**
     * Swap rows
     * @param[in]  PAQ                     the matrix upon which row pivoting will be performed
     * @param      i                       the index to the current row
     * @param      k                       the index of the row to be swapped with the ith row
     * @param      in                      the product of the diagonal element index and the number of columns
     *                                     in PAQ
     * @param      kn                      the product of the swap index and the number of columns in PAQ
     * @param      jStart                  the index of the starting column
     * @param      jp1End                  the index of the ending column plus one
     * @param      updatePermutationVector a flag indicating whether or not to update the permutation index
     *                                     vector
     * @param[out] PAQ                     rows i and k of PAQ have been swapped
     */
    virtual bool rowSwap(Matrix &PAQ, std::size_t i, std::size_t k, std::size_t in, std::size_t kn,
                         std::size_t jStart, std::size_t jp1End, bool bUpdatePermutationVector = true) final
    {
        if (i != k)
        {
            // swap row indices
            ++m_numRowSwaps;
            if (bUpdatePermutationVector)
                std::swap(m_p[k], m_p[i]);

            // swap rows
            for (auto j = jStart; j < jp1End; ++j)
                std::swap(PAQ[kn + j], PAQ[in + j]);

            return true;
        }

        return false;
    }

public:

    /**
     * Solve the linear system AX = B, where X and B are matrix objects. The subroutine returns an error code;
     * see implementation for details
     * @param      A the matrix to be factored
     * @param      B a matrix of coefficients corresponding to the equation AX = B
     * @param[in]  X storage to hold the solution to AX = B
     * @param[out] X the solution to the equation AX = B
     */
    virtual int solve(const Matrix &A, Matrix &X, const Matrix &B) = 0;

    /**
     * Solve the linear system Ax = b, where x and b are vector objects. The subroutine returns an error code;
     * see implementation for details
     * @param      A the matrix to be factored
     * @param      b a vector of coefficients corresponding to the equation Ax = b
     * @param[in]  x storage to hold the solution to Ax = b
     * @param[out] x the solution to the equation Ax = b
     */
    virtual int solve(const Matrix &A, std::vector<T> &x, const std::vector<T> &b) = 0;

    /**
     * Solve the upper triangular system U * Y = Z, where Y and Z may be the same calling argument
     * @param      U     an upper triangular matrix
     * @param      Z     a matrix of coefficients corresponding to the equation U * Y = Z
     * @param      bUnit flag indicating whether or not the input matrix is upper triangular
     * @param[out] Y     the solution to the upper triangular system
     */
    virtual void solveUpper(const Matrix &U, Matrix &Y, const Matrix &Z, bool bUnit) const
    {
        auto n = U.columns();
        auto p = Z.columns();
        auto in = U.size();
        auto ip = Z.size();
        for (auto i = n; i > 0; i--)
        {
            in -= n, ip -= p;
            auto Uii = U[in + i - 1];
            for (std::size_t j = 0; j < p; ++j)
            {
                T sum(0);
                for (std::size_t k = i, kp_j = ip + j; k < n; ++k)
                {
                    kp_j += p;
                    sum += U[in + k] * Y[kp_j];
                }

                Y[ip + j] = Z[ip + j] - sum;
                if (!bUnit)
                    Y[ip + j] /= Uii;
            }
        }
    }

protected:

    /**
     * number of column swaps performed in pivoting
     */
    std::size_t m_numColSwaps;

    /**
     * number of row swaps performed
     */
    std::size_t m_numRowSwaps;

    /**
     * row permutation vector
     */
    std::vector<std::size_t> m_p;

    /**
     * column permutation vector
     */
    std::vector<std::size_t> m_q;

    /**
     * workspace permutation vector
     */
    std::vector<std::size_t> m_w;
};

}

}

}

}

#endif
