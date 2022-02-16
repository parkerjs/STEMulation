#ifndef BEZIER_CURVE_H
#define BEZIER_CURVE_H

#include "bernstein.h"
#include "cloneable.h"
#include "reflective.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include <iostream>
#include <vector>

namespace math
{

namespace curves
{

/**
 * This class contains code to generate a Bezier curve of degree N for a specified set of control points
 */
template<typename T, size_t N>
class BezierCurve final
: public attributes::interfaces::Cloneable<BezierCurve<T, N>>,
  virtual private attributes::abstract::Reflective,
  public attributes::concrete::StaticMutexMappable<BezierCurve<T, N>, int, std::mutex *>,
  public attributes::concrete::StaticSynchronizable<BezierCurve<T, N>>
{
public:

    /**
     * Constructor
     */
    BezierCurve(void)
    : m_pBernsteinPolynomial(new expression::polynomial::BernsteinPolynomial<T, N>())
    {

    }

    /**
     * Copy constructor
     */
    BezierCurve(const BezierCurve<T, N> &curve)
    : m_pBernsteinPolynomial(nullptr)
    {
        operator = (curve);
    }

    /**
     * Move constructor
     */
    BezierCurve(BezierCurve<T, N> &&curve)
    : m_pBernsteinPolynomial(nullptr)
    {
        operator = (std::move(curve));
    }

    /**
     * Destructor
     */
    virtual ~BezierCurve(void) override
    {
        if (m_pBernsteinPolynomial != nullptr)
            delete m_pBernsteinPolynomial;
    }

    /**
     * Copy assignment operator
     */
    BezierCurve<T, N> &operator = (const BezierCurve<T, N> &curve)
    {
        if (&curve != this)
        {
            if (m_pBernsteinPolynomial != nullptr)
            {
                delete m_pBernsteinPolynomial;
                m_pBernsteinPolynomial = nullptr;
            }

            if (curve.m_pBernsteinPolynomial != nullptr)
                m_pBernsteinPolynomial = curve.m_pBernsteinPolynomial->clone();
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    BezierCurve<T, N> &operator = (BezierCurve<T, N> &&curve)
    {
        if (&curve != this)
        {
            if (m_pBernsteinPolynomial != nullptr)
            {
                delete m_pBernsteinPolynomial;
                m_pBernsteinPolynomial = nullptr;
            }

            m_pBernsteinPolynomial = std::move(curve.m_pBernsteinPolynomial);
            curve.m_pBernsteinPolynomial = nullptr;
        }

        return *this;
    }

    /**
     * Calculate a Bezier curve of degree N for the given control points
     * @param[in]  x    an array of control points used to calculate the Bernstein polynomial
     * @param[out] y    will be populated with the calculated Bezier curve; the size of the y vector will
     *                  determine the number of points for which a value will be calculated
     * @param      size the number of points which will be calculated along the curve and stored in the y
     *                  vector
     */
    inline virtual bool calculate(T (&x)[N + 1], std::vector<T> &y, size_t size) final
    {
        y.resize(size);

        return calculate(x, x + N + 1, &y[0], &y[0] + size);
    }

    /**
     * Calculate a Bezier curve of degree N for the given control points
     * @param[in]  x         a vector of control points used to calculate the Bernstein polynomial; its size
     *                       must be one plus the degree of the Bezier curve (N+1)
     * @param[out] y         will be populated with the calculated Bezier curve; the size of the y vector will
     *                       determine the number of points for which a value will be calculated
     * @param      numPoints the number of points which will be calculated along the curve
     */
    inline virtual bool calculate(const std::vector<T> &x, std::vector<T> &y, size_t numPoints) final
    {
        y.resize(numPoints);

        return calculate(x.cbegin(), x.cend(), y.begin(), y.end());
    }

    /**
     * Calculate a Bezier curve of degree N for the given control points
     * @param[in]  x an array of control points used to calculate the Bernstein polynomial
     * @param[out] y will be populated with the calculated Bezier curve; the size of the y vector will
     *               determine the number of points for which a value will be calculated
     */
    inline virtual bool calculate(T (&x)[N + 1], std::vector<T> &y) final
    {
        return calculate(x, x + N + 1, &y[0], &y[0] + y.size());
    }

    /**
     * Calculate a Bezier curve of degree N for the given control points
     * @param[in]  x a vector of control points used to calculate the Bernstein polynomial; its size must be
     *               one plus the degree of the Bezier curve (N+1)
     * @param[out] y will be populated with the calculated Bezier curve; the size of the y vector will
     *               determine the number of points for which a value will be calculated
     */
    inline virtual bool calculate(const std::vector<T> &x, std::vector<T> &y) final
    {
        return calculate(x.cbegin(), x.cend(), y.begin(), y.end());
    }

    /**
     * Calculate a Bezier curve of degree N for the given control points
     * @param it_xBegin an iterator (or pointer) to the beginning of a vector of control points; its size must
     *                  be the one plus the degree of the Bezier curve (N+1)
     * @param it_xEnd   an iterator (or pointer) to the end of a vector of control points (if there are N
     *                  elements [0 ... N-1], the iterator (or pointer) refers to the element that would follow
     *                  element N-1)
     * @param it_yBegin an iterator (or pointer) to the beginning of a vector that will store the calculated
     *                  Bezier curve
     * @param it_yEnd   an iterator (or pointer) to the end of a vector that will store the calculated Bezier
     *                  curve (if there are N elements [0 ... N-1], the iterator (or pointer) refers to the
     *                  element that would follow element N-1)
     */
    template<typename input_iterator, typename output_iterator> inline typename
    std::enable_if<!std::is_pointer<input_iterator>::value || !std::is_pointer<output_iterator>::value, bool>::type
    calculate(input_iterator it_xBegin, input_iterator it_xEnd, output_iterator it_yBegin, output_iterator it_yEnd)
    {
        return calculate(&it_xBegin[0], &it_xBegin[0] + std::distance(it_xBegin, it_xEnd),
                         &it_yBegin[0], &it_yBegin[0] + std::distance(it_yBegin, it_yEnd));
    }

    /**
     * Calculate a Bezier curve of degree N for the given control points
     * @param p_xBegin a pointer to the beginning of a vector of control points; its size must be one plus the
     *                 degree of the Bezier curve (N+1)
     * @param p_xEnd   a pointer to the end of a vector of control points
     * @param p_yBegin a pointer to the beginning of a vector that will store the calculated Bezier curve
     * @param p_yEnd   a pointer to the end of a vector that will store the calculated Bezier curve
     */
    virtual bool calculate(const T *p_xBegin, const T *p_xEnd, T *p_yBegin, T *p_yEnd) final
    {
        bool bSuccess = (std::distance(p_xBegin, p_xEnd) == N + 1);
        if (bSuccess)
        {
            auto &&numPoints = std::distance(p_yBegin, p_yEnd);
            bSuccess = (numPoints > 1);
            if (bSuccess)
            {
                auto &&dt = 1.0 / (numPoints - 1);
                for (int i = 0; i < numPoints; ++i)
                {
                    p_yBegin[i] = T(0);
                    for (size_t j = 0; j <= N; ++j)
                    {
                        auto &&basis = m_pBernsteinPolynomial->evaluate(j, i * dt);
                        p_yBegin[i] += basis * p_xBegin[j];
                    }
                }
            }
            else
            {
                this->lock(0);
                std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                          << "The number of points in the output vector must be greater than one."
                          << std::endl;
                this->unlock(0);
            }
        }
        else
        {
            this->lock(0);
            std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                      << "A Bezier curve of degree " << N << " needs " << N + 1 << " control points."
                      << std::endl;
            this->unlock(0);
        }

        return bSuccess;
    }

    /**
     * Calculate a Bezier curve of degree N using the specified table of Bernstein basis polynomials evaluated
     * on the interval t = [0, 1]; The number of points that will be calculated on the curve will be determined
     * by the number of rows present in the table
     * @param table    a 2d database of Bernstein basis polynomials, where each row contains the basis
     *                 polynomials evaluated at a single point on the interval t = [0, 1]
     * @param p_xBegin a pointer to the beginning of a vector of control points; its size must be one plus the
     *                 degree of the Bezier curve (N+1)
     * @param p_xEnd   a pointer to the end of a vector of control points
     * @param p_yBegin a pointer to the beginning of a vector that will store the calculated Bezier curve
     * @param p_yEnd   a pointer to the end of a vector that will store the calculated Bezier curve
     */
    bool calculate(const std::vector<std::vector<T>> &table, const T *p_xBegin, const T *p_xEnd, T *p_yBegin,
                   T *p_yEnd)
    {
        bool bSuccess = (std::distance(p_xBegin, p_xEnd) == N + 1);
        if (bSuccess)
        {
            auto &&numPoints = table.size();
            bSuccess = (numPoints > 1 && numPoints == std::distance(p_yBegin, p_yEnd));
            if (bSuccess)
            {
                auto &&dt = 1.0 / (numPoints - 1);
                for (int i = 0; i < numPoints; ++i)
                {
                    p_yBegin[i] = T(0);
                    for (int j = 0; j <= N; ++j)
                    {
                        p_yBegin[i] += table[i][j] * p_xBegin[j];
                    }
                }
            }
            else
            {
                this->lock(0);
                std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                          << "The number of rows in the table must be greater than one and the number of "
                          << "points in the output vector must match the number of rows in the table."
                          << std::endl;
                this->unlock(0);
            }
        }
        else
        {
            this->lock(0);
            std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                      << "A Bezier curve of degree " << N << " needs " << N + 1 << " control points."
                      << std::endl;
            this->unlock(0);
        }

        return bSuccess;
    }

    /**
     * Calculate a table of Bernstein basis polynomials as a function of t
     * @param      numPoints the number of points at which the Bernstein polynomials will be evaluated on the
     *                       interval t = [0, 1]
     * @param[out] table     populated with a 2d database of Bernstein basis polynomials, where each row
     *                       contains the basis polynomials evaluated at a single point on the interval t = [0,
     *                       1]
     */
    inline virtual bool
    calculateBernsteinBasisPolynomials(size_t numPoints, std::vector<std::vector<T>> &table) const final
    {
        bool bSuccess = (numPoints > 1);
        if (bSuccess)
        {
            auto &&dt = 1.0 / (numPoints - 1);
            table.resize(numPoints);
            for (size_t i = 0; i < numPoints; ++i)
            {
                for (size_t j = 0; j <= N; ++j)
                {
                    table.resize(N + 1);
                    table[i][j] = m_pBernsteinPolynomial->evaluate(j, i * dt);
                }
            }
        }
        else
        {
            this->lock(0);
            std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                      << "The number of rows in the table must be greater than one." << std::endl;
            this->unlock(0);
        }

        return bSuccess;
    }

    /**
     * clone() function
     */
    inline virtual BezierCurve<T, N> *clone(void) const override
    {
        return new BezierCurve<T, N>(*this);
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "BezierCurve";
    }

private:

    /**
     * Bernstein polynomial object
     */
    expression::polynomial::BernsteinPolynomial<T, N> *m_pBernsteinPolynomial;
};

}

}

#endif
