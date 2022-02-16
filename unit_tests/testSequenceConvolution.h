#ifndef TEST_SEQUENCE_CONVOLVER_H
#define TEST_SEQUENCE_CONVOLVER_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for SequenceConvolver class
 */
class SequenceConvolverUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    SequenceConvolverUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    SequenceConvolverUnitTest(const SequenceConvolverUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    SequenceConvolverUnitTest(SequenceConvolverUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~SequenceConvolverUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    SequenceConvolverUnitTest &operator = (const SequenceConvolverUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    SequenceConvolverUnitTest &operator = (SequenceConvolverUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static SequenceConvolverUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "SequenceConvolutionTest";
    }
};

}

#endif
