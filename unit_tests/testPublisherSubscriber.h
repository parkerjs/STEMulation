#ifndef TEST_PUBLISHER_SUBSCRIBER_H
#define TEST_PUBLISHER_SUBSCRIBER_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for publisher-subscriber messaging
 */
class PublisherSubscriberUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    PublisherSubscriberUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    PublisherSubscriberUnitTest(const PublisherSubscriberUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    PublisherSubscriberUnitTest(PublisherSubscriberUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~PublisherSubscriberUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    PublisherSubscriberUnitTest &operator = (const PublisherSubscriberUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    PublisherSubscriberUnitTest &operator = (PublisherSubscriberUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static PublisherSubscriberUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "PublisherSubscriberTest";
    }
};

}

#endif
