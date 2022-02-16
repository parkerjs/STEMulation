#include "cholesky.h"
#include "matrix.h"
#include "std_vector_overloads.h"
#include "testCholesky.h"
#include "unitTestManager.h"
#include <random>

// using namespace declarations
using namespace attributes::abstract;
using namespace math;
using namespace math::linear_algebra::matrix;
using namespace math::linear_algebra::matrix::decomposition;
using namespace math::linear_algebra::vector;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testCholesky", &CholeskyUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
CholeskyUnitTest::CholeskyUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
CholeskyUnitTest *CholeskyUnitTest::create(UnitTestManager *pUnitTestManager)
{
    CholeskyUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new CholeskyUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool CholeskyUnitTest::execute(void)
{
    /*
     * Test Cholesky factorization
     */

    std::ofstream stream("outputs/choleskyFactorTestOutput.dat");

    stream << " Test Cholesky factorization:" << std::endl << std::endl;

    std::random_device seed;
    std::mt19937 generator(seed());
    std::normal_distribution<double> gaussian(5, 2);

    for (std::size_t i = 1; i <= 8; i++)
    {
        Matrix<2, double> A(i, i);
        std::generate(A.begin(), A.end(), [&] (void) { return gaussian(generator); });
        A.removeUpperTriangle(1);

        A = A * A.getTranspose();

        stream << " Original matrix:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << A << std::endl << std::endl;

        // test Cholesky factorization
        Matrix<2, double> B = A; // save A before factoring
        Cholesky_Factor<Matrix<2, double>> solver;
        solver.factor(A);

        stream << " Cholesky factorization:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << A << std::endl << std::endl;

        Matrix<2, double> LLT = A * A.getTranspose();
        A = B;

        stream << " Product of matrix square roots:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << LLT << std::endl << std::endl;

        Matrix<2, double> diff = A - LLT;
        stream << " Difference: A - U' * U:" << std::endl << std::endl;
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

        // test Cholesky factorization with update
        stream << " Test Cholesky factorization update:" << std::endl << std::endl;

        Matrix<2, double> x(i, 1);
        std::generate(x.begin(), x.end(), [&] (void) { return gaussian(generator); });
        Matrix<2, double> Aup = A + x * x.getTranspose();

        stream << "A + x * x':" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << Aup << std::endl << std::endl;

        solver.factor(Aup);

        stream << " Factorization without update (A + x * x'):" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << Aup << std::endl << std::endl;

        LLT = Aup * Aup.getTranspose();

        stream << " Product of matrix square roots:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << LLT << std::endl << std::endl;

        diff = A + x * x.getTranspose() - LLT;
        stream << " Difference: A + x * x' - U' * U:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;

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

        Matrix<2, double> C = A;
        solver.factor(C);
        solver.update(C, x);

        stream << " Factorization using update:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << C << std::endl << std::endl;

        LLT = C * C.getTranspose();

        stream << " Product of matrix square roots:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << LLT << std::endl << std::endl;

        diff = C - Aup;
        stream << " Difference cholesky(A + x * x') - update(A, x):" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;

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

        // test Cholesky factorization with down-date
        x *= 0.1; // x needs to be small when performing a down-date...
        Aup = A - x * x.getTranspose();

        stream << "A - x * x':" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << Aup << std::endl << std::endl;

        solver.factor(Aup);

        stream << " Factorization without downdate (A - x * x'):" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << Aup << std::endl << std::endl;

        LLT = Aup * Aup.getTranspose();

        stream << " Product of matrix square roots:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << LLT << std::endl << std::endl;

        diff = A - x * x.getTranspose() - LLT;
        stream << " Difference: A - x * x' - U' * U:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;

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

        C = A;
        solver.factor(C);
        solver.downdate(C, x);

        stream << " Factorization using downdate:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << C << std::endl << std::endl;

        LLT = C * C.getTranspose();

        stream << " Product of matrix square roots:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << LLT << std::endl << std::endl;

        diff = C - Aup;
        stream << " Difference cholesky(A - x * x') - downdate(A, x):" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;

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

        /*
         * Test inverse of positive symmetric definite matrix using Cholesky factorization
         */

        stream << " Test inverse of positive symmetric definite matrix:" << std::endl << std::endl;
        stream << " Original matrix:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << A << std::endl << std::endl;

        Matrix<2, double> Ainv = Matrix<2, double>::createIdentity(i);
        solver.inverse(B, Ainv);

        stream << " Inverse computed using Cholesky factorization:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << Ainv << std::endl << std::endl;

        diff = A * Ainv;
        stream << " A * Ainv:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << diff << std::endl << std::endl;

        diff -= Matrix<2, double>::createIdentity(i);

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

        /*
         * Test determinant of a matrix using Cholesky decomposition
         */

        stream << " Test determinant of the matrix:" << std::endl << std::endl;

        LLT = A;
        double d = 0.0;
        int iErr = solver.determinant(LLT, d);
        if (iErr >= 0)
        {
            Matrix<2, double> LLTinv = Ainv;
            double dInv = 0.0;
            iErr = solver.determinant(LLTinv, dInv);
            if (iErr >= 0)
            {
                double diff = d * dInv - 1.0;
                stream << " Determinant computed using Cholesky factorization: " << d
                       << std::endl << std::endl;
                stream << " Determinant of inverse computed using Cholesky factorization: "
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
         * Test solution to Ax = b using Cholesky decomposition
         */

        stream << " Test solution to Ax = b using Cholesky decomposition."
               << std::endl << std::endl;

        Matrix<2, double> b(i, 1);
        std::generate(b.begin(), b.end(), [&] (void) { return gaussian(generator); });
        x = b;

        stream << " b:" << std::endl << std::endl;
        stream << b << std::endl << std::endl;

        stream << " A:" << std::endl << std::endl;
        stream << std::setw(15) << std::setprecision(4) << A << std::endl << std::endl;

        solver.solve(A, x, b);

        stream << " Solution to Ax = b" << std::endl << std::endl;
        stream << x << std::endl << std::endl;

        diff = A * x - b;
        stream << " Difference Ax - b:" << std::endl << std::endl;
        stream << diff << std::endl << std::endl;

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

    return true;
}

}
