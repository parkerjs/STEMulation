#ifndef LOW_ORDER_POLYNOMIAL_SOLVER_H
#define LOW_ORDER_POLYNOMIAL_SOLVER_H

#include "export_library.h"
#include "reflective.h"
#include "swappable.h"
#include <complex>
#include <memory>
#include <vector>

namespace math
{

namespace expression
{

namespace polynomial
{

namespace solvers
{

// forward declarations
template<typename> class JenkinsTraub;

/**
 * Class for finding the roots, possibly complex, of low-order polynomial equations having real coefficients
 */
class LowOrderPolynomialSolver
: virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<LowOrderPolynomialSolver>
{
public:

    /**
     * Enumerations
     */
    enum RootsType { ComplexRoots = 3, RealAndComplexRoots = 2, RealRoots = 1, Unknown = 0 };

    /**
     * Constructor
     * @param tolerance a threshold for root solutions above which iterative methods will be employed to
     *                  achieve the desired accuracy
     */
    EXPORT_STEM LowOrderPolynomialSolver(double tolerance = 1.e-6);

    /**
     * Copy constructor
     */
    EXPORT_STEM LowOrderPolynomialSolver(const LowOrderPolynomialSolver &polynomialSolver);

    /**
     * Move constructor
     */
    EXPORT_STEM LowOrderPolynomialSolver(LowOrderPolynomialSolver &&polynomialSolver);

    /**
     * Destructor
     */
    virtual EXPORT_STEM ~LowOrderPolynomialSolver(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM LowOrderPolynomialSolver &operator = (const LowOrderPolynomialSolver &polynomialSolver);

    /**
     * Move assignment operator
     */
    EXPORT_STEM LowOrderPolynomialSolver &operator = (LowOrderPolynomialSolver &&polynomialSolver);

    /**
     * Evaluate the polynomial at the specified abscissa using Horner's method
     * @param coeff the coefficients of the polynomial, where the first coefficient corresponds to the highest
     *              degree and the last coefficient corresponds to the lowest degree
     * @param x     the abscissa for which the polynomial will be evaluated
     */
    template<typename T>
    T evaluate(const std::vector<double> &coeff,
               const T &x)
    {
        T y = 0.0;
        if (!coeff.empty())
        {
            y = coeff[0];
            auto n = coeff.size();
            for (std::size_t i = 1; i < n; ++i)
                y = coeff[i] + x * y;
        }

        return y;
    }

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the desired root solution tolerance, which defines a threshold above which iterative methods will be
     * employed to achieve the desired accuracy
     */
    EXPORT_STEM double getTolerance(void) const;

    /**
     * Set the desired root solution tolerance, which defines a threshold above which iterative methods will be
     * employed to achieve the desired accuracy
     */
    EXPORT_STEM void setTolerance(double tolerance);

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
    EXPORT_STEM int solve(double a,
                          double b,
                          double c,
                          std::complex<double> roots[2]);

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
    EXPORT_STEM int solve(double a,
                          double b,
                          double c,
                          std::complex<double> &root1,
                          std::complex<double> &root2);

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
    EXPORT_STEM int solve(double a,
                          double b,
                          double c,
                          double d,
                          std::complex<double> roots[3]);

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
    EXPORT_STEM int solve(double a,
                          double b,
                          double c,
                          double d,
                          double &root);

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
    EXPORT_STEM int solve(double a,
                          double b,
                          double c,
                          double d,
                          double &root1,
                          std::complex<double> &root2,
                          std::complex<double> &root3);

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
    EXPORT_STEM int solve(double a,
                          double b,
                          double c,
                          double d,
                          double e,
                          std::complex<double> roots[4]);

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
    EXPORT_STEM int solve(double a,
                          double b,
                          double c,
                          double d,
                          double e,
                          std::complex<double> &root1,
                          std::complex<double> &root2,
                          std::complex<double> &root3,
                          std::complex<double> &root4);

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(LowOrderPolynomialSolver &polynomialSolver) override final;

private:

    /**
     * workspace vector to store polynomial coefficients when iteration is necessary
     */
    std::vector<double> m_coeff;

    /**
     * pointer to iterative polynomial solver
     */
    std::unique_ptr<JenkinsTraub<double>> m_pIterativeSolver;

    /**
     * workspace vector to store roots when iteration is necessary
     */
    std::vector<std::complex<double>> m_roots;

    /**
     * tolerance defines a threshold for root solutions above which iterative methods will be employed to
     * achieve the desired accuracy
     */
    double m_tolerance;
};

}

}

}

}

#endif
