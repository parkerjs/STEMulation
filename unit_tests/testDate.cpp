#include "date.h"
#include "date_yyyymmdd.h"
#include "testDate.h"
#include "unitTestManager.h"
#include <iostream>

// using namespace declarations
using namespace attributes::abstract;
using namespace messaging;
using namespace utilities::calendar;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testDate", &DateUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
DateUnitTest::DateUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
DateUnitTest *DateUnitTest::create(UnitTestManager *pUnitTestManager)
{
    DateUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new DateUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool DateUnitTest::execute(void)
{
    std::cout << "Starting unit test for Date class..." << std::endl << std::endl;

    bool bSuccess = true;

    for (int i = 0; i < 100; ++i)
    {
        DateYYYYMMDD earlierDate(1, 1, 2000);
        DateYYYYMMDD laterDate(1, 1, 2010);

        int numDaysBetweenDates = earlierDate.calcDifferenceInDays(laterDate);
        int randomDateOne = earlierDate.calcDateDaysFromCurrent(rand() % numDaysBetweenDates);
        int randomDateTwo = earlierDate.calcDateDaysFromCurrent(rand() % numDaysBetweenDates);

        earlierDate.setDate(randomDateOne < randomDateTwo ? randomDateOne : randomDateTwo);
        laterDate.setDate(randomDateOne > randomDateTwo ? randomDateOne : randomDateTwo);

        std::cout << "Earlier date: " <<  earlierDate << std::endl;
        std::cout << "Later date:   " << laterDate << std::endl << std::endl;

        std::cout << std::setw(10) << "Date 1" << std::setw(10) << "Date 2" << std::setw(10) << "Days"
                  << std::setw(10) << "Months" << std::setw(10) << "Years" << std::endl;

        int numDays = 0, numMonths = 0, numTotalDays = 0, numYears = 0;
        DateYYYYMMDD currentDate = earlierDate;
        while (true)
        {
            int prevNumDays = numDays;
            numYears = earlierDate.calcDifferenceInYears(currentDate, numMonths, numDays);

            std::cout << std::setw(10) << earlierDate << std::setw(10) << currentDate
                      << std::setw(10) << numDays << std::setw(10) << numMonths
                      << std::setw(10) << numYears << std::endl;

            if (numDays < prevNumDays)
            {
                numTotalDays += prevNumDays;
                if (numDays == 0)
                    ++numTotalDays;
            }

            if (currentDate == laterDate)
            {
                numTotalDays += numDays;
                break;
            }

            currentDate.setDate(currentDate.calcDateDaysFromCurrent(1));
        }

        std::cout << std::endl << "Computed number of day(s), month(s), year(s) between "
                  << earlierDate << " and " << laterDate << ": " << numDays << " day(s), "
                  << numMonths << " month(s), " << numYears << " year(s)." << std::endl;

        numDays = laterDate.calcDifferenceInDays(earlierDate);

        bSuccess &= (numDays == numTotalDays);
        if (bSuccess)
            std::cout << "Date test PASSED, total number of days computed by accumulating days "
                      << "between fully elapsed months (with respect to starting date): "
                      << numDays << "." << std::endl << std::endl;
        else
        {
            std::cout << "Date test FAILED." << std::endl << std::endl;
            break;
        }
    }

    if (bSuccess)
        std::cout << "All Date tests PASSED." << std::endl << std::endl;

    return bSuccess;
}

}
