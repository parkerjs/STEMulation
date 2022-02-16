#ifndef JENKINS_TRAUB_H
#define JENKINS_TRAUB_H

#include "math_constants.h"
#include "reflective.h"
#include "swappable.h"
#include <cmath>
#include <complex>
#include <iostream>
#include <limits>
#include <mutex>
#include <vector>

namespace math
{

namespace expression
{

namespace polynomial
{

namespace solvers
{

/**
 * Class for computing the roots of a polynomial with complex coefficients using the Jenkins-Traub algorithm.
 * This source is a translation of TOMS493 from FORTRAN to C++.
 */
template<typename T>
class JenkinsTraub final
: virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<JenkinsTraub<T>>
{
public:

    /**
     * Constructor
     */
    JenkinsTraub(void)
    : m_base(std::numeric_limits<T>::radix),
      m_eta(std::numeric_limits<T>::epsilon()),
      m_infinity(std::numeric_limits<T>::max()),
      m_smalno(std::numeric_limits<T>::min()),
      m_are(T(0)),
      m_mre(T(0)),
      m_pvi(T(0)),
      m_pvr(T(0)),
      m_si(T(0)),
      m_sr(T(0)),
      m_ti(T(0)),
      m_tr(T(0)),
      m_n(0)
    {

    }

    /**
     * Copy constructor
     */
    JenkinsTraub(const JenkinsTraub<T> &jenkinsTraub)
    : m_n(0)
    {
        operator = (jenkinsTraub);
    }

    /**
     * Move constructor
     */
    JenkinsTraub(JenkinsTraub<T> &&jenkinsTraub)
    : m_n(0)
    {
        jenkinsTraub.swap(*this);
    }

    /**
     * Destructor
     */
    virtual ~JenkinsTraub(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    JenkinsTraub<T> &operator = (const JenkinsTraub<T> &jenkinsTraub)
    {
        if (&jenkinsTraub != this)
        {
            m_are = jenkinsTraub.m_are;
            m_base = jenkinsTraub.m_base;
            m_eta = jenkinsTraub.m_eta;
            m_hi = jenkinsTraub.m_hi;
            m_hr = jenkinsTraub.m_hr;
            m_infinity = jenkinsTraub.m_infinity;
            m_mre = jenkinsTraub.m_mre;
            m_n = jenkinsTraub.m_n;
            m_pi = jenkinsTraub.m_pi;
            m_pr = jenkinsTraub.m_pr;
            m_pvi = jenkinsTraub.m_pvi;
            m_pvr = jenkinsTraub.m_pvr;
            m_qhi = jenkinsTraub.m_qhi;
            m_qhr = jenkinsTraub.m_qhr;
            m_qpi = jenkinsTraub.m_qpi;
            m_qpr = jenkinsTraub.m_qpr;
            m_shi = jenkinsTraub.m_shi;
            m_shr = jenkinsTraub.m_shr;
            m_si = jenkinsTraub.m_si;
            m_smalno = jenkinsTraub.m_smalno;
            m_sr = jenkinsTraub.m_sr;
            m_ti = jenkinsTraub.m_ti;
            m_tr = jenkinsTraub.m_tr;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    JenkinsTraub<T> &operator = (JenkinsTraub<T> &&jenkinsTraub)
    {
        if (&jenkinsTraub != this)
        {
            jenkinsTraub.swap(*this);
        }

        return *this;
    }

private:

    /**
     * Allocate vectors
     */
    virtual void allocate(std::size_t size) const final
    {
        if (m_pr.size() != size)
        {
            m_pi.resize(size);
            m_pr.resize(size);
            m_hi.resize(size);
            m_hr.resize(size);
            m_qpi.resize(size);
            m_qpr.resize(size);
            m_qhi.resize(size);
            m_qhr.resize(size);
            m_shi.resize(size);
            m_shr.resize(size);
        }
    }

    /**
     * Bounds the error in evaluating the polynomial by the Horner recurrence.
     * @param qr, qi   are the partial sums
     * @param ms       the modulus of the point
     * @param mp       the modulus of polynomial
     * @param are, mre are error bounds on complex addition and multiplication
     */
    inline T virtual boundError(const std::vector<T> &qr, const std::vector<T> &qi, const T &ms, const T &mp,
                                const T &are, const T &mre) const final
    {
        auto e = complexModulus(qr[0], qi[0]) * mre / (are + mre);
        for (int i = 0; i <= m_n; ++i)
            e = e * ms + complexModulus(qr[i], qi[i]);

        return e * (are + mre) - mp * mre;
    }

    /**
     * Computes t = -p(s) / h(s). Returns true if h(s) is essentially zero.
     */
    virtual bool calcT(void) const final
    {
        T hvr, hvi;
        auto nm1 = m_n - 1;
        evaluatePolynomial(nm1, m_sr, m_si, m_hr, m_hi, m_qhr, m_qhi, hvr, hvi); // evaluate h(s)

        bool bH_isNearZero = complexModulus(hvr, hvi) <= m_are * 10 * complexModulus(m_hr[nm1], m_hi[nm1]);
        if (!bH_isNearZero)
            complexDivide(-m_pvr, -m_pvi, hvr, hvi, m_tr, m_ti);
        else
        {
            m_tr = 0.0;
            m_ti = 0.0;
        }

        return bH_isNearZero;
    }

    /**
     * Cauchy computes a lower bound on the moduli of the zeros of a polynomial
     * @param pt the modulus of the coefficients.
     */
    virtual T cauchy(std::vector<T> &pt, std::vector<T> &q) const final
    {
        pt[m_n] = -pt[m_n];

        // compute upper estimate bound
        auto nm1 = m_n - 1;
        T x = std::exp(log(-pt[m_n]) - std::log(pt[0])) / m_n, xm;
        if (pt[nm1] != 0)
        {
            // Newton step at the origin is better, use it
            xm = -pt[m_n] / pt[nm1];
            if (xm < x)
                x = xm;
        }

        // chop the interval (0, x) until f < 0
        for (;;)
        {
            xm = x * 0.1;
            auto f = pt[0];
            for (int i = 1; i <= m_n; ++i)
                f = f * xm + pt[i];

            if (f <= 0)
                break;

            x = xm;
        }

        // do Newton iteration until x converges to two decimal places
        for (auto dx = x; std::abs(dx / x) > 0.005;)
        {
            q[0] = pt[0];
            for (int i = 1; i <= m_n; ++i)
                q[i] = q[i - 1] * x + pt[i];

            auto &&f = q[m_n];
            auto df = q[0];

            for (int i = 1; i < m_n; ++i)
                df = df * x + q[i];

            dx = f / df;
            x -= dx;
        }

        return x;
    }

    /**
     * Complex division c = a / b, avoiding overflow.
     */
    virtual void complexDivide(const T &ar, const T &ai, const T &br, const T &bi, T &cr, T &ci) const final
    {
        if (br == 0.0 && bi == 0.0) // division by zero, c = infinity
        {
            cr = m_infinity;
            ci = m_infinity;
        }
        else if (std::abs(br) < std::abs(bi))
        {
            auto &&r = br / bi;
            auto &&d = bi + r * br;
            cr = (ar * r + ai) / d;
            ci = (ai * r - ar) / d;
        }
        else
        {
            auto &&r = bi / br;
            auto &&d = br + r * bi;
            cr = (ar + ai * r) / d;
            ci = (ai - ar * r) / d;
        }
    }

    /**
     * Modulus of a complex number avoiding overflow.
     */
    inline virtual T complexModulus(const T &re, const T &im) const final
    {
        auto &&ar = std::abs(re);
        auto &&ai = std::abs(im);

        if (ar < ai)
            return ai * std::sqrt(1.0 + std::pow(ar / ai, 2.0));
        else if (ar > ai)
            return ar * std::sqrt(1.0 + std::pow(ai / ar, 2.0));
        else
            return ar * SQRT_TWO;
    }

    /**
     * Main subroutine
     */
    virtual int computeRoots(const std::vector<std::complex<T>> &coeff, int deg,
                             std::vector<std::complex<T>> &zeros) const final
    {
        m_are = m_eta;
        m_mre = 2.0 * SQRT_TWO * m_eta;
        auto xx = 0.5 * SQRT_TWO;
        auto yy = -xx;
        auto &&cosr = -0.06975647374412530077595883519414;
        auto &&sinr = -0.99756405025982424761316268064426;
        m_n = deg;

        // algorithm fails if the leading coefficient is zero
        if (coeff[0].real() == 0.0 && coeff[0].imag() == 0.0)
            return -1;

        // allocate arrays
        allocate(deg + 1);

        // remove the zeros at the origin, if any
        while (coeff[m_n].real() == 0.0 && coeff[m_n].imag() == 0.0)
        {
            auto idnn2 = deg - m_n;
            zeros[idnn2].real(0.0);
            zeros[idnn2].imag(0.0);
            --m_n;
        }

        // make a copy of the coefficients
        for (int i = 0; i <= m_n; ++i)
        {
            m_pr[i] = coeff[i].real();
            m_pi[i] = coeff[i].imag();
            m_shr[i] = complexModulus(m_pr[i], m_pi[i]);
        }

        // scale the polynomial
        auto bound = scale(m_shr);
        if (bound != 1.0)
        {
            for (int i = 0; i <= m_n; ++i)
            {
                m_pr[i] *= bound;
                m_pi[i] *= bound;
            }
        }

        while (m_n > 1)
        {
            // calculate bound, a lower bound on the modulus of the zeros
            for (int i = 0; i <= m_n; ++i)
                m_shr[i] = complexModulus(m_pr[i], m_pi[i]);

            bound = cauchy(m_shr, m_shi);

            // outer loop to control 2 major passes with different sequences of shifts
            bool bConv = false;
            for (int cnt1 = 1; !bConv && cnt1 <= 2; ++cnt1)
            {
                // first stage calculation, no shift
                noShift(5);

                // inner loop to select a shift
                for (int cnt2 = 1; !bConv && cnt2 <= 9; ++cnt2)
                {
                    // shift is chosen with modulus bound and amplitude rotated by 94 degrees from the
                    // previous shift
                    auto &&xxx = cosr * xx - sinr * yy;
                    yy = sinr * xx + cosr * yy;
                    xx = xxx;
                    m_sr = bound * xx;
                    m_si = bound * yy;

                    // second stage calculation, fixed shift
                    T zr, zi;
                    bConv = fixedShift(10 * cnt2, zr, zi);
                    if (bConv)
                    {
                        // the second stage jumps directly to the third stage iteration. If successful the
                        // zero is stored and the polynomial is deflated
                        auto idnn2 = deg - m_n;
                        zeros[idnn2].real(zr);
                        zeros[idnn2].imag(zi);
                        --m_n;
                        for (int i = 0; i <= m_n; ++i)
                        {
                            m_pr[i] = m_qpr[i];
                            m_pi[i] = m_qpi[i];
                        }
                    }
                }
            }

            if (!bConv)
            {
                // the zero-finder has failed on two major passes. Return empty handed with the number of roots
                // found (less than the original degree)
                return deg - m_n;
            }
        }

        T zr, zi;

        complexDivide(-m_pr[1], -m_pi[1], m_pr[0], m_pi[0], zr, zi);

        zeros[deg - 1].real(zr);
        zeros[deg - 1].imag(zi);

        return deg;
    }

    /**
     * Evaluates a polynomial p at s by the Horner recurrence, placing the partial sums in q and the computed
     * value in pv.
     */
    virtual void evaluatePolynomial(int nn, const T &sr, const T &si, const std::vector<T> &pr,
                                    const std::vector<T> &pi, std::vector<T> &qr, std::vector<T> &qi,
                                    T &pvr, T &pvi) const final
    {
        qr[0] = pr[0];
        qi[0] = pi[0];
        pvr = qr[0];
        pvi = qi[0];

        for (int i = 1; i <= nn; ++i)
        {
            auto &&t = pvr * sr - pvi * si + pr[i];
            pvi = pvr * si + pvi * sr + pi[i];
            pvr = t;
            qr[i] = pvr;
            qi[i] = pvi;
        }
    }

public:

    /**
     * Find the roots of the polynomial with complex coefficients using the Jenkins-Traub algorithm
     * @param      coeff the coefficients of the polynomial, where the first coefficient corresponds to the
     *                   highest degree and the last coefficient corresponds to the lowest degree
     * @param[in]  roots storage to hold the roots computed by the algorithm
     * @param[out] roots if the algorithm is successful, will contain the roots belonging to input polynomial
     * @return           the number of roots found
     */
    virtual std::size_t findRoots(const std::vector<std::complex<T>> &coeff,
                                  std::vector<std::complex<T>> &roots) const final
    {
        auto size = coeff.size();
        auto order = size - 1;
        if (order > 0)
        {
            if (roots.size() >= order)
            {
                auto &&nRoots = computeRoots(coeff, order, roots);

                return nRoots > 0 ? nRoots : 0;
            }
            else
            {
                // this should not happen...
                throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                                         "Size of vector to store roots is too small.");
            }
        }

        return 0;
    }

    /**
     * Find the roots of the polynomial with real coefficients using the Jenkins-Traub algorithm
     * @param      coeff the coefficients of the polynomial, where the first coefficient corresponds to the
     *                   highest degree and the last coefficient corresponds to the lowest degree
     * @param[in]  roots storage to hold the roots computed by the algorithm
     * @param[out] roots if the algorithm is successful, will contain the roots belonging to input polynomial
     * @return           the number of roots found
     */
    virtual std::size_t findRoots(const std::vector<T> &coeff,
                                  std::vector<std::complex<T>> &roots) const final
    {
        auto size = coeff.size();
        auto order = size - 1;
        if (order > 0)
        {
            if (roots.size() >= order)
            {
                m_coeff.resize(coeff.size());
                for (std::size_t i = 0; i < coeff.size(); ++i)
                {
                    m_coeff[i].real(coeff[i]);
                    m_coeff[i].imag(0.0);
                }

                return findRoots(m_coeff, roots);
            }
            else
            {
                // this should not happen...
                throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": "
                                         "Size of vector to store roots is too small.");
            }
        }

        return 0;
    }

private:

    /**
     * Computes l2 fixed-shift H polynomials and tests for convergence. Initiates a variable-shift iteration
     * and returns with the approximate zero  via function arguments if successful
     * @param      l2     the limit of fixed shift steps
     * @param[out] zr, zi define the real and imaginary parts of the approximate zero if convergence is
     *                    achieved
     * @return            true if convergence of stage 3 iteration is achieved
     */
    virtual bool fixedShift(int l2, T &zr, T &zi) const final
    {
        auto n = m_n;

        evaluatePolynomial(m_n, m_sr, m_si, m_pr, m_pi, m_qpr, m_qpi, m_pvr, m_pvi);

        // calculate first T = -P(S) / H(S)
        bool bH_isNearZero = calcT();
        bool bTest = true;
        bool bPasd = false;

        // main loop for second stage
        for (int j = 1; j <= l2; ++j)
        {
            auto &&otr = m_tr;
            auto &&oti = m_ti;

            // compute the next H Polynomial and new t
            nextH(bH_isNearZero);
            bH_isNearZero = calcT();
            zr = m_sr + m_tr;
            zi = m_si + m_ti;

            // test for convergence unless stage 3 has failed once or this is the last H polynomial
            if (!(bH_isNearZero || !bTest || j == 12))
            {
                if (complexModulus(m_tr - otr, m_ti - oti) < 0.5 * complexModulus(zr, zi))
                {
                    if (bPasd)
                    {
                        // the weak convergence test has been passed twice, start the third stage iteration,
                        // after saving the current H polynomial and shift
                        for (int i = 0; i < n; ++i)
                        {
                            m_shr[i] = m_hr[i];
                            m_shi[i] = m_hi[i];
                        }

                        auto &&svsr = m_sr;
                        auto &&svsi = m_si;

                        bool bConv = variableShift(10, zr, zi);
                        if (bConv)
                            return bConv;

                        // the iteration failed to converge. Turn off testing and restore h, s, pv and T
                        bTest = false;
                        for (int i = 0; i < n; ++i)
                        {
                            m_hr[i] = m_shr[i];
                            m_hi[i] = m_shi[i];
                        }

                        m_sr = svsr;
                        m_si = svsi;
                        evaluatePolynomial(m_n, m_sr, m_si, m_pr, m_pi, m_qpr, m_qpi, m_pvr, m_pvi);
                        bH_isNearZero = calcT();

                        continue;
                    }

                    bPasd = true;
                }
                else
                    bPasd = false;
            }
        }

        // attempt an iteration with final H polynomial from second stage
        return variableShift(10, zr, zi);
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "JenkinsTraub";
    }

    /**
     * Calculates the next shifted H polynomial
     * @param bH_isNearZero if true, indicates that H(s) is essentially zero
     */
    virtual void nextH(const bool &bH_isNearZero) const final
    {
        auto n = m_n;
        if (!bH_isNearZero)
        {
            for (int j = 1; j < n; ++j)
            {
                auto &&t1 = m_qhr[j - 1];
                auto &&t2 = m_qhi[j - 1];
                m_hr[j] = m_tr * t1 - m_ti * t2 + m_qpr[j];
                m_hi[j] = m_tr * t2 + m_ti * t1 + m_qpi[j];
            }

            m_hr[0] = m_qpr[0];
            m_hi[0] = m_qpi[0];
        }
        else
        {
            // if H(s) is zero replace H with qh
            for (int j = 1; j < n; ++j)
            {
                m_hr[j] = m_qhr[j - 1];
                m_hi[j] = m_qhi[j - 1];
            }

            m_hr[0] = 0.0;
            m_hi[0] = 0.0;
        }
    }

    /**
     * Computes the derivative polynomial as the initial H polynomial and computes l1 no-shift H polynomials.
     */
    virtual void noShift(int l1) const final
    {
        auto n = m_n;
        auto nm1 = n - 1;
        for (int i = 0; i < n; ++i)
        {
            auto &&xni = m_n - i;
            m_hr[i] = xni * m_pr[i] / n;
            m_hi[i] = xni * m_pi[i] / n;
        }

        for (int k = 1; k <= l1; ++k)
        {
            if (complexModulus(m_hr[nm1], m_hi[nm1]) > m_eta * 10 * complexModulus(m_pr[nm1], m_pi[nm1]))
            {
                complexDivide(-m_pr[m_n], -m_pi[m_n], m_hr[nm1], m_hi[nm1], m_tr, m_ti);
                for (int i = 0; i < nm1; ++i)
                {
                    auto j = m_n - i - 1;
                    auto t1 = m_hr[j - 1];
                    auto t2 = m_hi[j - 1];
                    m_hr[j] = m_tr * t1 - m_ti * t2 + m_pr[j];
                    m_hi[j] = m_tr * t2 + m_ti * t1 + m_pi[j];
                }

                m_hr[0] = m_pr[0];
                m_hi[0] = m_pi[0];
            }
            else
            {
                // if the constant term is essentially zero, shift H coefficients
                for (int i = 0; i < nm1; ++i)
                {
                    auto j = m_n - i - 1;
                    m_hr[j] = m_hr[j - 1];
                    m_hi[j] = m_hi[j - 1];
                }

                m_hr[0] = 0.0;
                m_hi[0] = 0.0;
            }
        }
    }

    /**
     * Returns a scale factor to multiply the coefficients of the polynomial. The scaling is done to avoid
     * overflow and to avoid undetected underflow interfering with the convergence criterion. The factor is a
     * power of the base.
     * @param pt the modulus of coefficients of p
     */
    virtual T scale(const std::vector<T> &pt) const final
    {
        // find largest and smallest moduli of coefficients
        auto &&hi = std::sqrt(m_infinity);
        auto &&lo = m_smalno / m_eta;
        auto max = 0.0;
        auto min = m_infinity;

        for (int i = 0; i <= m_n; ++i)
        {
            auto &&x = pt[i];
            if (x > max)
                max = x;

            if (x != 0 && x < min)
                min = x;
        }

        // scale only if there are very large or very small components
        if (min >= lo && max <= hi)
            return 1.0;

        T sc, x = lo / min;
        if (x <= 1.0)
            sc = 1.0 / (std::sqrt(max) * std::sqrt(min));
        else
        {
            sc = x;
            if (m_infinity / sc > max)
                sc = 1.0;
        }

        return std::pow(m_base, (int)(std::log(sc) / std::log(m_base) + 0.5));
    }

    /**
     * Swap function
     */
    virtual void swap(JenkinsTraub<T> &jenkinsTraub) override final
    {
        std::swap(m_are, jenkinsTraub.m_are);
//      std::swap(m_base, jenkinsTraub.m_base);
        m_coeff.swap(jenkinsTraub.m_coeff);
//      std::swap(m_eta, jenkinsTraub.m_eta);
        m_hi.swap(jenkinsTraub.m_hi);
        m_hr.swap(jenkinsTraub.m_hr);
//      std::swap(m_infinity, jenkinsTraub.m_infinity);
        std::swap(m_mre, jenkinsTraub.m_mre);
        std::swap(m_n, jenkinsTraub.m_n);
        m_pi.swap(jenkinsTraub.m_pi);
        m_pr.swap(jenkinsTraub.m_pr);
        std::swap(m_pvi, jenkinsTraub.m_pvi);
        std::swap(m_pvr, jenkinsTraub.m_pvr);
        m_qhi.swap(jenkinsTraub.m_qhi);
        m_qhr.swap(jenkinsTraub.m_qhr);
        m_qpi.swap(jenkinsTraub.m_qpi);
        m_qpr.swap(jenkinsTraub.m_qpr);
        m_shi.swap(jenkinsTraub.m_shi);
        m_shr.swap(jenkinsTraub.m_shr);
        std::swap(m_si, jenkinsTraub.m_si);
//      std::swap(m_smalno, jenkinsTraub.m_smalno);
        std::swap(m_sr, jenkinsTraub.m_sr);
        std::swap(m_ti, jenkinsTraub.m_ti);
        std::swap(m_tr, jenkinsTraub.m_tr);
    }

    /**
     * Carries out the third stage iteration
     * @param         l3     the limit of steps in stage 3.
     * @param[in,out] zr, zi on entry contain the initial iterate; if the iteration converges it contains the
     *                       final iterate on exit.
     * @return               true if iteration converges
     */
    virtual bool variableShift(int l3, T &zr, T &zi) const final
    {
        T omp = 0, relstp = 0, tp = 0;
        int b = 0;
        m_sr = zr;
        m_si = zi;

        // main loop for stage three
        for (int i = 1; i <= l3; ++i)
        {
            // evaluate P at s and test for convergence
            evaluatePolynomial(m_n, m_sr, m_si, m_pr, m_pi, m_qpr, m_qpi, m_pvr, m_pvi);

            auto &&mp = complexModulus(m_pvr, m_pvi);
            auto &&ms = complexModulus(m_sr, m_si);
            if (mp <= 20 * boundError(m_qpr, m_qpi, ms, mp, m_are, m_mre))
            {
                // polynomial value is smaller in value than a bound on the error in evaluating P, terminate
                // the iteration
                zr = m_sr;
                zi = m_si;

                return true;
            }

            bool bH_isNearZero, bSkipToCalcNextIterate = false;
            if (i != 1)
            {
                if (!(b || mp < omp || relstp >= 0.05))
                {
                    // iteration has stalled. Probably a cluster of zeros. Do 5 fixed shift steps into the
                    // cluster to force one zero to dominate
                    tp = relstp, b = 1;

                    if (relstp < m_eta)
                        tp = m_eta;

                    auto &&r1 = std::sqrt(tp);
                    auto &&r2 = m_sr * (1 + r1) - m_si * r1;
                    m_si = m_sr * r1 + m_si * (1 + r1);
                    m_sr = r2;

                    evaluatePolynomial(m_n, m_sr, m_si, m_pr, m_pi, m_qpr, m_qpi, m_pvr, m_pvi);

                    for (int j = 1; j <= 5; ++j)
                    {
                        bH_isNearZero = calcT();
                        nextH(bH_isNearZero);
                    }

                    omp = m_infinity;
                    bSkipToCalcNextIterate = true;
                }

                // exit if polynomial value increase significantly
                if (!bSkipToCalcNextIterate && mp * 0.1 > omp)
                    return false;
            }

            if (!bSkipToCalcNextIterate)
                omp = mp;

            // calculate next iterate
            bH_isNearZero = calcT();
            nextH(bH_isNearZero);
            bH_isNearZero = calcT();

            if (!bH_isNearZero)
            {
                relstp = complexModulus(m_tr, m_ti) / complexModulus(m_sr, m_si);
                m_sr += m_tr;
                m_si += m_ti;
            }
        }

        return false;
    }

    /**
     * the base of the floating-point number system used
     */
    const T m_base;

    /**
     * the maximum relative representation error which can be described as the smallest positive floating-
     * point number such that 1.0 + eta > 1.0.
     */
    const T m_eta;

    /**
     * the largest floating-point number
     */
    const T m_infinity;

    /**
     * the smallest positive floating-point number
     */
    const T m_smalno;

    /**
     * internal member variables used to reduce parameter passing
     */
    mutable T m_are, m_mre, m_pvi, m_pvr, m_si, m_sr, m_ti, m_tr;
    mutable int m_n;

    /**
     * workspace coefficient vector
     */
    mutable std::vector<std::complex<T>> m_coeff;
    mutable std::vector<T> m_hi, m_hr, m_pi, m_pr, m_qhi, m_qhr, m_qpi, m_qpr, m_shi, m_shr;
};

}

}

}

}

#endif
