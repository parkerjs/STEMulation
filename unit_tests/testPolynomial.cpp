#include "polynomial.h"
#include "testPolynomial.h"
#include "unitTestManager.h"
#include <complex>
#include <random>

// using namespace declarations
using namespace attributes::abstract;
using namespace math::expression::polynomial;
using namespace math::number_systems::complex;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testPolynomial", &PolynomialUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
PolynomialUnitTest::PolynomialUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
PolynomialUnitTest *PolynomialUnitTest::create(UnitTestManager *pUnitTestManager)
{
    PolynomialUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new PolynomialUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * unit test for polynomial class
 */
bool PolynomialUnitTest::execute(void)
{
    bool bSuccess = true; // assume success

    /*
     * test polynomial operations
     */

    auto norm = [] (auto vec)
    {
        auto result = 0.0;
        for (size_t i = 0; i < vec.size(); ++i)
            result = std::abs(vec[i]) * std::abs(vec[i]);

        return std::sqrt(result);
    };

    std::default_random_engine randomNumberGenerator;
    std::uniform_int_distribution<int> uniform(0, 10);

    std::cout << "Starting unit test for polynomial operations..." << std::endl << std::endl;

    auto crand = [&randomNumberGenerator] ()
    {
        static std::normal_distribution<double> gaussian(0, 1);

        auto real = gaussian(randomNumberGenerator);
        auto imag = gaussian(randomNumberGenerator);

        return std::complex<double>(real, imag);
    };

    // test complex polynomials first from degree 0 up to 10
    for (unsigned int i = 0; i < 500; ++i)
    {
        auto order = uniform(randomNumberGenerator);
        std::vector<std::complex<double>> coeff(1 + order);
        std::generate(coeff.begin(), coeff.end(), crand);

        // introduce some zero coefficients
        for (int j = 0; j <= order; ++j)
            if (uniform(randomNumberGenerator) < 4)
                coeff[j] = 0.0;

        Polynomial<std::complex<double>> poly(coeff);

        // print the polynomial.
        std::cout << "Polynomial for iteration # " << i + 1 << ":" << std::endl << std::endl;
        std::cout << std::setprecision(5) << poly << std::endl << std::endl;

        if (order > 0)
        {
            // compute the roots of this polynomial
            std::vector<std::complex<double>> roots(order);
            auto nRoots = poly.roots(roots);
            if (nRoots > 0)
            {
                std::cout << "Computed roots of this polynomial:" << std::endl << std::endl;
                for (unsigned int j = 0; j < nRoots; ++j)
                    std::cout << Complex<double>(roots[j]) << std::endl;

                std::cout << std::endl << "Evaluate the polynomial at each of the computed roots:"
                          << std::endl << std::endl << "Are the computed roots possible zeros of P(x)?"
                          << std::endl << std::endl;

                // evaluate the polynomial at the computed roots to determine if they are indeed the roots of
                // the polynomial
                std::vector<std::complex<double>> eval(nRoots);
                for (unsigned int j = 0; j < nRoots; ++j)
                {
                    eval[j] = poly.evaluate(roots[j]);
                    std::cout << "P(x) evaluated at x = " << std::setw(20) << Complex<double>(roots[j]) << " -> "
                              << Complex<double>(eval[j]) << std::endl;
                }

                std::cout << std::endl;

                auto n = norm(eval);
                bSuccess &= (std::fabs(n) <= 1.e-4);
                if (bSuccess)
                    std::cout << "Yes, test PASSED with 2-norm of difference being " << std::fabs(n) << "."
                              << std::endl << std::endl;
                else
                {
                    std::cout << "No, test FAILED." << std::endl << std::endl;
                    break;
                }
            }
        }

        // test ability to take derivative of polynomial
        auto polyDeriv = poly.derivative();

        std::cout << "Derivative of the polynomial:" << std::endl << std::endl;
        std::cout << polyDeriv << std::endl << std::endl;

        // save the 0-order coefficient
        auto c = poly[order];

        // now take the anti-derivative of polyDeriv
        auto antiDiffpolyDeriv = polyDeriv.antiDerivative(c);

        std::cout << "Anti-derivative of the derivative of the polynomial with constant of integration c = "
                  << c << ":" << std::endl << std::endl << antiDiffpolyDeriv << std::endl << std::endl;

        std::cout << "Does the anti-derivative of the derivative equal that of the original?";

        // does the anti-derivative equal the original?
        bSuccess &= (antiDiffpolyDeriv == poly);
        if (bSuccess)
            std::cout << " Yes, test PASSED." << std::endl << std::endl;
        else
        {
            auto n = norm((antiDiffpolyDeriv - poly).getCoefficients());
            bSuccess = (std::fabs(n) <= 1.e-4);
            if (bSuccess)
                std::cout << " Yes, test PASSED with 2-norm of difference being " << std::fabs(n) << "."
                          << std::endl << std::endl;
            else
            {
                std::cout << " No, test FAILED." << std::endl << std::endl;
                break;
            }
        }

        // now differentiate the actual object rather than return a copy
        poly.differentiate();

        std::cout << "Does output of Polynomial::derivative() equal that of Polynomial::differentiate()?";

        // does polyDeriv equal poly?
        bSuccess &= (polyDeriv == poly);
        if (bSuccess)
            std::cout << " Yes, test PASSED." << std::endl << std::endl;
        else
        {
            auto n = norm((polyDeriv - poly).getCoefficients());
            bSuccess = (std::fabs(n) <= 1e-12);
            if (bSuccess)
                std::cout << " Yes, test PASSED with 2-norm of difference being " << std::fabs(n) << "."
                          << std::endl << std::endl;
            else
            {
                std::cout << " No, test FAILED." << std::endl << std::endl;
                break;
            }
        }

        // now anti-differentiate the actual object rather than return a copy
        polyDeriv.antiDifferentiate(c);

        std::cout << "Does output of Polynomial::antiDerivative() equal that of Polynomial::antiDifferentiate()?";

        // does polyDeriv equal poly?
        bSuccess &= (polyDeriv == antiDiffpolyDeriv);
        if (bSuccess)
            std::cout << " Yes, test PASSED." << std::endl << std::endl;
        else
        {
            auto n = norm((polyDeriv - poly).getCoefficients());
            bSuccess = (std::fabs(n) < 1.e-4);
            if (bSuccess)
                std::cout << " Yes, test PASSED with 2-norm of difference being " << std::fabs(n) << "."
                          << std::endl << std::endl;
            else
            {
                std::cout << " No, test FAILED." << std::endl << std::endl;
                break;
            }
        }

        std::cout << "Test addition of two polynomials:" << std::endl << std::endl;

        // test addition, product and subtraction of polynomials
        auto orderOne = uniform(randomNumberGenerator) >> 1;
        auto orderTwo = uniform(randomNumberGenerator) >> 1;

        Polynomial<std::complex<double>> polyOne(orderOne), polyTwo(orderTwo);

        auto &&polyOneCoeff = polyOne.getCoefficients();
        std::generate(polyOneCoeff.begin(), polyOneCoeff.end(), crand);

        auto &&polyTwoCoeff = polyTwo.getCoefficients();
        std::generate(polyTwoCoeff.begin(), polyTwoCoeff.end(), crand);

        // introduce some zero coefficients
        for (int j = 0; j <= orderOne; ++j)
            if (uniform(randomNumberGenerator) < 4)
                polyOne[j] = 0.0;

        for (int j = 0; j <= orderTwo; ++j)
            if (uniform(randomNumberGenerator) <4)
                polyTwo[j] = 0.0;

        // print the polynomials
        std::cout << "Polynomial #1:" << std::endl << std::endl;
        std::cout << std::setprecision(5) << polyOne << std::endl << std::endl;
        std::cout << "Polynomial #2:" << std::endl << std::endl;
        std::cout << std::setprecision(5) << polyTwo << std::endl << std::endl;

        // test sum of polynomials
        auto polySum = polyOne + polyTwo;

        std::cout << "Sum of polynomials:" << std::endl << std::endl;
        std::cout << std::setprecision(5) << polySum << std::endl << std::endl;

        // is the sum correct?
        std::cout << "Does Psum(x) - P1(x) - P2(x) yield approximately zero?" << std::endl << std::endl;

        auto polySumMinusPolyOneAndPolyTwo = polySum - polyOne - polyTwo;
        std::cout << "Psum(x) - P1(x) - P2(x) = " << polySumMinusPolyOneAndPolyTwo << std::endl << std::endl;

        auto n = norm(polySumMinusPolyOneAndPolyTwo.getCoefficients());
        bSuccess &= (std::fabs(n) < 1.e-4);
        if (bSuccess)
            std::cout << "Yes, test PASSED with 2-norm of difference being " << std::fabs(n) << "."
                      << std::endl << std::endl;
        else
        {
            std::cout << "No, test FAILED." << std::endl << std::endl;
            break;
        }

        // test difference of polynomials
        auto polyDiff = polyOne - polyTwo;

        std::cout << "Difference of polynomials:" << std::endl << std::endl;
        std::cout << std::setprecision(5) << polyDiff << std::endl << std::endl;

        // is the difference correct?
        std::cout << "Does Pdiff(x) - P1(x) + P2(x) yield approximately zero?" << std::endl << std::endl;

        auto polyDiffMinusPolyOnePlusPolyTwo = polyDiff - polyOne + polyTwo;
        std::cout << "Psum(x) - P1(x) + P2(x) = " << polyDiffMinusPolyOnePlusPolyTwo << std::endl << std::endl;

        n = norm (polyDiffMinusPolyOnePlusPolyTwo.getCoefficients());
        bSuccess &= (std::fabs(n) < 1.e-4);
        if (bSuccess)
            std::cout << "Yes, test PASSED with 2-norm of difference being " << std::fabs(n) << "."
                      << std::endl << std::endl;
        else
        {
            std::cout << "No, test FAILED." << std::endl << std::endl;
            break;
        }

        // test add-assign
        Polynomial<std::complex<double>> polyOneOrig(polyOne);
        polyOne += polyTwo;

        std::cout << "Sum of polynomials using add-assign:" << std::endl << std::endl;
        std::cout << std::setprecision(5) << polyOne << std::endl << std::endl;

        // is the sum correct?
        std::cout << "Does output of Polynomial::addAssign() equal that of Polynomial::add()?"
                  << std::endl << std::endl;

        Polynomial<std::complex<double>> polyAddAssignDiff = polySum - polyOne;
        std::cout << "PaddAssign(x) - P1(x) = " << polyAddAssignDiff << std::endl << std::endl;

        n = norm(polyAddAssignDiff.getCoefficients());
        bSuccess &= (std::fabs(n) < 1.e-4);
        if (bSuccess)
            std::cout << "Yes, test PASSED with 2-norm of difference being " << std::fabs(n) << "."
                      << std::endl << std::endl;
        else
        {
            std::cout << "No, test FAILED." << std::endl << std::endl;
            break;
        }

        // test subtract-assign
        polyOneOrig -= polyTwo;

        std::cout << "Difference of polynomials using subtract-assign:" << std::endl << std::endl;
        std::cout << std::setprecision(5) << polyOne << std::endl << std::endl;

        // is the difference correct?
        std::cout << "Does output of Polynomial::subtractAssign() equal that of Polynomial::subtract()?"
                  << std::endl << std::endl;

        Polynomial<std::complex<double>> polyDiffAssignDiff = polyDiff - polyOneOrig;
        std::cout << "PdiffAssign(x) - P1(x) = " << polyDiffAssignDiff << std::endl << std::endl;

        n = norm(polyAddAssignDiff.getCoefficients());
        bSuccess &= (std::fabs(n) < 1.e-4);
        if (bSuccess)
            std::cout << "Yes, test PASSED with 2-norm of difference being " << std::fabs(n) << "."
                      << std::endl << std::endl;
        else
        {
            std::cout << "No, test FAILED." << std::endl << std::endl;
            break;
        }

        // test multiplication of two polynomials
        auto polyLeftOrder = 1 + (uniform(randomNumberGenerator) >> 1);
        auto polyRightOrder = 1 + (uniform(randomNumberGenerator) >> 1);

        std::vector<std::complex<double>> coeffLeft(1 + polyLeftOrder);
        std::vector<std::complex<double>> coeffRight(1 + polyRightOrder);

        std::generate(coeffLeft.begin(), coeffLeft.end(), crand);
        std::generate(coeffRight.begin(), coeffRight.end(), crand);

        // introduce some zero coefficients
        for (int j = 1; j <= polyLeftOrder; ++j)
            if (uniform(randomNumberGenerator) < 4)
                coeffLeft[j] = 0.0;

        // introduce some zero coefficients
        for (int j = 1; j <= polyRightOrder; ++j)
            if (uniform(randomNumberGenerator) < 4)
                coeffRight[j] = 0.0;

        Polynomial<std::complex<double>> polyLeft(coeffLeft);
        Polynomial<std::complex<double>> polyRight(coeffRight);
        Polynomial<std::complex<double>> dividend(polyLeft);
        Polynomial<std::complex<double>> divisor(polyRight);

        // print the polynomials
        std::cout << "Left polynomial:" << std::endl << std::endl;
        std::cout << std::setprecision(5) << polyLeft << std::endl << std::endl;

        std::cout << "Right polynomial:" << std::endl << std::endl;
        std::cout << std::setprecision(5) << polyRight << std::endl << std::endl;

        // multiply the polynomials
        auto product = polyLeft * polyRight;

        std::cout << "Product of the polynomials:" << std::endl << std::endl;
        std::cout << std::setprecision(5) << product << std::endl << std::endl;

        std::vector<std::complex<double>> rootsLeft(polyLeftOrder), rootsRight(polyRightOrder);
        auto nRootsLeft = polyLeft.roots(rootsLeft);
        auto nRootsRight = polyRight.roots(rootsRight);

        std::cout << "Computed roots of left polynomial:" << std::endl << std::endl;

        for (unsigned int j = 0; j < nRootsLeft; ++j)
            std::cout << Complex<double>(rootsLeft[j]) << std::endl;

        std::cout << std::endl;
        std::cout << "Computed roots of right polynomial:" << std::endl << std::endl;

        for (unsigned int j = 0; j < nRootsRight; ++j)
            std::cout << Complex<double>(rootsRight[j]) << std::endl;

        std::cout << std::endl;

        std::vector<std::complex<double>> rootsProduct(nRootsLeft + nRootsRight);
        auto nRoots = product.roots(rootsProduct);

        std::cout << "Computed roots of multiplied polynomial:" << std::endl << std::endl;

        for (unsigned int j = 0; j < nRoots; ++j)
            std::cout << Complex<double>(rootsProduct[j]) << std::endl;

        std::cout << std::endl;

        std::vector<std::complex<double>> combinedRoots(rootsLeft);
        combinedRoots.insert(combinedRoots.end(), rootsRight.begin(), rootsRight.end());

        // now sort the root vectors
        std::sort(combinedRoots.begin(), combinedRoots.end(),
                  [] (const std::complex<double> &a, const std::complex<double> &b) { return a.real() < b.real(); });
        std::sort(rootsProduct.begin(), rootsProduct.end(),
                  [] (const std::complex<double> &a, const std::complex<double> &b) { return a.real() < b.real(); });

        // is the difference correct?
        std::cout << "Do the roots of the product of the polynomials equal the combination of the "
                  << "roots from the individual polynomials?" << std::endl << std::endl;

        // does the combination of the two sets of roots belonging respectively to the left and right
        // polynomials equal the roots belonging to the product of the polynomials?
        bSuccess &= (combinedRoots == rootsProduct);
        if (bSuccess)
            std::cout << "Yes, test PASSED." << std::endl << std::endl;
        else
        {
            std::vector<std::complex<double>> difference(nRoots);
            std::transform(combinedRoots.begin(), combinedRoots.end(), rootsProduct.begin(),
                           difference.begin(), std::minus<std::complex<double>>());

            n = norm(difference);
            bSuccess = (std::fabs(n) <= 1.e-4);
            if (bSuccess)
                std::cout << "Yes, test PASSED with 2-norm of difference being " << std::fabs(n) << "."
                          << std::endl << std::endl;
            else
            {
                std::cout << "No, test FAILED." << std::endl << std::endl;
                break;
            }
        }

        // test multiply-assign
        polyLeft *= polyRight;

        std::cout << "Multiplication of polynomials using multiply-assign:" << std::endl << std::endl;
        std::cout << std::setprecision(5) << polyLeft << std::endl << std::endl;

        // is the product correct?
        std::cout << "Does output of Polynomial::multiplyAssign() equal that of Polynomial::multiply()?"
                  << std::endl << std::endl;

        Polynomial<std::complex<double>> productAssignDiff = product - polyLeft;
        std::cout << "PmultiplyAssign(x) - Pleft(x) = " << productAssignDiff << std::endl << std::endl;

        n = norm(productAssignDiff.getCoefficients());
        bSuccess &= (std::fabs(n) < 1.e-4);
        if (bSuccess)
            std::cout << "Yes, test PASSED with 2-norm of difference being " << std::fabs(n) << "."
                      << std::endl << std::endl;
        else
        {
            std::cout << "No, test FAILED." << std::endl << std::endl;
            break;
        }

        // test division of two polynomials
        std::cout << "Dividend:" << std::endl << std::endl;
        std::cout << std::setprecision(5) << dividend << std::endl << std::endl;

        std::cout << "Divisor:" << std::endl << std::endl;
        std::cout << std::setprecision(5) << divisor << std::endl << std::endl;

        // divide the polynomials
        auto &&quotient = dividend / divisor;
        auto &&remainder = dividend % divisor;

        std::cout << "Quotient resulting from division of the polynomials:" << std::endl << std::endl;
        std::cout << std::setprecision(5) << quotient << std::endl << std::endl;

        std::cout << "Remainder resulting from division of the polynomials:" << std::endl << std::endl;
        std::cout << std::setprecision(5) << remainder << std::endl << std::endl;

        // given dividend f(x), divisor d(x), quotient q(x), and remainder r(x), is the following
        // relationship satisfied?
        std::cout << "Do both the computed quotient and remainder, when multiplied by the divisor, "
                  << "yield the original dividend?" << std::endl << std::endl;

        n = norm(quotient.getCoefficients());
        Polynomial<std::complex<double>> recoveredDividend;
        if (std::fabs(n) < 1.e-4)
            recoveredDividend = remainder;
        else
            recoveredDividend = quotient * divisor + remainder;

        bSuccess &= (recoveredDividend == dividend);
        if (bSuccess)
            std::cout << "Yes, test PASSED." << std::endl << std::endl;
        else
        {
            std::vector<std::complex<double>> difference(1 + dividend.getOrder());
            auto &&recoveredDividendCoeffs = recoveredDividend.getCoefficients();
            auto &&dividendCoeffs = dividend.getCoefficients();
            std::transform(recoveredDividendCoeffs.begin(), recoveredDividendCoeffs.end(),
                           dividendCoeffs.begin(), difference.begin(), std::minus<std::complex<double>>());

            n = norm(difference);
            bSuccess = (std::fabs(n) <= 1.e-4);
            if (bSuccess)
                std::cout << "Yes, test PASSED with 2-norm of difference being " << std::fabs(n) << "."
                          << std::endl << std::endl;
            else
            {
                std::cout << "No, test FAILED." << std::endl << std::endl;
                break;
            }
        }
    }

    return bSuccess;
}

}
