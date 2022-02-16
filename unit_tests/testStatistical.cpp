#include "correlation.h"
#include "covariance.h"
#include "testStatistical.h"
#include "unitTestManager.h"
#include <iomanip>
#include <iostream>

// using namespace declarations
using namespace attributes::abstract;
using namespace math::statistical;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testStatistical", &StatisticalUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
StatisticalUnitTest::StatisticalUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
StatisticalUnitTest *StatisticalUnitTest::create(UnitTestManager *pUnitTestManager)
{
    StatisticalUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new StatisticalUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool StatisticalUnitTest::execute(void)
{
    bool bSuccess = true;

    std::cout << "Starting unit test for statistical calculations..." << std::endl << std::endl;

    std::vector<std::pair<double, double>> samples = { { 1, 8 }, { 3, 6 }, { 2, 9 }, { 5, 4 }, { 8, 3 },
                                                       { 7, 3 }, { 12, 2 }, { 2, 7 }, { 4, 7 } };

    typedef std::vector<std::pair<double, double>> tSamplePairs;
    typedef std::vector<double> tVector;

    // naive calculation of variance
    auto &&variance = [] (const tVector::iterator itBegin, const tVector::iterator itEnd, bool bBiased)
    {
        double mean = 0.0;
        auto &&size = std::distance(itBegin, itEnd);
        for (auto it = itBegin; it != itEnd; ++it)
            mean += *it;

        mean /= size;

        double var = 0.0;
        for (auto it = itBegin; it != itEnd; ++it)
            var += (*it - mean) * (*it - mean);

        if (size > 1)
            var /= (bBiased ? size : size - 1);
        else
            var = 0.0;

        return var;
    };

    // naive calculation of covariance
    auto &&covar = [] (const tSamplePairs::iterator itBegin, const tSamplePairs::iterator itEnd, bool bBiased)
    {
        double x_mean = 0.0;
        double y_mean = 0.0;
        auto &&size = std::distance(itBegin, itEnd);
        for (auto it = itBegin; it != itEnd; ++it)
        {
            x_mean += it->first;
            y_mean += it->second;
        }

        x_mean /= size;
        y_mean /= size;

        double cov = 0.0;
        for (auto it = itBegin; it != itEnd; ++it)
            cov += (it->first - x_mean) * (it->second - y_mean);

        if (size > 1)
            cov /= (bBiased ? size : size - 1);
        else
            cov = 0.0;

        return cov;
    };

    // naive calculation of correlation
    auto &&corr = [&covar, &variance] (const tSamplePairs::iterator itBegin, const tSamplePairs::iterator itEnd,
                                       bool bBiased)
    {
        std::vector<double> x_samples, y_samples;
        for (auto it = itBegin; it != itEnd; ++it)
        {
            x_samples.push_back(it->first);
            y_samples.push_back(it->second);
        }

        auto &&stdDev_x = std::sqrt(variance(x_samples.begin(), x_samples.end(), bBiased));
        auto &&stdDev_y = std::sqrt(variance(y_samples.begin(), y_samples.end(), bBiased));

        if (stdDev_x > 0.0 && stdDev_y > 0.0)
            return covar(itBegin, itEnd, bBiased) / stdDev_x / stdDev_y;
        else
            return 0.0;
    };

    std::cout << "Test covariance/correlation calculations using Welford's on-line/moving algorithm."
              << std::endl << std::endl;

    // add samples one by one...
    std::cout << "Test ability to add samples one at a time using on-line Welford algorithm." << std::endl
              << std::endl;

    Correlation<double> correlation;
    Covariance<double> covariance;
    std::cout << std::setw(16) << "# Samples," << std::setw(16) << "x," << std::setw(16) << "y,"
              << std::setw(16) << "sample cov," << std::setw(16) << "pop cov,"
              << std::setw(16) << "sample corr," << std::setw(16) << "pop corr," << std::endl;

    int i = 1;
    for (auto &&pair : samples)
    {
        covariance.addSample(pair);
        covariance.setBiasedEstimate(false);
        auto &&sample_cov = covariance.calculate();
        covariance.setBiasedEstimate(true);
        auto &&pop_cov = covariance.calculate();
        std::cout << std::setw(15) << covariance.getNumSamples() << "," << std::setw(15) << pair.first << ","
                  << std::setw(15) << pair.second << "," << std::setw(15) << sample_cov << ","
                  << std::setw(15) << pop_cov << ",";

        auto &&cov_s = covar(samples.begin(), samples.begin() + i, false);
        auto &&cov_p = covar(samples.begin(), samples.begin() + i, true);

        correlation.addSample(pair);
        correlation.setBiasedEstimate(false);
        auto &&sample_corr = correlation.calculate();
        correlation.setBiasedEstimate(true);
        auto &&pop_corr = correlation.calculate();
        std::cout << std::setw(15) << sample_corr << "," << std::setw(15) << pop_corr << "," << std::endl;

        auto &&corr_s = corr(samples.begin(), samples.begin() + i, false);
        auto &&corr_p = corr(samples.begin(), samples.begin() + i, true);
        bSuccess &= (std::fabs(sample_cov - cov_s) < 1e-10 && std::fabs(pop_cov - cov_p) < 1e-10 &&
                     std::fabs(sample_corr - corr_s) < 1e-10 && std::fabs(pop_corr - corr_p) < 1e-10);
        ++i;
    }

    std::cout << std::endl << "Does on-line covariance calculation match the naive calculation? "
              << (bSuccess ? "Yes, test PASSED." : "No, test FAILED.") << std::endl << std::endl;

    // add samples one by one...
    std::cout << "Test ability to remove samples one at a time using on-line Welford algorithm."
              << std::endl << std::endl;

    std::cout << std::setw(16) << "# Samples," << std::setw(16) << "x," << std::setw(16) << "y,"
              << std::setw(16) << "sample cov," << std::setw(16) << "pop cov,"
              << std::setw(16) << "sample corr," << std::setw(16) << "pop corr," << std::endl;

    for (auto &&itPair = samples.rbegin(); itPair != samples.rend(); ++itPair)
    {
        --i;
        covariance.setBiasedEstimate(false);
        auto &&sample_cov = covariance.calculate();
        covariance.setBiasedEstimate(true);
        auto &&pop_cov = covariance.calculate();
        std::cout << std::setw(15) << covariance.getNumSamples() << "," << std::setw(15) << itPair->first << ","
                  << std::setw(15) << itPair->second << "," << std::setw(15) << sample_cov << ","
                  << std::setw(15) << pop_cov << ",";

        auto &&cov_s = covar(samples.begin(), samples.begin() + i, false);
        auto &&cov_p = covar(samples.begin(), samples.begin() + i, true);
        covariance.deleteSample(*itPair);

        correlation.setBiasedEstimate(false);
        auto &&sample_corr = correlation.calculate();
        correlation.setBiasedEstimate(true);
        auto &&pop_corr = correlation.calculate();
        std::cout << std::setw(15) << sample_corr << "," << std::setw(15) << pop_corr << "," << std::endl;

        auto &&corr_s = corr(samples.begin(), samples.begin() + i, false);
        auto &&corr_p = corr(samples.begin(), samples.begin() + i, true);
        correlation.deleteSample(*itPair);

        bSuccess &= (std::fabs(sample_cov - cov_s) < 1e-10 && std::fabs(pop_cov - cov_p) < 1e-10 &&
                     std::fabs(sample_corr - corr_s) < 1e-10 && std::fabs(pop_corr - corr_p) < 1e-10);
    }

    std::cout << std::endl << "Does on-line covariance calculation match the naive calculation? "
              << (bSuccess ? "Yes, test PASSED." : "No, test FAILED.") << std::endl << std::endl;

    return bSuccess;
}

}
