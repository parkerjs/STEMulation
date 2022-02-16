#include "testDependencyInjectable.h"
#include "unitTestManager.h"

// using namespace declarations
using namespace attributes::abstract;
using namespace attributes::concrete;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testDependencyInjectable",
                                                    &DependencyInjectableUnitTest::create);

// define some dependency classes
struct DependencyOne { };
struct DependencyTwo { };
struct DependencyThree { };
struct DependencyFour { };
struct DependencyFive { };

/**
 * A class with some injection dependencies
 */
class DependentClassOne
: virtual public DependencyInjectable<DependencyOne *>
{
public:

    /**
     * Constructor
     */
    DependentClassOne(const tDependencies &dependencies,
                      const std::string &string,
                      int integer)
    : DependencyInjectableVirtualBaseInitializer(1,
                                                 dependencies),
      m_integer(integer),
      m_string(string)
    {

    }

    /**
     * Constructor
     */
    DependentClassOne(DependencyOne *pDependencyOne,
                      const std::string &string,
                      int integer)
    : DependencyInjectableVirtualBaseInitializer(1,
                                                 pDependencyOne),
      m_integer(integer),
      m_string(string)
    {

    }

    /**
     * Copy constructor
     */
    DependentClassOne(const DependentClassOne &dependencClassOne)
    : DependencyInjectableVirtualBaseInitializer(1,
                                                 dependencClassOne)
    {

    }

    /**
     * Move constructor
     */
    DependentClassOne(DependentClassOne &&dependencClassOne)
    : DependencyInjectableVirtualBaseInitializer(1,
                                                 std::move(dependencClassOne))
    {

    }

    int m_integer;
    std::string m_string;
};

/**
 * A class with some injection dependencies
 */
class DependentClassTwo
: virtual public DependencyInjectable<dependencies_of<DependentClassOne>,
                                      DependencyTwo *>,
  public DependentClassOne
{
public:

    /**
     * Constructor
     */
    DependentClassTwo(const tDependencies &dependencies,
                      int integer)
    : DependencyInjectableVirtualBaseInitializer(2,
                                                 dependencies),
      DependentClassOne(dependencies,
                        "two",
                        integer)
    {

    }

    /**
     * Constructor
     */
    DependentClassTwo(DependencyOne *pDependencyOne,
                      DependencyTwo *pDependencyTwo,
                      int integer)
    : DependencyInjectableVirtualBaseInitializer(2,
                                                 pDependencyOne,
                                                 pDependencyTwo),
      DependentClassOne(pDependencyOne,
                        "two",
                        integer)
    {

    }

    /**
     * Copy constructor
     */
    DependentClassTwo(const DependentClassTwo &dependentClassTwo)
    : DependencyInjectableVirtualBaseInitializer(2,
                                                 dependentClassTwo),
      DependentClassOne(dependentClassTwo)
    {

    }

    /**
     * Move constructor
     */
    DependentClassTwo(DependentClassTwo &&dependentClassTwo)
    : DependencyInjectableVirtualBaseInitializer(2,
                                                 std::move(dependentClassTwo)),
      DependentClassOne(std::move(dependentClassTwo))
    {

    }
};

/**
 * A class with some injection dependencies
 */
class DependentClassThree
: virtual public DependencyInjectable<dependencies_of<DependentClassTwo>,
                                      DependencyThree *>,
  public DependentClassTwo
{
public:

    /**
     * Constructor
     */
    DependentClassThree(const tDependencies &dependencies)
    : DependencyInjectableVirtualBaseInitializer(3,
                                                 dependencies),
      DependentClassTwo(dependencies,
                        3)
    {

    }

    /**
     * Constructor
     */
    DependentClassThree(DependencyOne *pDependencyOne,
                        DependencyTwo *pDependencyTwo,
                        DependencyThree *pDependencyThree)
    : DependencyInjectableVirtualBaseInitializer(3,
                                                 pDependencyOne,
                                                 pDependencyTwo,
                                                 pDependencyThree),
      DependentClassTwo(pDependencyOne,
                        pDependencyTwo,
                        3)
    {

    }

    /**
     * Copy constructor
     */
    DependentClassThree(const DependentClassThree &dependentClassThree)
    : DependencyInjectableVirtualBaseInitializer(3,
                                                 dependentClassThree),
      DependentClassTwo(dependentClassThree)
    {

    }

    /**
     * Move constructor
     */
    DependentClassThree(DependentClassThree &&dependentClassThree)
    : DependencyInjectableVirtualBaseInitializer(3,
                                                 std::move(dependentClassThree)),
      DependentClassTwo(std::move(dependentClassThree))
    {

    }
};

/**
 * A class with some injection dependencies
 */
class DependentClassFour
: public DependencyInjectable<dependencies_of<DependentClassThree>,
                              DependencyFour *>,
  public DependentClassThree
{
public:

    /**
     * Constructor
     */
    DependentClassFour(const tDependencies &dependencies,
                       int integer)
    : DependencyInjectableVirtualBaseInitializer(4,
                                                 dependencies),
      DependentClassThree(dependencies)
    {
        m_integer = integer;
    }

    /**
     * Constructor
     */
    DependentClassFour(DependencyOne *pDependencyOne,
                       DependencyTwo *pDependencyTwo,
                       DependencyThree *pDependencyThree,
                       DependencyFour *pDependencyFour,
                       int integer)
    : DependencyInjectableVirtualBaseInitializer(4,
                                                 pDependencyOne,
                                                 pDependencyTwo,
                                                 pDependencyThree,
                                                 pDependencyFour),
      DependentClassThree(pDependencyOne,
                          pDependencyTwo,
                          pDependencyThree)
    {
        m_integer = integer;
    }

    /**
     * Copy constructor
     */
    DependentClassFour(const DependentClassFour &dependentClassFour)
    : DependencyInjectableVirtualBaseInitializer(4,
                                                 dependentClassFour),
      DependentClassThree(dependentClassFour)
    {

    }

    /**
     * Move constructor
     */
    DependentClassFour(DependentClassFour &&dependentClassFour)
    : DependencyInjectableVirtualBaseInitializer(4,
                                                 std::move(dependentClassFour)),
      DependentClassThree(std::move(dependentClassFour))
    {

    }
};

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
DependencyInjectableUnitTest::DependencyInjectableUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1,
                                             dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
DependencyInjectableUnitTest *DependencyInjectableUnitTest::create(UnitTestManager *pUnitTestManager)
{
    DependencyInjectableUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new DependencyInjectableUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool DependencyInjectableUnitTest::execute(void)
{
    std::cout << "Starting unit test for dependency injection..."
              << std::endl
              << std::endl;

    DependencyOne dependencyOne;
    DependencyTwo dependencyTwo;
    DependencyThree dependencyThree;
    DependencyFour dependencyFour;

    auto &&tuple = std::make_tuple(&dependencyOne,
                                   &dependencyTwo,
                                   &dependencyThree,
                                   &dependencyFour);
    DependentClassOne dependentClassOne(tuple,
                                        "one",
                                        1);
    DependentClassTwo dependentClassTwo(&dependencyOne,
                                        &dependencyTwo,
                                        2);
    DependentClassThree dependentClassThree(tuple);
    DependentClassFour dependentClassFour(&dependencyOne,
                                          &dependencyTwo,
                                          &dependencyThree,
                                          &dependencyFour,
                                          4);

    bool bSuccess = (dependentClassOne.m_integer == 1 &&
                     dependentClassTwo.m_integer == 2 &&
                     dependentClassThree.m_integer == 3 &&
                     dependentClassFour.m_integer == 4);

    if (bSuccess)
    {
        // test whether it compiles...
        dependentClassFour.setDependencies(&dependencyOne,
                                           &dependencyTwo,
                                           &dependencyThree,
                                           &dependencyFour);

        bSuccess = dependentClassFour.dependenciesInitialized();
    }

    if (bSuccess)
    {
        auto &&dependencies = dependentClassThree.getDependencies();
        bSuccess = (std::get<DependencyOne *>(dependencies) == &dependencyOne &&
                    std::get<DependencyTwo *>(dependencies) == &dependencyTwo &&
                    std::get<DependencyThree *>(dependencies) == &dependencyThree);
    }

    if (bSuccess)
    {
        auto *pDependencyOne = dependentClassOne.getDependency<DependencyOne *>();
        auto *pDependencyTwo = dependentClassTwo.getDependency<DependencyTwo *>();
        auto *pDependencyOneFromTwo = dependentClassTwo.getDependency<DependencyOne *>();
        auto *pDependencyThree = dependentClassThree.getDependency<DependencyThree *>();
        auto *pDependencyFour = dependentClassFour.getDependency<DependencyFour *>();
        auto *pDependencyOneFromThree = dependentClassThree.getDependency<DependencyOne *>();
        auto *pDependencyTwoFromThree = dependentClassThree.getDependency<DependencyTwo *>();
        auto *pDependencyOneFromFour = dependentClassFour.getDependency<DependencyOne *>();
        auto *pDependencyTwoFromFour = dependentClassFour.getDependency<DependencyTwo *>();
        auto *pDependencyThreeFromFour = dependentClassFour.getDependency<DependencyThree *>();
        bSuccess = (pDependencyOne == &dependencyOne &&
                    pDependencyOneFromTwo == pDependencyOne &&
                    pDependencyOneFromThree == pDependencyOne &&
                    pDependencyOneFromFour == pDependencyOne &&
                    pDependencyTwo == &dependencyTwo &&
                    pDependencyTwoFromThree == pDependencyTwo &&
                    pDependencyTwoFromFour == pDependencyTwo &&
                    pDependencyThree == &dependencyThree &&
                    pDependencyThreeFromFour == pDependencyThree &&
                    pDependencyFour == &dependencyFour);
    }

    std::cout << "Test "
              << (bSuccess ? "PASSED"
                           : "FALIED")
              << "."
              << std::endl
              << std::endl;

    return bSuccess;
}

}
