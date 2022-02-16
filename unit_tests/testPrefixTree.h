#ifndef TEST_PREFIX_TREE_H
#define TEST_PREFIX_TREE_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for PrefixTree class
 */
class PrefixTreeUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    PrefixTreeUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    PrefixTreeUnitTest(const PrefixTreeUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    PrefixTreeUnitTest(PrefixTreeUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~PrefixTreeUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    PrefixTreeUnitTest &operator = (const PrefixTreeUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    PrefixTreeUnitTest &operator = (PrefixTreeUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static PrefixTreeUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "PrefixTreeTest";
    }
};

}

#endif
