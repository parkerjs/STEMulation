#include "permutator.h"
#include "qr.h"
#include "real_matrix_2d.h"
#include "std_vector_overloads.h"
#include "testQR.h"
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
static FactoryRegistrar<UnitTest> unit_test_factory("testQR", &QR_UnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
QR_UnitTest::QR_UnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
QR_UnitTest *QR_UnitTest::create(UnitTestManager *pUnitTestManager)
{
    QR_UnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new QR_UnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool QR_UnitTest::execute(void)
{
    /*
     * Test QR factorization
     */

    std::ofstream stream("outputs/qrFactorTestOutput.dat");

    std::random_device seed;
    std::mt19937 generator(seed());
    std::normal_distribution<double> gaussian(5, 2);
    std::uniform_int_distribution<int> uniform(0, 9);

    stream << " Test QR factorization with column pivoting:" << std::endl << std::endl;

    for (std::size_t i = 1; i <= 10; i++)
    {
        int j = 1 + uniform(generator);
        int k = 1 + uniform(generator);
        int m = std::max(j, k);

        Matrix<2, double> A(j, k);
        std::generate(A.begin(), A.end(), [&] (void) { return gaussian(generator); });

        std::vector<std::size_t> p(k, 0); // column permutation vector
        Matrix<2, double> P(k, k); // column permutation matrix

        stream << " Original matrix:" << std::endl << std::endl;
        stream << std::setw(15) << A << std::endl << std::endl;

        Matrix<2, double> QR = A;
        std::vector<double> u(m);
        QR_Factor<Matrix<2, double>> solver;
        int iErr = solver.factor(QR, u);
        if (iErr >= 0)
        {
            // extract orthogonal and upper triangular matrices
            Matrix<2, double> Q = Matrix<2, double>::createIdentity(j);
            solver.getOrthogonalMatrix(QR, Q, u);
            Matrix<2, double> R = solver.getUpperTriangle(QR);

            stream << " QR factorization:" << std::endl << std::endl;

            stream << " Orthogonal matrix:" << std::endl << std::endl;
            stream << std::setw(15) << Q << std::endl << std::endl;

            stream << " Upper triangle:" << std::endl << std::endl;
            stream << std::setw(15) << R << std::endl << std::endl;

            stream << " Product of Q and R:" << std::endl << std::endl;
            stream << std::setw(15) << Q * R << std::endl << std::endl;

            stream << " Product of Q and Q':" << std::endl << std::endl;
            stream << std::setw(15) << Q * Q.getTranspose() << std::endl << std::endl;

            Matrix<2, double> diff;
            if (iErr == 2)
            {
                // get column permutation matrix
                solver.getPermutationMatrix(PivotType::Enum::Column, P);

                stream << " Column permutation matrix:" << std::endl << std::endl;
                stream << std::setw(15) << P << std::endl << std::endl;

                diff = A * P - Q * R;
                stream << " Difference: AP - QR:" << std::endl << std::endl;
                stream << std::setw(15) << diff  << std::endl << std::endl;
            }
            else
            {
                diff = A - Q * R;
                stream << " Difference: A - QR:" << std::endl << std::endl;
                stream << std::setw(15) << diff << std::endl << std::endl;
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

        stream << " Test QR factorization update:" << std::endl << std::endl;

        Matrix<2, double> x = QR.column(0);
        Matrix<2, double> y = QR.row(0);
        Matrix<2, double> Aplus = A + x * y;

        stream << "A + xy':" << std::endl << std::endl;
        stream << std::setw(15) << Aplus << std::endl << std::endl;

        QR = Aplus;
        iErr = solver.factor(QR, u);
        if (iErr >= 0)
        {
            // extract orthogonal and upper triangular matrices
            Matrix<2, double> Q = Matrix<2, double>::createIdentity(j);
            solver.getOrthogonalMatrix(QR, Q, u);
            Matrix<2, double> R = solver.getUpperTriangle(QR);

            stream << " QR factorization without update:" << std::endl << std::endl;

            stream << " Orthogonal matrix:" << std::endl << std::endl;
            stream << std::setw(15) << Q << std::endl << std::endl;

            stream << " Upper triangle:" << std::endl << std::endl;
            stream << std::setw(15) << R << std::endl << std::endl;

            stream << " Product of Q and R:" << std::endl << std::endl;
            stream << std::setw(15) << Q * R << std::endl << std::endl;

            stream << " Product of Q and Q':" << std::endl << std::endl;
            stream << std::setw(15) << Q * Q.getTranspose() << std::endl << std::endl;

            Matrix<2, double> diff;
            if (iErr == 2 || iErr == 3)
            {
                // get column permutation matrix
                solver.getPermutationMatrix(PivotType::Enum::Column, P);

                stream << " Column permutation matrix:" << std::endl << std::endl;
                stream << std::setw(15) << P << std::endl << std::endl;

                diff = Aplus * P - Q * R;
                stream << " Difference: AP + xy'P - Q * R:" << std::endl << std::endl;
                stream << std::setw(15) << diff << std::endl << std::endl;
            }
            else
            {
                diff = Aplus - Q * R;
                stream << " Difference: A + xy' - Q * R:" << std::endl << std::endl;
                stream << std::setw(15) << diff << std::endl << std::endl;
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

        QR = A;
        iErr = solver.factor(QR, u);

        // extract orthogonal and upper triangular matrices
        Matrix<2, double> Q = Matrix<2, double>::createIdentity(j);
        solver.getOrthogonalMatrix(QR, Q, u);
        Matrix<2, double> R = solver.getUpperTriangle(QR);

        if (iErr >= 0)
        {
            if (iErr == 2 || iErr == 3)
            {
                solver.getPermutationVector(PivotType::Enum::Column, p);
                solver.update(Q, R, x, y, p);
            }
            else
                solver.update(Q, R, x, y);

            stream << " QR factorization with rank-one update:" << std::endl << std::endl;

            stream << " Orthogonal matrix:" << std::endl << std::endl;
            stream << std::setw(15) << Q << std::endl << std::endl;

            stream << " Upper triangle:" << std::endl << std::endl;
            stream << std::setw(15) << R << std::endl << std::endl;

            stream << " Product of Q and R:" << std::endl << std::endl;
            stream << std::setw(15) << Q * R << std::endl << std::endl;

            stream << " Product of Q and Q':" << std::endl << std::endl;
            stream << std::setw(15) << Q * Q.getTranspose() << std::endl << std::endl;

            Matrix<2, double> diff;
            if (iErr == 2 || iErr == 3)
            {
                // get column permutation matrix
                solver.getPermutationMatrix(PivotType::Enum::Column, P);

                diff = Aplus * P - Q * R;
                stream << " Difference AP + xy'P - { Q * R | update(Q, R, x, y, p) }:"
                       << std::endl << std::endl;
                stream << std::setw(15) << diff << std::endl << std::endl;
            }
            else
            {
                diff = Aplus - Q * R;
                stream << " Difference A + xy' - { Q * R | update(Q, R, x, y) }:"
                       << std::endl << std::endl;
                stream << std::setw(15) << diff << std::endl << std::endl;
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
         * Test inverse of matrix using QR factorization
         */

        stream << " Test inverse of a matrix:" << std::endl << std::endl;
        stream << " Original matrix:" << std::endl << std::endl;

        A.resize(i, i, false);
        std::generate(A.begin(), A.end(), [&] (void) { return gaussian(generator); });

        QR = A;
        stream << std::setw(15) << QR << std::endl << std::endl;

        Matrix<2, double> Ainv = Matrix<2, double>::createIdentity(i);
        iErr = solver.inverse(QR, Ainv);
        if (iErr != -3)
        {
            stream << " Inverse computed using QR factorization:" << std::endl << std::endl;
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
         * Test determinant of a matrix using QR decomposition
         */

        stream << " Test determinant of the matrix:" << std::endl << std::endl;

        QR = A;
        double d = 0.0;
        iErr = solver.determinant(QR, d);
        if (iErr >= 0)
        {
            Matrix<2, double> QRinv = Ainv;
            double dInv = 0.0;
            iErr = solver.determinant(QRinv, dInv);
            if (iErr >= 0)
            {
                double diff = d * dInv - 1.0;
                stream << " Determinant computed using QR factorization: " << d
                       << std::endl << std::endl;
                stream << " Determinant of inverse computed using QR factorization: "
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
            stream << " Computation of determinant FAILED (matrix may be nearly singular)"
                   << std::endl << std::endl;

        /*
         * Test solution to Ax = b using QR decomposition
         */

        stream << " Test solution to Ax = b using QR decomposition."
               << std::endl << std::endl;

        Matrix<2, double> b(i, 1);
        std::generate(b.begin(), b.end(), [&] (void) { return gaussian(generator); });
        x = b;

        stream << " b:" << std::endl << std::endl;
        stream << b << std::endl << std::endl;

        stream << " A:" << std::endl << std::endl;
        stream << std::setw(15) << A << std::endl << std::endl;

        iErr = solver.solve(A, x, b);
        if (iErr == -3)
            stream << " FAILED: matrix is singular, cannot solve Ax = b."
                   << std::endl << std::endl;
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
