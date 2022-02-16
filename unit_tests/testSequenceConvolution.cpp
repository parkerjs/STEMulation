#include "sequence_convolver.h"
#include "testSequenceConvolution.h"
#include "unitTestManager.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>

// using namespace declarations
using namespace attributes::abstract;
using namespace math::signal_processing;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testSequenceConvolution",
                                                    &SequenceConvolverUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
SequenceConvolverUnitTest::SequenceConvolverUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
SequenceConvolverUnitTest *SequenceConvolverUnitTest::create(UnitTestManager *pUnitTestManager)
{
    SequenceConvolverUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new SequenceConvolverUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool SequenceConvolverUnitTest::execute(void)
{
    /*
     * Test convolution of two data sequences
     */

    bool bSuccess = true; // assume success
    SequenceConvolver<double> sequenceConvolver;

    std::cout << "Starting unit test for convolution of two data sequences..." << std::endl << std::endl;

    std::default_random_engine randomNumberGenerator;
    std::uniform_int_distribution<int> uniform(-100, 100);

    for (std::size_t i = 0; i < 100; ++i)
    {
        std::vector<double> f(1 + std::abs(uniform(randomNumberGenerator)) % 10);
        std::vector<double> g(1 + std::abs(uniform(randomNumberGenerator)) % 10);

        std::generate(f.begin(), f.end(), [&] () { return 0.01 * uniform(randomNumberGenerator); });
        std::generate(g.begin(), g.end(), [&] () { return 0.01 * uniform(randomNumberGenerator); });

        std::cout << "Sequence f[n]: ";

        for (std::size_t j = 0; j < f.size(); ++j)
            std::cout << std::setw(10) << std::setprecision(5) << f[j];

        std::cout << std::endl << std::endl << "Sequence g[n]: ";

        for (std::size_t j = 0; j < g.size(); ++j)
            std::cout << std::setw(10) << std::setprecision(5) << g[j];

        // convolve f and g
        auto &&h = sequenceConvolver.convolve(f, g);

        std::cout << std::endl << std::endl << "Convolution of f[n] and g[n]: ";

        for (std::size_t j = 0; j < h.size(); ++j)
            std::cout << std::setw(10) << std::setprecision(5) << h[j];

        // try to deconvolve f out of h, thereby obtaining g
        std::vector<double> q;
        sequenceConvolver.deconvolve(f, h, q);

        std::cout << std::endl << std::endl << "Deconvolution of f[n] out of h[n]: ";

        for (std::size_t j = 0; j < q.size(); ++j)
            std::cout << std::setw(10) << std::setprecision(5) << q[j];

        // compute the difference between g and the result of the deconvolution of f from h
        std::vector<double> diff(q.size());
        std::transform(q.begin(), q.end(), g.begin(), diff.begin(), std::minus<double>());

        auto &&norm = std::sqrt(std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0));

        std::cout << std::endl << std::endl << "Is sequence g[n] obtained subsequent to deconvolving f[n] "
                  << "out of h[n]?" << std::endl << std::endl;

        bSuccess &= (norm <= 1e-7);
        if (bSuccess)
            std::cout << "Yes, test PASSED with 2-norm of difference being " << norm << "."
                      << std::endl << std::endl;
        else
            std::cout << "No, test FAILED." << std::endl << std::endl;
    }

    /*
     * Test convolution of two functions
     */

    std::cout << "*** Testing convolution of two functions ***" << std::endl << std::endl;

    // create vectors of abscissas
    std::vector<double> t(100);
    for (std::size_t i = 0; i < t.size(); ++i)
        t[i] = 0.01 + i * 0.01;

    std::cout << "Functions f(t) = 1 / sqrt(t) and g(t) = t^2" << std::endl;

    auto f_t = [] (double x) { return 1 / std::sqrt(x); };
/*  auto g_t = [] (double time) { return t * t; };*/
    auto h_t = [] (double x) { return 16.0 / 15.0 * x * x * std::sqrt(x); };

    // evaluate g(t)
    std::vector<double> f(t.size());
    std::vector<double> g(t.size());

    std::transform(t.begin(), t.end(), f.begin(), f_t);
    std::transform(t.begin(), t.end(), g.begin(), h_t);

    // convolve f(t) = 1 / sqrt(t) with g(t) = t^2
    auto &&h = sequenceConvolver.convolve(f, g);

    // try to deconvolve f(t) out of h(t), thereby obtaining g(t)
    std::vector<double> q;
    sequenceConvolver.deconvolve(f, h, q);

    std::cout << std::endl << std::setw(20) << "t" << std::setw(20) << "f(t)" << std::setw(20) << "g(t)"
              << std::setw(20) << "Convolution" << std::setw(20) << "Deconvolution" << std::endl
              << std::setw(80) << "h = f * g" << std::setw(20) << "g = h *^-1 f" << std::endl;

    for (std::size_t i = 0; i < t.size(); ++i)
        std::cout << std::setw(20) << t[i] << std::setw(20) << f[i] << std::setw(20) << g[i]
                  << std::setw(20) << h[i] << std::setw(20) << q[i] << std::endl;

    // compute the difference between g and the result of the deconvolution of f from h
    std::vector<double> diff(q.size());
    std::transform(q.begin(), q.end(), g.begin(), diff.begin(), std::minus<double>());

    auto &&norm = std::sqrt(std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0));

    std::cout << std::endl << "Is sequence g(t) obtained subsequent to deconvolving f(t) "
              << "out of h(t)?" << std::endl << std::endl;

    bSuccess &= (norm <= 1e-12);
    if (bSuccess)
        std::cout << "Yes, test PASSED with 2-norm of difference being " << norm << "."
                  << std::endl << std::endl;
    else
        std::cout << "No, test FAILED." << std::endl << std::endl;

    return bSuccess;
}

}
