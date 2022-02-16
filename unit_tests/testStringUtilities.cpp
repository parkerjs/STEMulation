#include "string_replacer.h"
#include "testStringUtilities.h"
#include "unitTestManager.h"
#include <iostream>

// using namespace declarations
using namespace attributes::abstract;
using namespace messaging;
using namespace utilities::string;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testStringUtilities", &StringUtilitiesUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
StringUtilitiesUnitTest::StringUtilitiesUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
StringUtilitiesUnitTest *StringUtilitiesUnitTest::create(UnitTestManager *pUnitTestManager)
{
    StringUtilitiesUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new StringUtilitiesUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool StringUtilitiesUnitTest::execute(void)
{
    bool bSuccess = true;

    std::cout << "Starting unit test for StringUtilities class..." << std::endl << std::endl;

    // test simple search and replace
    std::string text = "Hello, ABCD should be replaced by DEFG and then by ABCD";

    StringReplacer<std::string> stringReplacer;
    stringReplacer.add("ABCD", "DEFGHIJK");
    stringReplacer.add("EFGHIJK ", "? ");
    stringReplacer.add("Hello", "Hi");

    // perform the search and replace; the following position and length shouldn't replace anything
    std::string temp(text);
    stringReplacer.searchAndReplace(temp, 8, 46);

    bSuccess = temp.compare("Hello, ABCD should be replaced by DEFG and then by ABCD") == 0;
    if (!bSuccess)
        return bSuccess;

    // perform the search and replace; the following should replace everything after and including the 8th
    // character
    temp = text;
    stringReplacer.searchAndReplace(temp, 8, 47);

    bSuccess = temp.compare("Hello, ABCD should be replaced by DEFG and then by DEFGHIJK") == 0;
    if (!bSuccess)
        return bSuccess;

    // perform the search and replace; the first occurrence of "ABCD" should be replaced
    temp = text;
    stringReplacer.searchAndReplace(temp, 7, 46);

    bSuccess = temp.compare("Hello, D? should be replaced by DEFG and then by ABCD") == 0;
    if (!bSuccess)
        return bSuccess;

    // perform the search and replace; the following should replace everything in the string
    temp = text;
    stringReplacer.searchAndReplace(temp, 0, 55);

    bSuccess = temp.compare("Hi, D? should be replaced by DEFG and then by DEFGHIJK") == 0;
    if (!bSuccess)
        return bSuccess;

    // now test manipulation of strings between delimiters
    text = "Test manipulation of \"quoted strings\" within \"strings\" using search and replace"
           "overload that manipulates substrings within \"delimiters\"";

    auto &&manipulator = [&] (std::string &string, std::string::size_type start, std::string::size_type length)
    {
        auto &&begin = string.begin() + start;
        auto &&end = begin + length;
        std::transform(begin, end, begin, ::toupper);

        return true;
    };

    // perform search and replace, do not erase delimiters yet...
    stringReplacer.searchAndReplace(text, manipulator, "\"", "\"", false);

    // after the find and replace, the strings within quotes should have been upper-cased
    temp = text;
    bSuccess = temp.compare("Test manipulation of \"QUOTED STRINGS\" within \"STRINGS\" using search and replace"
                            "overload that manipulates substrings within \"DELIMITERS\"") == 0;
    if (!bSuccess)
        return bSuccess;

    // perform search and replace, do not erase delimiters yet...
    stringReplacer.searchAndReplace(text, manipulator, "\"", "\"");

    // after the find and replace, the strings within quotes should have been upper-cased
    temp = text;
    bSuccess = text.compare("Test manipulation of QUOTED STRINGS within STRINGS using search and replace"
                            "overload that manipulates substrings within DELIMITERS") == 0;

    std::cout << "Test " << (bSuccess ? "PASSED." : "FAILED.") << std::endl << std::endl;

    return bSuccess;
}

}
