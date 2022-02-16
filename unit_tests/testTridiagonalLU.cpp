#include "matrix.h"
#include "qr.h"
#include "std_vector_overloads.h"
#include "testTridiagonalLU.h"
#include "tridiag_lu.h"
#include "unitTestManager.h"
#include <fstream>
#include <random>

// using namespace declarations
using namespace attributes::abstract;
using namespace math;
using namespace math::linear_algebra::matrix;
using namespace math::linear_algebra::matrix::decomposition;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testTridiagonalLU", &TridiagonalLU_UnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
TridiagonalLU_UnitTest::TridiagonalLU_UnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
TridiagonalLU_UnitTest *TridiagonalLU_UnitTest::create(UnitTestManager *pUnitTestManager)
{
    TridiagonalLU_UnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new TridiagonalLU_UnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool TridiagonalLU_UnitTest::execute(void)
{
    /*
     * test tridiagonal LU factorization
     */

    std::ofstream stream("outputs/tridiagonalLU_FactorTestOutput.dat");

    std::random_device seed;
    std::mt19937 generator(seed());
    std::normal_distribution<double> gaussian(5, 2);

    stream << " Test tridiagonal LU factorization (no pivoting):" << std::endl << std::endl;

    for (std::size_t i = 1; i <= 10; i++)
    {
        Matrix<2, double> A(i, i);
        std::generate(A.begin(), A.end(), [&] (void) { return gaussian(generator); });

        // Keep only the tridiagonal of matrix A
        A.removeLowerAndUpperTriangles();

        stream << " Original matrix:" << std::endl << std::endl;
        stream << std::setw(15) << A << std::endl << std::endl;

        Matrix<2, double> LU = A;
        Tridiag_LU_Factor<Matrix<2, double>> solver;
        int iErr = solver.factor(LU);
        if (iErr >= 0)
        {
            // extract the lower and upper triangles
            Matrix<2, double> L = LU.getLowerTriangle();
            Matrix<2, double> U = LU.getUpperTriangle(1);
            U.setDiagonal(1);

            stream << " Tridiagonal LU factorization:" << std::endl << std::endl;

            stream << " Lower triangle:" << std::endl << std::endl;
            stream << std::setw(15) << L << std::endl << std::endl;

            stream << " Upper triangle:" << std::endl << std::endl;
            stream << std::setw(15) << U << std::endl << std::endl;

            stream << " Product of L and U:" << std::endl << std::endl;
            stream << std::setw(15) << L * U << std::endl << std::endl;

            Matrix<2, double> diff = A - L * U;
            stream << " Difference: A - LU:" << std::endl << std::endl;
            stream << std::setw(15) << diff  << std::endl << std::endl;

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
            stream << " Test conversion from (LD)U to L(DU):" << std::endl << std::endl;

            solver.makeUnitLowerUpper(LU);

            // extract the lower and upper triangles
            L = LU.getLowerTriangle(-1);
            U = LU.getUpperTriangle();
            L.setDiagonal(1);

            stream << " Lower triangle:" << std::endl << std::endl;
            stream << std::setw(15) << L << std::endl << std::endl;

            stream << " Upper triangle:" << std::endl << std::endl;
            stream << std::setw(15) << U << std::endl << std::endl;

            stream << " Product of L and U:" << std::endl << std::endl;
            stream << std::setw(15) << L * U << std::endl << std::endl;

            diff = A - L * U;
            stream << " Difference: A - LU:" << std::endl << std::endl;
            stream << std::setw(15) << diff  << std::endl << std::endl;

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
            LU = L.getLowerTriangle(-1) + U;

            // test conversion back from unit-lower upper factorization
            // to lower unit-upper factorization
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

            diff = A - L * U;
            stream << " Difference: A - LU:" << std::endl << std::endl;
            stream << std::setw(15) << std::setprecision(4) << diff  << std::endl << std::endl;

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

        /*
         * test inverse of matrix using tridiagonal LU factorization
         */

        stream << " Test inverse of matrix:" << std::endl << std::endl;
        stream << " Original matrix:" << std::endl << std::endl;
        stream << std::setw(15) << A << std::endl << std::endl;

        Matrix<2, double> Ainv = Matrix<2, double>::createIdentity(i);
        LU = A;
        iErr = solver.inverse(LU, Ainv);
        if (iErr != -3)
        {
            stream << " Inverse computed using tridiagonal LU factorization:"
                   << std::endl << std::endl;
            stream << std::setw(15) << Ainv << std::endl << std::endl;

            Matrix<2, double> diff = A * Ainv;
            stream << " A * Ainv:" << std::endl << std::endl;
            stream << std::setw(15) << diff << std::endl << std::endl;

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
         * test determinant of a matrix using Tridiagonal LU decomposition
         */

        stream << " Test determinant of the matrix:" << std::endl << std::endl;

        LU = A;
        double d = 0.0;
        iErr = solver.determinant(LU, d);
        if (iErr >= 0)
        {
            Matrix<2, double> LUinv = Ainv;
            double dInv = 0.0;

            // inverse of a tridiagonal matrix is no longer tridiagonal, determine the
            // determinant using QR decomposition
            QR_Factor<Matrix<2, double>> solver;
            iErr = solver.determinant(LUinv, dInv);
            if (iErr >= 0)
            {
                double diff = d * dInv - 1.0;
                stream << " Determinant computed using Tridiagonal LU factorization: " << d
                       << std::endl << std::endl;
                stream << " Determinant of inverse computed using Tridiagonal LU factorization: "
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
         * test solution to Ax = b using tridiagonal LU decomposition
         */

        stream << " Test solution to Ax = b using tridiagonal LU decomposition."
               << std::endl << std::endl;

        Matrix<2, double> b(i, 1);
        std::generate(b.begin(), b.end(), [&] (void) { return gaussian(generator); });
        Matrix<2, double> x = b;

        stream << " b:" << std::endl << std::endl;
        stream << b << std::endl << std::endl;

        stream << " A:" << std::endl << std::endl;
        stream << std::setw(15) << A << std::endl << std::endl;

        LU = A;
        iErr = solver.factor(LU, x, b);
        if (iErr == -3)
            stream << " Matrix is singular, cannot solve Ax = b." << std::endl << std::endl;
        else
        {
            stream << " Solution to Ax = b" << std::endl << std::endl;
            stream << x << std::endl << std::endl;

            Matrix<2, double> diff = A * x - b;
            stream << " Difference Ax - b:" << std::endl << std::endl;
            stream << diff << std::endl << std::endl;

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
