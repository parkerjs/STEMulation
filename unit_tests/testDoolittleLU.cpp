#include "doolittle_lu.h"
#include "matrix.h"
#include "std_vector_overloads.h"
#include "testDoolittleLU.h"
#include "unitTestManager.h"
#include <random>

// using namespace declarations
using namespace attributes::abstract;
using namespace math;
using namespace math::combinatorics;
using namespace math::linear_algebra::matrix;
using namespace math::linear_algebra::matrix::decomposition;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testDoolittleLU", &DoolittleLU_UnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
DoolittleLU_UnitTest::DoolittleLU_UnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
DoolittleLU_UnitTest *DoolittleLU_UnitTest::create(UnitTestManager *pUnitTestManager)
{
    DoolittleLU_UnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new DoolittleLU_UnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool DoolittleLU_UnitTest::execute(void)
{
    /*
     * Test Doolittle LU factorization
     */

    std::ofstream stream("outputs/doolittleLU_FactorTestOutput.dat");

    std::random_device seed;
    std::mt19937 generator(seed());
    std::normal_distribution<double> gaussian(5, 2);

    stream << " Test Doolittle LU factorization with partial pivoting:" << std::endl << std::endl;

    for (std::size_t i = 1; i <= 10; i++)
    {
        Matrix<2, double> A(i, i);
        std::generate(A.begin(), A.end(), [&] (void) { return gaussian(generator); });

        std::vector<std::size_t> p(i); // row permutation vector
        Matrix<2, double> P(i, i); // row permutation matrix

        stream << " Original matrix:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << A << std::endl << std::endl;

        Matrix<2, double> LU = A;
        Doolittle_LU_Factor<Matrix<2, double>> solver;
        int iErr = solver.factor(LU);
        if (iErr >= 0)
        {
            // extract the lower and upper triangles
            Matrix<2, double> L = LU.getLowerTriangle(-1);
            Matrix<2, double> U = LU.getUpperTriangle();
            L.setDiagonal(1);

            stream << " Doolittle LU factorization:" << std::endl << std::endl;

            stream << " Lower triangle:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << L << std::endl << std::endl;

            stream << " Upper triangle:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << U << std::endl << std::endl;

            stream << " Product of L and U:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << L * U << std::endl << std::endl;

            Matrix<2, double> diff;
            if (iErr == 1)
            {
                // get row permutation matrix
                solver.getPermutationMatrix(PivotType::Enum::Row, P);

                stream << " Row permutation matrix:" << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << P << std::endl << std::endl;

                diff = P * A - L * U;
                stream << " Difference: PA - LU:" << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << diff  << std::endl << std::endl;
            }
            else
            {
                diff = A - L * U;
                stream << " Difference: A - LU:" << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;
            }

            // test whether or not difference matrix is zero
            auto it = std::find_if(diff.cbegin(), diff.cend(),
                                   [] (auto &&value) { return fabs(value) > 1e-12; });

            stream << " Is the difference matrix nearly zero?";
            if (it == diff.cend())
                stream << " Yes, test PASSED with difference matrix being nearly zero."
                       << std::endl << std::endl;
            else
                stream << " No, test FAILED with non-zero difference matrix."
                       << std::endl << std::endl;

            // test conversion from lower unit-upper factorization to unit-lower upper factorization
            stream << " Test conversion from L(DU) to (LD)U:" << std::endl << std::endl;

            solver.makeLowerUnitUpper(LU);

            // extract the lower and upper triangles
            L = LU.getLowerTriangle();
            U = LU.getUpperTriangle(1);
            U.setDiagonal(1);

            stream << " Lower triangle:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << L << std::endl << std::endl;

            stream << " Upper triangle:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << U << std::endl << std::endl;

            stream << " Product of L and U:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << L * U << std::endl << std::endl;

            if (iErr == 1)
            {
                diff = P * A - L * U;
                stream << " Difference: PA - LU:" << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;
            }
            else
            {
                diff = A - L * U;
                stream << " Difference: A - LU:" << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;
            }

            // test whether or not difference matrix is zero
            it = std::find_if(diff.cbegin(), diff.cend(),
                              [] (auto &&value) { return fabs(value) > 1e-12; });

            stream << " Is the difference matrix nearly zero?";
            if (it == diff.cend())
                stream << " Yes, test PASSED with difference matrix being nearly zero."
                       << std::endl << std::endl;
            else
                stream << " No, test FAILED with non-zero difference matrix."
                       << std::endl << std::endl;

            // reconstruct the compact single-matrix LU representation
            // from the lower and upper triangles
            LU = L + U.getUpperTriangle(1);

            // test conversion back from unit-lower upper factorization
            // to lower unit-upper factorization
            stream << " Test conversion from (LD)U to L(DU):" << std::endl << std::endl;

            solver.makeUnitLowerUpper(LU);

            // extract the lower and upper triangles
            L = LU.getLowerTriangle(-1);
            U = LU.getUpperTriangle();
            L.setDiagonal(1);

            stream << " Lower triangle:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << L << std::endl << std::endl;

            stream << " Upper triangle:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << U << std::endl << std::endl;

            stream << " Product of L and U:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << L * U << std::endl << std::endl;

            if (iErr == 1)
            {
                diff = P * A - L * U;
                stream << " Difference: PA - LU:" << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;
            }
            else
            {
                diff = A - L * U;
                stream << " Difference: A - LU:" << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;
            }

            // test whether or not difference matrix is zero
            it = std::find_if(diff.cbegin(), diff.cend(),
                              [] (auto &&value) { return fabs(value) > 1e-12; });

            stream << " Is the difference matrix nearly zero?";
            if (it == diff.cend())
                stream << " Yes, test PASSED with difference matrix being nearly zero."
                       << std::endl << std::endl;
            else
                stream << " No, test FAILED with non-zero difference matrix."
                       << std::endl << std::endl;
        }

        stream << " Test Doolittle LU factorization update:" << std::endl << std::endl;

        Matrix<2, double> x(i, 1);
        std::generate(x.begin(), x.end(), [&] (void) { return gaussian(generator); });

        Matrix<2, double> y(i, 1);
        std::generate(y.begin(), y.end(), [&] (void) { return gaussian(generator); });

        Matrix<2, double> Aplus = A + x * y.getTranspose();

        stream << "A + xy':" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << Aplus << std::endl << std::endl;

        LU = Aplus;
        iErr = solver.factor(LU);
        if (iErr >= 0)
        {
            // extract the lower and upper triangles
            Matrix<2, double> L = LU.getLowerTriangle(-1);
            Matrix<2, double> U = LU.getUpperTriangle();
            L.setDiagonal(1);

            stream << " Doolittle LU factorization without update:" << std::endl << std::endl;

            stream << " Lower triangle:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << L << std::endl << std::endl;

            stream << " Upper triangle:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << U << std::endl << std::endl;

            stream << " Product of L and U:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << L * U << std::endl << std::endl;

            Matrix<2, double> diff;
            if (iErr == 1)
            {
                // get row permutation matrix
                solver.getPermutationMatrix(PivotType::Enum::Row, P);

                stream << " Row permutation matrix:" << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << P << std::endl << std::endl;

                diff = P * Aplus - L * U;
                stream << " Difference: PA + Pxy' - L * U:" << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;
            }
            else
            {
                diff = Aplus - L * U;
                stream << " Difference: A + xy' - L * U:" << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;
            }

            // test whether or not difference matrix is zero
            auto it = std::find_if(diff.cbegin(), diff.cend(),
                                   [] (auto &&value) { return fabs(value) > 1e-12; });

            stream << " Is the difference matrix nearly zero?";
            if (it == diff.cend())
                stream << " Yes, test PASSED with difference matrix being nearly zero."
                       << std::endl << std::endl;
            else
                stream << " No, test FAILED with non-zero difference matrix."
                       << std::endl << std::endl;
        }

        LU = A;
        iErr = solver.factor(LU);
        if (iErr == -3)
            stream << " Matrix is singular, cannot perform update." << std::endl << std::endl;
        else
        {
            Matrix<2, double> B = LU; // save the factorization
            Matrix<2, double> u(x), v(y);

            if (iErr == 1)
            {
                solver.getPermutationVector(PivotType::Enum::Row, p);
                solver.update(LU, u, v, p);
            }
            else
                solver.update(LU, u, v);

            // extract the lower and upper triangles
            Matrix<2, double> L = LU.getLowerTriangle(-1);
            Matrix<2, double> U = LU.getUpperTriangle();
            L.setDiagonal(1);

            stream << " Doolittle LU factorization with rank-one update (Bennett's Algorithm):"
                   << std::endl << std::endl;

            stream << " Lower triangle:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << L << std::endl << std::endl;

            stream << " Upper triangle:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << U << std::endl << std::endl;

            stream << " Product of L and U:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << L * U << std::endl << std::endl;

            Matrix<2, double> diff;
            if (iErr == 1)
            {
                // get row permutation matrix
                solver.getPermutationMatrix(PivotType::Enum::Row, P);

                diff = P * Aplus - L * U;
                stream << " Difference PA + Pxy' - { L * U | update(LU, x, y, p) }:" << std::endl
                       << std::endl;
                stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;

                // test whether or not difference matrix is zero
                auto it = std::find_if(diff.cbegin(), diff.cend(),
                                       [] (auto &&value) { return fabs(value) > 1e-12; });

                stream << " Is the difference matrix nearly zero?";
                if (it == diff.cend())
                    stream << " Yes, test PASSED with difference matrix being nearly zero."
                           << std::endl << std::endl;
                else
                    stream << " No, test FAILED with non-zero difference matrix."
                           << std::endl << std::endl;

                // extract the lower and upper triangles of original factorization
                L = B.getLowerTriangle(-1);
                U = B.getUpperTriangle();
                L.setDiagonal(1);

                solver.getPermutationVector(PivotType::Enum::Row, p);
                solver.update(L, U, x, y, p);

                Matrix<2, double> Q(i, i);
                solver.getPermutationMatrix(PivotType::Enum::Row, Q);

                stream << " Doolittle LU factorization with pivoted-update "
                       << "(Schwetlick/Kielbasinski algorithm):" << std::endl << std::endl;

                stream << " Lower triangle:" << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << L << std::endl << std::endl;

                stream << " Upper triangle:" << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << U << std::endl << std::endl;

                stream << " Product of L and U:" << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << L * U << std::endl << std::endl;

                diff = Q * Aplus - L * U;
                stream << " Difference (pivoted) PA + Pxy' - { L * U | update(L, U, x, y, p) }:"
                       << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;
            }
            else
            {
                diff = Aplus - L * U;
                stream << " Difference A + xy' - { L * U | update(LU, x, y) }:"
                       << std::endl << std::endl;
                stream << std::setw(15) << std::setprecision(4) << diff
                       << std::endl << std::endl;
            }

            // test whether or not difference matrix is zero
            auto it = std::find_if(diff.cbegin(), diff.cend(),
                                   [] (auto &&value) { return fabs(value) > 1e-12; });

            stream << " Is the difference matrix nearly zero?";
            if (it == diff.cend())
                stream << " Yes, test PASSED with difference matrix being nearly zero."
                       << std::endl << std::endl;
            else
                stream << " No, test FAILED with non-zero difference matrix."
                       << std::endl << std::endl;
        }

        /*
         * Test inverse of matrix using Doolittle LU factorization
         */

        stream << " Test inverse of matrix:" << std::endl << std::endl;
        stream << " Original matrix:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << A << std::endl << std::endl;

        Matrix<2, double> Ainv = Matrix<2, double>::createIdentity(i);
        LU = A;
        iErr = solver.inverse(LU, Ainv);
        if (iErr != -3)
        {
            stream << " Inverse computed using Doolittle LU factorization:"
                   << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << Ainv << std::endl << std::endl;

            Matrix<2, double> diff = A * Ainv;
            stream << " A * Ainv:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;

            diff -= Matrix<2, double>::createIdentity(i);

            // test whether or not difference matrix is zero
            auto it = std::find_if(diff.cbegin(), diff.cend(),
                                   [] (auto &&value) { return fabs(value) > 1e-12; });

            stream << " Is the difference matrix nearly zero?";
            if (it == diff.cend())
                stream << " Yes, test PASSED with difference matrix being nearly zero."
                       << std::endl << std::endl;
            else
                stream << " No, test FAILED with non-zero difference matrix."
                       << std::endl << std::endl;
        }

        /*
         * Test determinant of a matrix using Doolittle LU decomposition
         */

        stream << " Test determinant of the matrix:" << std::endl << std::endl;

        LU = A;
        double d = 0.0;
        iErr = solver.determinant(LU, d);
        if (iErr >= 0)
        {
            Matrix<2, double> LUinv = Ainv;
            double dInv = 0.0;
            iErr = solver.determinant(LUinv, dInv);
            if (iErr >= 0)
            {
                double diff = d * dInv - 1.0;
                stream << " Determinant computed using Doolittle LU factorization: " << d
                       << std::endl << std::endl;
                stream << " Determinant of inverse computed using Doolittle LU factorization: "
                       << dInv << std::endl << std::endl << " Difference |A||A^-1| - 1: "
                       << diff << std::endl << std::endl;

                stream << " Is the difference in both determinants nearly zero?";
                if (fabs(diff) <= 1e-12)
                    stream << " Yes, test PASSED with difference being nearly zero."
                           << std::endl << std::endl;
                else
                    stream << " No, test FAILED with non-zero difference."
                           << std::endl << std::endl;
            }
        }

        if (iErr < 0)
            stream << " Computation of determinant failed (matrix may be nearly singular)"
                   << std::endl << std::endl;

        /*
         * Test solution to Ax = b using Doolittle LU decomposition
         */

        stream << " Test solution to Ax = b using Doolittle LU decomposition."
               << std::endl << std::endl;

        Matrix<2, double> b(i, 1);
        std::generate(b.begin(), b.end(), [&] (void) { return gaussian(generator); });
        x = b;

        stream << " b:" << std::endl << std::endl;
        stream << b << std::endl << std::endl;

        stream << " A:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << A << std::endl << std::endl;

        LU = A;
        iErr = solver.factor(LU, x, b);
        if (iErr == -3)
            stream << " Matrix is singular, cannot solve Ax = b." << std::endl << std::endl;
        else
        {
            stream << " Solution to Ax = b" << std::endl << std::endl;
            stream << x << std::endl << std::endl;

            Matrix<2, double> diff;
            if (iErr == 1)
            {
                // get row permutation matrix
                solver.getPermutationMatrix(PivotType::Enum::Row, P);

                diff = P * A * x - b;
                stream << " Difference PAx - Pb:" << std::endl << std::endl;
                stream << diff << std::endl << std::endl;
            }
            else
            {
                diff = A * x - b;
                stream << " Difference Ax - b:" << std::endl << std::endl;
                stream << diff << std::endl << std::endl;
            }

            // test whether or not difference matrix is zero
            auto it = std::find_if(diff.cbegin(), diff.cend(),
                                   [] (auto &&value) { return fabs(value) > 1e-12; });

            stream << " Is the difference matrix nearly zero?";
            if (it == diff.cend())
                stream << " Yes, test PASSED with difference matrix being nearly zero."
                       << std::endl << std::endl;
            else
                stream << " No, test FAILED with non-zero difference matrix."
                       << std::endl << std::endl;
        }
    }

    return true;
}

}
