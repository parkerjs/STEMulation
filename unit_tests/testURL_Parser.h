#ifndef TEST_URL_PARSER_H
#define TEST_URL_PARSER_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for URL_Parser class
 */
class URL_ParserUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    URL_ParserUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    URL_ParserUnitTest(const URL_ParserUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    URL_ParserUnitTest(URL_ParserUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~URL_ParserUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    URL_ParserUnitTest &operator = (const URL_ParserUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    URL_ParserUnitTest &operator = (URL_ParserUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static URL_ParserUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "URL_ParserTest";
    }
};

}

#endif
