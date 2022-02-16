#include "jenkins_traub.h"
#include "lowOrderPolynomialSolver.h"
#include "math_constants.h"
#include <algorithm>

namespace math
{

namespace expression
{

namespace polynomial
{

namespace solvers
{

/**
 * Constructor
 * @param tolerance a threshold for root solutions above which iterative methods will be employed to achieve
 *                  the desired accuracy
 */
LowOrderPolynomialSolver::LowOrderPolynomialSolver(double tolerance)
: m_pIterativeSolver(new JenkinsTraub<double>()),
  m_tolerance(tolerance)
{

}

/**
 * Copy constructor
 */
LowOrderPolynomialSolver::LowOrderPolynomialSolver(const LowOrderPolynomialSolver &polynomialSolver)
: m_pIterativeSolver(new JenkinsTraub<double>())
{
    operator = (polynomialSolver);
}

/**
 * Move constructor
 */
LowOrderPolynomialSolver::LowOrderPolynomialSolver(LowOrderPolynomialSolver &&polynomialSolver)
: m_pIterativeSolver(nullptr)
{
    operator = (std::move(polynomialSolver));
}

/**
 * Destructor
 */
LowOrderPolynomialSolver::~LowOrderPolynomialSolver(void)
{

}

/**
 * Copy assignment operator
 */
LowOrderPolynomialSolver &LowOrderPolynomialSolver::operator = (const LowOrderPolynomialSolver &polynomialSolver)
{
    if (&polynomialSolver != this)
    {
        m_tolerance = polynomialSolver.m_tolerance;
    }

    return *this;
}

/**
 * Move assignment operator
 */
LowOrderPolynomialSolver &LowOrderPolynomialSolver::operator = (LowOrderPolynomialSolver &&polynomialSolver)
{
    if (&polynomialSolver != this)
    {
        polynomialSolver.swap(*this);
    }

    return *this;
}

/**
 * Get the name of this class
 */
std::string LowOrderPolynomialSolver::getClassName(void) const
{
    return "LowOrderPolynomialSolver";
}

/**
 * Get the desired root solution tolerance, which defines a threshold above which iterative methods will be
 * employed to achieve the desired accuracy
 */
double LowOrderPolynomialSolver::getTolerance(void) const
{
    return m_tolerance;
}

/**
 * Set the desired root solution tolerance, which defines a threshold above which iterative methods will be
 * employed to achieve the desired accuracy
 */
void LowOrderPolynomialSolver::setTolerance(double tolerance)
{
    m_tolerance = tolerance;
}

/**
 * Function to solve for roots of a generic quadratic polynomial of the following form:
   \verbatim
   p(x) = a * x^2 + b * x + c, where a, b, and c are real coefficients
   \endverbatim
 * @param[out] roots the solution to p(x) = 0
 * @return           an integer value according to the following:
 *                   = 0, if an error occurs (invalid coefficients)
 *                   = 1, if both roots are real
 *                   = 3, if both roots are complex
 */
int LowOrderPolynomialSolver::solve(double a,
                                    double b,
                                    double c,
                                    std::complex<double> roots[2])
{
    return solve(a, b, c, roots[0], roots[1]);
}

/**
 * Function to solve for roots of a generic quadratic polynomial of the following form:
   \verbatim
   p(x) = a * x^2 + b * x + c, where a, b, and c are real coefficients
   \endverbatim
 * @param[out] root1, root2 contain the solution to p(x) = 0
 * @return                  an integer value according to the following:
 *                          = 0, if an error occurs (invalid coefficients)
 *                          = 1, if both roots are real
 *                          = 3, if both roots are complex
 */
int LowOrderPolynomialSolver::solve(double a,
                                    double b,
                                    double c,
                                    std::complex<double> &root1,
                                    std::complex<double> &root2)
{
    int rootsType = RootsType::Unknown;
    if (a != 0.0)
    {
        auto bb = b * b;
        auto ac = a * c;

        // for purposes of numerical stability, determine if delta is zero without performing subtractions
        double deltaNegSum = 0.0, deltaPosSum = bb;
        if (ac > 0.0)
            deltaNegSum = 4 * ac;
        else
            deltaPosSum += std::fabs(4 * ac);

        auto delta = deltaPosSum - deltaNegSum; // discriminant
        auto absDiscriminant = std::fabs(delta);
        double sqrtAbsDiscriminant;
        if (a == 0.0 || c == 0.0)
            sqrtAbsDiscriminant = std::fabs(b);
        else if (b == 0.0)
            sqrtAbsDiscriminant = 2.0 * std::sqrt(std::fabs(a * c));
        else
            sqrtAbsDiscriminant = std::sqrt(absDiscriminant);

        if (deltaPosSum > deltaNegSum)
        {
            // use the 'Citardauq' formula to achieve better numerical stability
            double signumB = (b < 0 ? -1.0 : 1.0);
            auto q = -0.5 * (b + signumB * sqrtAbsDiscriminant);

            root1.real(q / a);
            root1.imag(0.0);
            root2.real(c / q);
            root2.imag(0.0);

            rootsType = RootsType::RealRoots;
        }
        else if (deltaPosSum <= deltaNegSum)
        {
            root1.real(-0.5 * b / a);
            root2.real(root1.real());
            if (deltaPosSum < deltaNegSum)
            {
                root1.imag(0.5 * sqrtAbsDiscriminant / a);
                root2.imag(-root1.imag());

                rootsType = RootsType::ComplexRoots;
            }
            else
                rootsType = RootsType::RealRoots;
        }

        double polyEval[] = { std::abs(c + root1 * (b + root1 * a)),
                              std::abs(c + root2 * (b + root2 * a)) };

        // if desired tolerance wasn't achieved using closed-form solution, then try iterating
        if (polyEval[0] > m_tolerance || polyEval[1] > m_tolerance)
        {
            m_coeff.resize(3);
            m_coeff[0] = a;
            m_coeff[1] = b;
            m_coeff[2] = c;

            m_roots.resize(m_coeff.size() - 1);
            auto numRoots = m_pIterativeSolver->findRoots(m_coeff, m_roots);
            if (numRoots != m_roots.size())
                rootsType = RootsType::Unknown;
            else
            {
                // determine which element has the smallest imaginary component
                root1 = m_roots[0];
                root2 = m_roots[1];

                rootsType = (root1.imag() != 0.0) ? RootsType::ComplexRoots : RootsType::RealRoots;
            }
        }
    }
    else if (b != 0.0)
    {
        root1.real(-c / b);
        root1.imag(0.0);
        root2.real(0.0);
        root2.imag(0.0);

        rootsType = RootsType::RealRoots;
    }

    return rootsType;
}

/**
 * Function to solve for roots of a generic cubic polynomial of the following form:
   \verbatim
   p(x) = a * x^3 + b * x^2 + c * x + d, where a, b, c, and d are real coefficients
   \endverbatim
 * @param[out] roots the solution to p(x) = 0
 * @return           an integer value according to the following:
 *                   = 0, if an error occurs (invalid coefficients)
 *                   = 1, if all roots are real
 *                   = 2, if one root is real and the other two roots are imaginary
 */
int LowOrderPolynomialSolver::solve(double a,
                                    double b,
                                    double c,
                                    double d,
                                    std::complex<double> roots[3])
{
    double root;
    auto rootsType = solve(a, b, c, d, root, roots[1], roots[2]);
    if (rootsType != RootsType::Unknown)
    {
        roots[0].real(root);
        roots[0].imag(0.0);
    }

    return rootsType;
}

/**
 * Function to solve for a single real root of a generic cubic polynomial of the following form:
   \verbatim
   p(x) = a * x^3 + b * x^2 + c * x + d, where a, b, c, and d are real coefficients
   \endverbatim
 * @param[out] root a single real root solution to p(x) = 0
 * @return          an integer value according to the following:
 *                  = 0, if an error occurs (invalid coefficients)
 *                  = 1, upon success
 */
int LowOrderPolynomialSolver::solve(double a,
                                    double b,
                                    double c,
                                    double d,
                                    double &root)
{
    std::complex<double> root2(0.0), root3(0.0);

    auto rootsType = solve(a, b, c, d, root, root2, root3);
    if (rootsType != RootsType::Unknown)
        rootsType = RootsType::RealRoots;

    return rootsType;
}

/**
 * Function to solve for roots of a generic cubic polynomial of the following form:
   \verbatim
   p(x) = a * x^3 + b * x^2 + c * x + d, where a, b, c, and d are real coefficients
   \endverbatim
 * @param[out] root1, root2, root3 contain the solution to p(x) = 0
 * @return                         an integer value according to the following:
 *                                 = 0, if an error occurs (invalid coefficients)
 *                                 = 1, if all roots are real
 *                                 = 2, if one root is real and the other two roots are imaginary
 */
int LowOrderPolynomialSolver::solve(double a,
                                    double b,
                                    double c,
                                    double d,
                                    double &root1,
                                    std::complex<double> &root2,
                                    std::complex<double> &root3)
{
    auto rootsType = RootsType::Unknown;
    if (a != 0.0)
    {
        auto a2 = b / a, a1 = c / a, a0 = d / a;
        double oneThird = 1.0 / 3.0, aTwoOverThree = oneThird * a2;
        auto q = oneThird * (a1 - a2 * aTwoOverThree);
        auto r = (0.5 * (a1 * a2 - 3.0 * a0) - oneThird * a2 * a2 * aTwoOverThree) / 3.0;
        auto delta = q * q * q + r * r;
        if (delta >= 0.0)
        {
            auto sqrtDelta = std::sqrt(delta);
            auto rPlusSqrtDelta = r + sqrtDelta;
            auto rMinusSqrtDelta = r - sqrtDelta;
            double rPlusSqrtDeltaSignum = rPlusSqrtDelta < 0.0 ? -1.0 : 1.0;
            double rMinusSqrtDeltaSignum = rMinusSqrtDelta < 0.0 ? -1.0 : 1.0;
            auto s1 = rPlusSqrtDeltaSignum * std::pow(std::fabs(rPlusSqrtDelta), oneThird);
            auto s2 = rMinusSqrtDeltaSignum * std::pow(std::fabs(rMinusSqrtDelta), oneThird);
            auto sum = s1 + s2;
            root1 = sum - aTwoOverThree;
            if (delta == 0.0)
            {
                root2.imag(0.0);
                root2.real(-0.5 * sum - aTwoOverThree);
                root3 = root2;

                rootsType = RootsType::RealRoots;
            }
            else
            {
                root2.real(-0.5 * sum - aTwoOverThree);
                root2.imag(0.5 * SQRT_THREE * (s1 - s2));
                root3.real(root2.real());
                root3.imag(-root2.imag());

                rootsType = RootsType::RealAndComplexRoots;
            }
        }
        else
        {
            auto &&sqrtDelta  = std::sqrt(std::complex<double>(delta));
            auto &&s1 = std::pow(r + sqrtDelta, oneThird);
            auto &&s2 = std::pow(r - sqrtDelta, oneThird);
            auto sum(s1 + s2), diff(s1 - s2);
            root1 = s1.real() + s2.real() - aTwoOverThree;

            std::complex<double> sqrtMinusThreeOverTwo(0, 0.5 * SQRT_THREE);
            root2.real((-0.5 * sum - aTwoOverThree + sqrtMinusThreeOverTwo * diff).real());
            root3.real((-0.5 * sum - aTwoOverThree - sqrtMinusThreeOverTwo * diff).real());

            rootsType = RootsType::RealRoots;
        }

        double polyEval[] = { std::abs(d + root1 * (c + root1 * (b + root1 * a))),
                              std::abs(d + root2 * (c + root2 * (b + root2 * a))),
                              std::abs(d + root3 * (c + root3 * (b + root3 * a))) };

        // if desired tolerance wasn't achieved using closed-form solution, then try iterating
        if (polyEval[0] > m_tolerance || polyEval[1] > m_tolerance || polyEval[2] > m_tolerance)
        {
            m_coeff.resize(4);
            m_coeff[0] = a;
            m_coeff[1] = b;
            m_coeff[2] = c;
            m_coeff[3] = d;

            m_roots.resize(m_coeff.size() - 1);
            auto numRoots = m_pIterativeSolver->findRoots(m_coeff, m_roots);
            if (numRoots != m_roots.size())
                rootsType = RootsType::Unknown;
            else
            {
                // determine which element has the smallest imaginary component
                int j = (std::abs(m_roots[0].imag()) < std::abs(m_roots[1].imag())) ? 0 : 1;
                j = (std::abs(m_roots[j].imag()) < std::abs(m_roots[2].imag())) ? j : 2;

                root1 = m_roots[j].real();
                root2 = m_roots[(++j) % 3];
                root3 = m_roots[(++j) % 3];

                rootsType = (root2.imag() != 0.0) ? RootsType::RealAndComplexRoots : RootsType::RealRoots;
            }
        }
    }
    else
    {
        root1 = 0.0;

        return solve(b, c, d, root2, root3);
    }

    return rootsType;
}

/**
 * Function to solve for roots of a generic quartic polynomial of the following form:
   \verbatim
   p(x) = a * x^4 + b * x^3 + c * x^2 + d * x + e, where a, b, c, d, and e are real coefficients
   \endverbatim
 * @param[out] roots the solution to p(x) = 0
 * @return           an integer value according to the following:
 *                   = 0, if an error occurs (invalid coefficients)
 *                   = 1, if all roots are real
 *                   = 2, if two roots are real and two roots are complex conjugates
 *                   = 3, if the roots are two pairs of complex conjugates
 */
int LowOrderPolynomialSolver::solve(double a,
                                    double b,
                                    double c,
                                    double d,
                                    double e,
                                    std::complex<double> roots[4])
{
    return solve(a, b, c, d, e, roots[0], roots[1], roots[2], roots[3]);
}

/**
 * Function to solve for roots of a generic quartic polynomial of the following form:
   \verbatim
   p(x) = a * x^4 + b * x^3 + c * x^2 + d * x + e, where a, b, c, d, and e are real coefficients
   \endverbatim
 * @param[out] root1, root2, root3, root4 contain the solution to p(x) = 0
 * @return                                an integer value according to the following:
 *                                        = 0, if an error occurs (invalid coefficients)
 *                                        = 1, if all roots are real
 *                                        = 2, if two roots are real and two roots are complex conjugates
 *                                        = 3, if the roots are two pairs of complex conjugates
 */
int LowOrderPolynomialSolver::solve(double a,
                                    double b,
                                    double c,
                                    double d,
                                    double e,
                                    std::complex<double> &root1,
                                    std::complex<double> &root2,
                                    std::complex<double> &root3,
                                    std::complex<double> &root4)
{
    int rootsType = RootsType::Unknown;
    if (a != 0.0)
    {
        // normalize the coefficients in this equation so that
        // the leading coefficient of the highest term is one
        auto leadCoefficient = a;
        auto a3 = b / leadCoefficient;
        auto a2 = c / leadCoefficient;
        auto a1 = d / leadCoefficient;
        auto a0 = e / leadCoefficient;

        // solve for a single real root of the resolvent cubic equation
        double aa = a3 * a3, root;
        rootsType = solve(1.0, -a2, a3 * a1 - 4 * a0, -aa * a0 + 4 * a2 * a0 - a1 * a1, root);
        if (rootsType != RootsType::Unknown)
        {
            auto rSq = 0.25 * aa - a2 + root;
            auto term1 = 0.75 * aa - 2 * a2 - rSq, term3 = a3 * a2 - 2 * a1 - 0.25 * aa * a3;
            std::complex<double> f(0.0, 0.0), g(0.0, 0.0), r(0.0, 0.0);
            if (rSq != 0.0)
            {
                auto rMag = std::sqrt(std::fabs(rSq));
                if (rSq < 0.0)
                {
                    f.real(term1), f.imag(term3 / rMag);
                    g.real(f.real()), g.imag(-f.imag());
                    f = std::sqrt(f), g = std::sqrt(g);
                    r.imag(rMag);
                }
                else
                {
                    auto dSq = term1 + term3 / rMag;
                    auto fMag = std::sqrt(std::fabs(dSq));
                    if (dSq > 0.0)
                        f.real(fMag);
                    else
                        f.imag(fMag);

                    auto gSq = term1 - term3 / rMag;
                    auto gMag = std::sqrt(std::fabs(gSq));
                    if (gSq > 0.0)
                        g.real(gMag);
                    else
                        g.imag(gMag);

                    r.real(rMag);
                }
            }
            else
            {
                term3 = root * root - 4 * a0;
                auto term2 = 2 * std::sqrt(std::fabs(term3));
                if (term3 < 0.0)
                {
                    f.imag(term2), g.imag(-f.imag());
                    f = std::sqrt(f);
                    g = std::sqrt(g);
                }
                else
                {
                    f.real(std::sqrt(f.real() + term2));
                    g.real(std::sqrt(g.real() - term2));
                }
            }

            root1 = -0.5 * (0.5 * a3 - r - f);
            root2 = -0.5 * (0.5 * a3 - r + f);
            root3 = -0.5 * (0.5 * a3 + r - g);
            root4 = -0.5 * (0.5 * a3 + r + g);
        }

        double polyEval[] = { std::abs(e + root1 * (d + root1 * (c + root1 * (b + root1 * a)))),
                              std::abs(e + root2 * (d + root2 * (c + root2 * (b + root2 * a)))),
                              std::abs(e + root3 * (d + root3 * (c + root3 * (b + root3 * a)))),
                              std::abs(e + root4 * (d + root4 * (c + root4 * (b + root4 * a)))) };

        // if desired tolerance wasn't achieved using closed-form solution, then try iterating
        if (polyEval[0] > m_tolerance || polyEval[1] > m_tolerance ||
            polyEval[2] > m_tolerance || polyEval[3] > m_tolerance)
        {
            m_coeff.resize(5);
            m_coeff[0] = a;
            m_coeff[1] = b;
            m_coeff[2] = c;
            m_coeff[3] = d;
            m_coeff[4] = e;

            m_roots.resize(m_coeff.size() - 1);
            auto numRoots = m_pIterativeSolver->findRoots(m_coeff, m_roots);
            if (numRoots == (int)m_roots.size())
            {
                root1 = m_roots[0];
                root2 = m_roots[1];
                root3 = m_roots[2];
                root4 = m_roots[3];
            }
        }

        std::complex<double> *roots[] = { &root1, &root2, &root3, &root4 };
        rootsType = RootsType::Unknown;
        for (int i = 0; i < 4; ++i)
            if (roots[i]->imag() != 0.0)
                ++rootsType;

        rootsType = std::max(rootsType, (int)RootsType::RealRoots);
        rootsType = std::min(rootsType, (int)RootsType::ComplexRoots);
    }
    else
    {
        root1.imag(0.0), root1.real(0.0), root2.imag(0.0);

        double root;
        rootsType = solve(b, c, d, e, root, root3, root4);
        root2.real(root);
    }

    return rootsType;
}

/**
 * Swap function
 */
void LowOrderPolynomialSolver::swap(LowOrderPolynomialSolver &polynomialSolver)
{
    m_coeff.swap(polynomialSolver.m_coeff);
    m_pIterativeSolver.swap(polynomialSolver.m_pIterativeSolver);
    m_roots.swap(polynomialSolver.m_roots);
    std::swap(m_tolerance, polynomialSolver.m_tolerance);
}

}

}

}

}
