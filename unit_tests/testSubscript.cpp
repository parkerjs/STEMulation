#include "subscript.h"
#include "testSubscript.h"
#include "unitTestManager.h"
#include <random>

// using namespace declarations
using namespace attributes::abstract;
using namespace math::sets;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testSubscript",
                                                    &SubscriptUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
SubscriptUnitTest::SubscriptUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
SubscriptUnitTest *SubscriptUnitTest::create(UnitTestManager *pUnitTestManager)
{
    SubscriptUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new SubscriptUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool SubscriptUnitTest::execute(void)
{
    std::cout << "Starting unit test for Subscript..." << std::endl << std::endl;

    bool bSuccess = dependenciesInitialized();
    if (bSuccess)
    {
        std::ofstream outFile("bin/outputs/subscriptTestOutput.dat");

        Subscript set;
        set.addSubset(0, [] (auto &&index) { return index < 5; }, [] (auto &&index) { return index + 1; });
        set.addSubset(0, [] (auto &&index) { return index < 3; }, [] (auto &&index) { return index + 1; });
        set.addSubset(0, [] (auto &&index) { return index < 2; }, [] (auto &&index) { return index + 1; });
        set.addSubset(0, [] (auto &&index) { return index < 6; }, [] (auto &&index) { return index + 1; });

        // print the set cardinality
        outFile << "Cardinality of the subscript set: " << set.cardinality() << std::endl << std::endl;

        // print the sets of elements to file
        outFile << set << std::endl;

        // generate the Cartesian product of the sets
        auto &&cardinality = set.cardinality();
        auto &&cardinalities = set.cardinalities();
        std::vector<std::vector<std::size_t>> subscript(cardinality, cardinalities);
        set.generateTuples(subscript);

        // print all tuples from the Cartesian product; also print the linear index corresponding to each tuple
        outFile << std::setw(24) << "   --- Subscript ---    ";
        outFile << std::setw(5) << "Index" << std::endl;
        for (auto &&itTuple = subscript.begin(); itTuple != subscript.end(); ++itTuple)
        {
            for (auto &&it = itTuple->begin(); it != itTuple->end(); it++)
                outFile << std::setw(5) << *it;

            outFile << std::setw(7) << set(*itTuple) << std::endl;
        }

        outFile << std::endl;

        // now generate random numbers from 0 to 179 and get the corresponding tuple from the subscript set
        auto &&indices = Set<std::size_t>::generate(0, [] (auto &&index) { return index < 180; },
                                                       [] (auto &&index) { return index + 1; });
        std::default_random_engine randomNumberGenerator;
        outFile << std::setw(5) << "Index" << std::setw(24) << "   --- Subscript ---    " << std::endl;
        while (bSuccess && !indices.empty())
        {
            std::uniform_int_distribution<std::size_t> uniform(0, indices.size() - 1);
            auto index = indices[uniform(randomNumberGenerator)];
            auto &&itIndex = std::find(indices.begin(), indices.end(), index);
            outFile << std::setw(5) << index;

            auto &&tuple = set[index];
            for (auto &&itTuple = tuple.cbegin(); itTuple != tuple.cend(); ++itTuple)
                outFile << std::setw(5) << *itTuple;

            bSuccess = (set.toIndex(tuple) == *itIndex);
            indices.erase(itIndex);

            outFile << std::endl;
        }

        std::cout << "Test " << (bSuccess ? "PASSED" : "FALIED") << "." << std::endl << std::endl;
    }

    return bSuccess;
}

}

