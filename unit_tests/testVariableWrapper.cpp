#include "testVariableWrapper.h"
#include "unitTestManager.h"
#include "variable_wrapper.h"
#include <fstream>

// using namespace declarations
using namespace attributes::abstract;
using namespace functional;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testVariableWrapper", &VariableWrapperUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
VariableWrapperUnitTest::VariableWrapperUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
VariableWrapperUnitTest *VariableWrapperUnitTest::create(UnitTestManager *pUnitTestManager)
{
    VariableWrapperUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new VariableWrapperUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool VariableWrapperUnitTest::execute(void)
{
    std::cout << "Starting unit test for VariableWrapper..." << std::endl << std::endl;

    bool bSuccess = dependenciesInitialized();
    if (bSuccess)
    {
        std::ofstream stream("bin/outputs/variableWrapperTest.dat");

        /*
         * test the assignment operator
         */

        short shortVal = 1;
        long longVal = 2L;
        float floatVal = 3.0f;
        double doubleVal = 4.0;

        VariableWrapper<decltype(doubleVal) &> doubleVariable(doubleVal);
        std::string string;
        VariableWrapper<std::string &> stringVariable(string);

        stream << "Test operator = between VariableWrapper of type double and short value (1): "
               << (doubleVariable = shortVal) << std::endl;
        bSuccess &= (doubleVariable == 1.0);
        stream << "Test operator = between VariableWrapper of type double and long value (2L): "
               << (doubleVariable = longVal) << std::endl;
        bSuccess &= (doubleVariable == 2.0);
        stream << "Test operator = between VariableWrapper of type double and float value (3.0f): "
               << (doubleVariable = floatVal) << std::endl;
        bSuccess &= (doubleVariable == 3.0);
        stream << "Test operator = between VariableWrapper of type double and double value (4.0): "
               << (doubleVariable = 4) << std::endl;
        bSuccess &= (doubleVariable == 4.0);
        stream << "Test operator = between VariableWrapper of type std::string and char array (\"Marco\"): "
               << (stringVariable = "Marco") << "\n";
        bSuccess &= (stringVariable == "Marco");
        stream << "Test operator = between VariableWrapper of type std::string and std::string (\"Marco\"): "
               << (stringVariable = std::string("Marco")) << "\n";
        bSuccess &= (stringVariable == "Marco");

        /*
         * test the addition operator
         */

        VariableWrapper<decltype(shortVal) &> shortVariable(shortVal);
        VariableWrapper<decltype(longVal) &> longVariable(longVal);
        VariableWrapper<decltype(floatVal) &> floatVariable(floatVal);

        stream << "Test operator + between VariableWrapper of type double and a short (4.0 + 1): "
               << doubleVariable + shortVal << std::endl;
        bSuccess &= (doubleVariable + shortVal == 5.0);
        stream << "Test operator + between VariableWrapper of type double and a long (4.0 + 2L): "
               << doubleVariable + longVal << std::endl;
        bSuccess &= (doubleVariable + longVal == 6.0);
        stream << "Test operator + between VariableWrapper of type double and a float (4.0 + 3.0f): "
               << doubleVariable + floatVal << std::endl;
        bSuccess &= (doubleVariable + floatVal == 7.0);
        stream << "Test operator + between VariableWrapper of type double and a double (4.0 + 4.0): "
               << doubleVariable + doubleVal << std::endl;
        bSuccess &= (doubleVariable + doubleVal == 8.0);
        stream << "Test operator + between VariableWrapper of type std::string and a char array "
               << "(\"Marco\" + \"Polo\"): " << stringVariable + " Polo" << std::endl;
        bSuccess &= (stringVariable + " Polo" == "Marco Polo");
        stream << "Test operator + between VariableWrapper of type std::string and a(n) std::string "
               << "(\"Marco\" + \"Polo\"): " << stringVariable + std::string(" Polo") << std::endl;
        bSuccess &= (stringVariable + std::string(" Polo") == "Marco Polo");
        stream << "Test operator + between VariableWrappers of types short and double (1 + 4.0): "
               << shortVariable + doubleVariable << std::endl;
        bSuccess &= (shortVariable + doubleVariable == 5.0);
        stream << "Test operator + between VariableWrappers of types long and double (2L + 4.0): "
               << longVariable + doubleVariable << std::endl;
        bSuccess &= (longVariable + doubleVariable == 6.0);
        stream << "Test operator + between VariableWrappers of types float and double (3.0f + 4.0): "
               << floatVariable + doubleVariable << std::endl;
        bSuccess &= (floatVariable + doubleVariable == 7.0);
        stream << "Test operator + between VariableWrappers of type double (4.0 + 4.0): "
               << doubleVariable + doubleVariable << std::endl;
        bSuccess &= (doubleVariable + doubleVariable == 8.0);
        stream << "Test operator + between VariableWrappers of types std::string and char array (\"Marco\""
               << "+ \"Polo\"): " << stringVariable + VariableWrapper<const char *>(" Polo") << std::endl;
        bSuccess &= (stringVariable + VariableWrapper<const char *>(" Polo") == "Marco Polo");
        stream << "Test operator + between VariableWrappers of type std::string (\"Marco\" + \"Polo\"): "
               << stringVariable + VariableWrapper<std::string>(" Polo") << std::endl;
        bSuccess &= (stringVariable + VariableWrapper<std::string>(" Polo") == "Marco Polo");
        stream << "Test operator + between a short and VariableWrapper of type double (1 + 4.0): "
               << shortVal + doubleVariable << std::endl;
        bSuccess &= (shortVal + doubleVariable == 5.0);
        stream << "Test operator + between a long and VariableWrapper of type double (2L + 4.0): "
               << longVal + doubleVariable << std::endl;
        bSuccess &= (longVal + doubleVariable == 6.0);
        stream << "Test operator + between a float and VariableWrapper of type double (3f + 4.0): "
               << floatVal + doubleVariable << std::endl;
        bSuccess &= (floatVal + doubleVariable == 7.0);
        stream << "Test operator + between a double and VariableWrapper of type double (4.0 + 4.0): "
               << doubleVal + doubleVariable << std::endl;
        bSuccess &= (doubleVal + doubleVariable == 8.0);
        stream << "Test operator + between a char array and VariableWrapper of type std::string (\"Polo\""
               << " + \"Marco\"): " << "Polo " + stringVariable << std::endl;
        bSuccess &= ("Polo " + stringVariable == "Polo Marco");
        stream << "Test operator + between a(n) std::string and VariableWrapper of type std::string (\"Polo\""
               << " + \"Marco\"): " << std::string("Polo ") + stringVariable << std::endl;
        bSuccess &= (std::string("Polo ") + stringVariable == "Polo Marco");

        /*
         * test the subtraction operator
         */

        stream << "Test operator - between VariableWrapper of type double and a short (4.0 - 1): "
               << doubleVariable - shortVal << std::endl;
        bSuccess &= (doubleVariable - shortVal == 3.0);
        stream << "Test operator - between VariableWrapper of type double and a long (4.0 - 2L): "
               << doubleVariable - longVal << std::endl;
        bSuccess &= (doubleVariable - longVal == 2.0);
        stream << "Test operator - between VariableWrapper of type double and a float (4.0 - 3.0f): "
               << doubleVariable - floatVal << std::endl;
        bSuccess &= (doubleVariable - floatVal == 1.0);
        stream << "Test operator - between VariableWrappers of type double and a double (4.0 - 4.0): "
               << doubleVariable - doubleVal << std::endl;
        bSuccess &= (doubleVariable - doubleVal == 0.0);
        stream << "Test operator - between VariableWrappers of types short and double (1 - 4.0): "
               << shortVariable - doubleVariable << std::endl;
        bSuccess &= (shortVariable - doubleVariable == -3.0);
        stream << "Test operator - between VariableWrappers of types long and double (2L - 4.0): "
               << longVariable - doubleVariable << std::endl;
        bSuccess &= (longVariable - doubleVariable == -2.0);
        stream << "Test operator - between VariableWrappers of types float and double (3.0f - 4.0): "
               << floatVariable - doubleVariable << std::endl;
        bSuccess &= (floatVariable - doubleVariable == -1.0);
        stream << "Test operator - between VariableWrappers of type double (4.0 - 4.0): "
               << doubleVariable - doubleVariable << std::endl;
        bSuccess &= (doubleVariable - doubleVariable == 0.0);
        stream << "Test operator - between a short and VariableWrapper of type double (1 - 4.0): "
               << shortVal - doubleVariable << std::endl;
        bSuccess &= (shortVal - doubleVariable == -3.0);
        stream << "Test operator - between a long and VariableWrapper of type double (2L - 4.0): "
               << longVal - doubleVariable << std::endl;
        bSuccess &= (longVal - doubleVariable == -2.0);
        stream << "Test operator - between a float and VariableWrapper of type double (3f - 4.0): "
               << floatVal - doubleVariable << std::endl;
        bSuccess &= (floatVal - doubleVariable == -1.0);
        stream << "Test operator - between a double and VariableWrapper of type double  (4.0 - 4.0): "
               << doubleVal - doubleVariable << std::endl;
        bSuccess &= (doubleVal - doubleVariable == 0.0);

        /*
         * test the unary plus/minus operator
         */

        stream << "Test doubleVariable unary + operator +(4.0): " << +doubleVariable << std::endl;
        bSuccess &= (+doubleVariable == 4.0);
        stream << "Test doubleVariable unary - (negate) operator -(4.0): " << -doubleVariable << std::endl;
        bSuccess &= (-doubleVariable == -4.0);

        /*
         * test the multiplication operator
         */

        stream << "Test operator * between VariableWrapper of type double and a short (4.0 * 1): "
               << doubleVariable * shortVal << std::endl;
        bSuccess &= (doubleVariable * shortVal == 4.0);
        stream << "Test operator * between VariableWrapper of type double and a long (4.0 * 2L): "
               << doubleVariable * longVal << std::endl;
        bSuccess &= (doubleVariable * longVal == 8.0);
        stream << "Test operator * between VariableWrapper of type double and a float (4.0 * 3f): "
               << doubleVariable * floatVal << std::endl;
        bSuccess &= (doubleVariable * floatVal == 12.0);
        stream << "Test operator * between VariableWrapper of type double and a double (4.0 * 4.0): "
               << doubleVariable * doubleVal << std::endl;
        bSuccess &= (doubleVariable * doubleVal == 16.0);
        stream << "Test operator * between VariableWrappers of types short and double (1 * 4.0): "
               << shortVariable * doubleVariable << std::endl;
        bSuccess &= (shortVariable * doubleVariable == 4.0);
        stream << "Test operator * between VariableWrappers of types long and double (2L * 4.0): "
               << longVariable * doubleVariable << std::endl;
        bSuccess &= (longVariable * doubleVariable == 8.0);
        stream << "Test operator * between VariableWrappers of types float and double (3.0f * 4.0): "
               << floatVariable * doubleVariable << std::endl;
        bSuccess &= (floatVariable * doubleVariable == 12.0);
        stream << "Test operator * between VariableWrappers of type double (4.0 * 4.0): "
               << doubleVariable * doubleVariable << std::endl;
        bSuccess &= (doubleVariable * doubleVariable == 16.0);
        stream << "Test operator * between a short and VariableWrapper of type double (1 * 4.0): "
               << shortVal * doubleVariable << std::endl;
        bSuccess &= (shortVal * doubleVariable == 4.0);
        stream << "Test operator * between a long and VariableWrapper of type double (2L * 4.0): "
               << longVal * doubleVariable << std::endl;
        bSuccess &= (longVal * doubleVariable == 8.0);
        stream << "Test operator * between a float and VariableWrapper of type double (3f * 4.0): "
               << floatVal * doubleVariable << std::endl;
        bSuccess &= (floatVal * doubleVariable == 12.0);
        stream << "Test operator * between a double and VariableWrapper of type double (4.0 * 4.0): "
               << doubleVal * doubleVariable << std::endl;
        bSuccess &= (doubleVal * doubleVariable == 16.0);

        /*
         * test the division operator
         */

        stream << "Test operator / between VariableWrapper of type double and a short (4.0 / 1): "
               << doubleVariable / shortVal << std::endl;
        bSuccess &= (doubleVariable / shortVal == 4.0);
        stream << "Test operator / between VariableWrapper of type double and a long (4.0 / 2L): "
               << doubleVariable / longVal << std::endl;
        bSuccess &= (doubleVariable / longVal == 2.0);
        stream << "Test operator / between VariableWrapper of type double and a float (4.0 / 3f): "
               << doubleVariable / floatVal << std::endl;
        bSuccess &= (doubleVariable / floatVal == 4.0 / 3.0);
        stream << "Test operator / between VariableWrapper of type double and a double (4.0 / 4.0): "
               << doubleVariable / doubleVal << std::endl;
        bSuccess &= (doubleVariable / doubleVal == 1.0);
        stream << "Test operator / between VariableWrappers of types short and double (1 / 4.0): "
               << shortVariable / doubleVariable << std::endl;
        bSuccess &= (shortVariable / doubleVariable == 0.0);
        stream << "Test operator / between VariableWrappers of types long and double (2L / 4.0): "
               << longVariable / doubleVariable << std::endl;
        bSuccess &= (longVariable / doubleVariable == 0.0);
        stream << "Test operator / between VariableWrappers of types float and double (3.0f / 4.0): "
               << floatVariable / doubleVariable << std::endl;
        bSuccess &= (floatVariable / doubleVariable == 3.0 / 4.0);
        stream << "Test operator / between VariableWrappers of type double (4.0 / 4.0): "
               << doubleVariable / doubleVariable << std::endl;
        bSuccess &= (doubleVariable / doubleVariable == 1.0);
        stream << "Test operator / between a short and VariableWrapper of type double (1 / 4.0): "
               << shortVal / doubleVariable << std::endl;
        bSuccess &= (shortVal / doubleVariable == 0.0);
        stream << "Test operator / between a long and VariableWrapper of type double (2L / 4.0): "
               << longVal / doubleVariable << std::endl;
        bSuccess &= (longVal / doubleVariable == 0.0);
        stream << "Test operator / between a float and VariableWrapper of type double (3f / 4.0): "
               << floatVal / doubleVariable << std::endl;
        bSuccess &= (floatVal / doubleVariable == 3.0 / 4.0);
        stream << "Test operator / between a double and VariableWrapper of type double (4.0 / 4.0): "
               << doubleVal / doubleVariable << std::endl;
        bSuccess &= (doubleVal / doubleVariable == 1.0);

        /*
         * test the modulo operator
         */

        VariableWrapper<int> intVariable(5);

        stream << "Test operator % between VariableWrapper of type int and a short (5 % 1): "
               << intVariable % shortVal << std::endl;
        bSuccess &= (intVariable % shortVal == 0);
        stream << "Test operator % between VariableWrapper of type int and a long (5 % 2L): "
               << intVariable % longVal << std::endl;
        bSuccess &= (intVariable % longVal == 1);
        stream << "Test operator % between VariableWrappers of types int and short (5 % 1): "
               << intVariable % shortVariable << std::endl;
        bSuccess &= (intVariable % shortVariable == 0);
        stream << "Test operator % between VariableWrappers of types int and long (1 % 2L): "
               << intVariable % longVariable << std::endl;
        bSuccess &= (intVariable % longVariable == 1);
        stream << "Test operator % between a short and VariableWrapper of type int (1 % 5): "
               << shortVal % intVariable << std::endl;
        bSuccess &= (shortVal % intVariable == 1);
        stream << "Test operator % between a long and VariableWrapper of type int (2L % 5): "
               << longVal % intVariable << std::endl;
        bSuccess &= (longVal % intVariable == 2);

        /*
         * test prefix/suffix increment/decrement operators
         */

        stream << "Test operator ++ suffix with VariableWrapper of type int (5++): " << intVariable++ << std::endl;
        bSuccess &= (intVariable == 6);
        stream << "Test operator ++ prefix with VariableWrapper of type int (++6): " << ++intVariable << std::endl;
        bSuccess &= (intVariable == 7);
        stream << "Test operator -- suffix with VariableWrapper of type int (7--): " << intVariable-- << std::endl;
        bSuccess &= (intVariable == 6);
        stream << "Test operator -- prefix with VariableWrapper of type int (--6): " << --intVariable << std::endl;
        bSuccess &= (intVariable == 5);

        /*
         * verify that references are not being 'copied', but are instead being wrapped in a copyable object
         */

        stream << "Reference variable for short integer "
               << (&shortVal == &(short &)shortVariable ? "was" : "wasn't") << " successfully created." << std::endl;
        bSuccess &= (&shortVal == &(short &)shortVariable);
        stream << "Reference variable for long integer "
               << (&longVal == &(long &)longVariable ? "was" : "wasn't") << " successfully created." << std::endl;
        bSuccess &= (&longVal == &(long &)longVariable);
        stream << "Reference variable for float "
               << (&floatVal == &(float &)floatVariable ? "was" : "wasn't") << " successfully created." << std::endl;
        bSuccess &= (&floatVal == &(float &)floatVariable);
        stream << "Reference variable for double "
               << (&doubleVal == &(double &)doubleVariable ? "was" : "wasn't") << " successfully created." << std::endl;
        bSuccess &= (&doubleVal == &(double &)doubleVariable);

        /*
         * test comparison operators
         */

        VariableWrapper<double> otherDoubleVariable(5);

        stream << "Test operator == between VariableWrappers of types double and int (5.0 == 5): "
               << (otherDoubleVariable == intVariable ? "true" : "false") << std::endl;
        bSuccess &= (otherDoubleVariable == intVariable);
        stream << "Test operator != between VariableWrappers of type double (4.0 != 5.0): "
               << (doubleVariable != otherDoubleVariable ? "true" : "false") << std::endl;
        bSuccess &= (doubleVariable != otherDoubleVariable);
        stream << "Test operator >  between VariableWrappers of type double (4.0 > 5.0): "
               << (doubleVariable > otherDoubleVariable ? "true" : "false") << std::endl;
        bSuccess &= !(doubleVariable > otherDoubleVariable);
        stream << "Test operator <  between VariableWrappers of type double (4.0 < 5.0): "
               << (doubleVariable < intVariable ? "true" : "false") << std::endl;
        bSuccess &= (doubleVariable < intVariable);
        stream << "Test operator >= between VariableWrappers of type double (4.0 >= 5.0): "
               << (doubleVariable >= otherDoubleVariable ? "true" : "false") << std::endl;
        bSuccess &= !(doubleVariable >= otherDoubleVariable);
        stream << "Test operator <= between VariableWrappers of type double (4.0 <= 4): "
               << (doubleVariable <= intVariable - 1 ? "true" : "false") << std::endl;
        bSuccess &= (doubleVariable <= intVariable - 1);
        stream << "Test operator == between a VariableWrapper of type double and an int (5.0 == 5): "
               << (otherDoubleVariable == 5 ? "true" : "false") << std::endl;
        bSuccess &= (otherDoubleVariable == 5);
        stream << "Test operator != between a VariableWrapper of type double and a double (4.0 != 5.0): "
               << (doubleVariable != 5.0 ? "true" : "false") << std::endl;
        bSuccess &= (doubleVariable != 5.0);
        stream << "Test operator >  between a VariableWrapper of type double and a double (4.0 > 5.0): "
               << (doubleVariable > 5.0 ? "true" : "false") << std::endl;
        bSuccess &= !(doubleVariable > 5.0);
        stream << "Test operator <  between a VariableWrapper of type double and a double (4.0 < 5.0): "
               << (doubleVariable < 5.0 ? "true" : "false") << std::endl;
        bSuccess &= (doubleVariable < 5.0);
        stream << "Test operator >= between a VariableWrapper of type double and a double (4.0 >= 5.0): "
               << (doubleVariable >= 5.0 ? "true" : "false") << std::endl;
        bSuccess &= !(doubleVariable >= 5.0);
        stream << "Test operator <= between a VariableWrapper of type double and an int (4.0 <= 4): "
               << (doubleVariable <= 4 ? "true" : "false") << std::endl;
        bSuccess &= (doubleVariable <= 4);
        stream << "Test operator == between a VariableWrapper of type double and an int (5.0 == 5): "
               << (5.0 == intVariable ? "true" : "false") << std::endl;
        bSuccess &= (5.0 == intVariable);
        stream << "Test operator != between a VariableWrapper of type double and a double (4.0 != 5.0): "
               << (4.0 != otherDoubleVariable ? "true" : "false") << std::endl;
        bSuccess &= (4.0 != otherDoubleVariable);
        stream << "Test operator >  between a VariableWrapper of type double and a double (4.0 > 5.0): "
               << (4.0 > otherDoubleVariable ? "true" : "false") << std::endl;
        bSuccess &= !(4.0 > otherDoubleVariable);
        stream << "Test operator <  between a VariableWrapper of type double and a double (4.0 < 5.0): "
               << (4.0 < intVariable ? "true" : "false") << std::endl;
        bSuccess &= (4.0 < intVariable);
        stream << "Test operator >= between a VariableWrapper of type double and a double (4.0 >= 5.0): "
               << (4.0 >= otherDoubleVariable ? "true" : "false") << std::endl;
        bSuccess &= !(4.0 >= otherDoubleVariable);
        stream << "Test operator <= between a VariableWrapper of type double and an int (4.0 <= 4): "
               << (4.0 <= intVariable ? "true" : "false") << std::endl;
        bSuccess &= (4.0 <= intVariable);

        /*
         * test logical operators
         */

        VariableWrapper<bool> boolVariable(true);
        VariableWrapper<bool> otherBoolVariable(false);

        stream << "Test operator ! with a VariableWrapper of type bool (!true): "
               << (!boolVariable ? "true" : "false") << std::endl;
        bSuccess &= !(!boolVariable);
        stream << "Test operator && with VariableWrappers of type bool (true && false): "
               << (boolVariable && otherBoolVariable ? "true" : "false") << std::endl;
        bSuccess &= !(boolVariable && otherBoolVariable);
        stream << "Test operator && with VariableWrapper of type bool and a constant 'true' (true && true): "
               << (boolVariable && true ? "true" : "false") << std::endl;
        bSuccess &= (boolVariable && true);
        stream << "Test operator && with a constant 'true' and VariableWrapper of type bool (true && !false): "
               << (true && !otherBoolVariable ? "true" : "false") << std::endl;
        bSuccess &= (true && !otherBoolVariable);
        stream << "Test operator || with VariableWrappers of type bool (true || false): "
               << (boolVariable || otherBoolVariable ? "true" : "false") << std::endl;
        bSuccess &= (boolVariable || otherBoolVariable);
#if 0
        stream << "Test operator || with VariableWrapper of type bool and a constant 'true' (true || true): "
               << (boolVariable || true ? "true" : "false") << std::endl;
        bSuccess &= (boolVariable || true);
#endif
        stream << "Test operator || with a constant 'false' and VariableWrapper of type bool (false || !false): "
               << (false || !otherBoolVariable ? "true" : "false") << std::endl;
        bSuccess &= (false || !otherBoolVariable);

        /*
         * test bitwise operators
         */

        longVal = 48384432L;                              // 10 1110 0010 0100 1001 1011 0000
        VariableWrapper<long> otherLongVariable(536543L); // 00 0000 1000 0010 1111 1101 1111
        longVariable = longVal;

        stream << "Test bitwise ~ operator on VariableWrapper of type long (~536,543L = -536,544L): "
               << ~otherLongVariable << std::endl;
        bSuccess &= (~otherLongVariable == -536544L);
        stream << "Test bitwise & operator on two VariableWrappers of type long (48,384,432L & 536,543L"
               << " = 2,448L): " << (longVariable & otherLongVariable) << std::endl;
        bSuccess &= ((longVariable & otherLongVariable) == 2448L);
        stream << "Test bitwise & operator on a long and a VariableWrapper of type long (48,384,432L &"
               << " 536,543L = 2,448L): " << (48384432L & otherLongVariable) << std::endl;
        bSuccess &= ((48384432L & otherLongVariable) == 2448L);
        stream << "Test bitwise & operator on a VariableWrapper of type long and a long (48,384,432L &"
               << " 536,543L = 2,448L): " << (longVariable & 536543L) << std::endl;
        bSuccess &= ((longVariable & 536543L) == 2448L);
        stream << "Test bitwise | operator on two VariableWrappers of type long (48,384,432L | 536,543L"
               << " = 48,918,527L): " << (longVariable | otherLongVariable) << std::endl;
        bSuccess &= ((longVariable | otherLongVariable) == 48918527L);
        stream << "Test bitwise | operator on a long and a VariableWrapper of type long (48,384,432L |"
               << " 536,543L = 48,918,527L): " << (48384432L | otherLongVariable) << std::endl;
        bSuccess &= ((48384432L | otherLongVariable) == 48918527L);
        stream << "Test bitwise | operator on a VariableWrapper of type long and a long (48,384,432L |"
               << " 536,543L = 48,918,527L): " << (longVariable | 536543L) << std::endl;
        bSuccess &= ((longVariable | 536543L) == 48918527L);
        stream << "Test bitwise ^ operator on two VariableWrappers of type long (48,384,432L ^ 536,543L"
               << " = 48,916,079L): " << (longVariable ^ otherLongVariable) << std::endl;
        bSuccess &= ((longVariable ^ otherLongVariable) == 48916079L);
        stream << "Test bitwise ^ operator on a long and a VariableWrapper of type long (48,384,432L ^ 536,543L"
               << " = 48,916,079L): " << (48384432L ^ otherLongVariable) << std::endl;
        bSuccess &= ((48384432L ^ otherLongVariable) == 48916079L);
        stream << "Test bitwise ^ operator on a VariableWrapper of type long and a long (48,384,432L ^ 536,543L"
               << " = 48,916,079L): " << (longVariable ^ 536543L) << std::endl;
        bSuccess &= ((longVariable ^ 536543L) == 48916079L);
        stream << "Test bitwise << operator on two VariableWrappers of type long (48,384,432L << 3L ="
               << " 387,075,456L): " << (longVariable << (otherLongVariable = 3L)) << std::endl;
        bSuccess &= ((longVariable << otherLongVariable) == 387075456L);
        stream << "Test bitwise << operator on a long and a VariableWrapper of type long (48,384,432L"
               << " << 3L = 387,075,456L): " << (longVariable << otherLongVariable) << std::endl;
        bSuccess &= ((longVariable << otherLongVariable) == 387075456L);
        stream << "Test bitwise << operator on a VariableWrapper of type long and a long (48,384,432L"
               << " << 3L = 387,075,456L): " << (longVariable << 3L) << std::endl;
        bSuccess &= ((longVariable << 3L) == 387075456L);
        stream << "Test bitwise >> operator on two VariableWrappers of type long (48,384,432L >> 3L"
               << " = 6,048,054L): " << (longVariable >> (otherLongVariable = 3L)) << std::endl;
        bSuccess &= ((longVariable >> otherLongVariable) == 6048054L);
        stream << "Test bitwise >> operator on a long and a VariableWrapper of type long (48,384,432L"
               << " >> 3L = 6,048,054L): " << (longVariable >> otherLongVariable) << std::endl;
        bSuccess &= ((longVariable >> otherLongVariable) == 6048054L);
        stream << "Test bitwise >> operator on a VariableWrapper of type long and a long (48,384,432L"
               << " >> 3L = 6,048,054L): " << (longVariable >> 3L) << std::endl;
        bSuccess &= ((longVariable >> 3L) == 6048054L);

        /*
         * test compound assignment operators
         */

        int intVal = 9;
        floatVal = 3.3f;
        longVariable = 5L;

        stream << "Test += operator on VariableWrappers of types long and float (5L += 3.3f): "
               << (longVariable += floatVariable) << std::endl;
        bSuccess &= (longVariable == 8);
        stream << "Test += operator on a float and VariableWrapper of type long (3.3f += 8L): "
               << (floatVal += longVariable) << std::endl;
        bSuccess &= (int(10 * floatVal) / 10.0 == 11.3);
        stream << "Test += operator on a VariableWrapper of type long and a float (8L += 11.3f): "
               << (longVariable += floatVal) << std::endl;
        bSuccess &= (longVariable == 19);

        stringVariable = "Hello";
        VariableWrapper<std::string &> otherStringVariable("is there anyone home?");

        stream << "Test += operator on VariableWrappers of types std::string and char array: "
               << (stringVariable += "...is there anybody in there?") << std::endl;
        bSuccess &= (stringVariable == "Hello...is there anybody in there?");
        stream << "Test += operator on an std::string and VariableWrapper of type std::string: "
               << (stringVariable += std::string(" Just nod if you can hear me...")) << std::endl;
        bSuccess &= (stringVariable == "Hello...is there anybody in there? Just nod if you can hear me...");
        stream << "Test += operator on VariableWrappers of type std::string: "
               << (stringVariable += otherStringVariable) << std::endl;
        bSuccess &= (stringVariable == "Hello...is there anybody in there? Just nod if you can hear me..."
                                       "is there anyone home?");
        stream << "Test -= operator on VariableWrappers of types long and float (19L -= 11.3f): "
               << (longVariable -= floatVariable) << std::endl;
        bSuccess &= (longVariable == 7L);
        stream << "Test -= operator on a float and VariableWrapper of type long (11.3f -= 7L): "
               << (floatVal -= longVariable) << std::endl;
        bSuccess &= (int(10 * floatVal) / 10.0 == 4.3);
        stream << "Test -= operator on a VariableWrapper of type long and a float (7L -= 4.3f): "
               << (longVariable -= floatVal) << std::endl;
        bSuccess &= (longVariable == 2L);
        stream << "Test *= operator on VariableWrappers of types long and float (2L *= 4.3f): "
               << (longVariable *= floatVariable) << std::endl;
        bSuccess &= (longVariable == 8L);
        stream << "Test *= operator on a float and VariableWrapper of type long (4.3f *= 8L): "
               << (floatVal *= longVariable) << std::endl;
        bSuccess &= (int(10 * floatVal) / 10.0 == 34.4);
        stream << "Test *= operator on a VariableWrapper of type long and a float int(8L *= 34.4f): "
               << (longVariable *= floatVal) << std::endl;
        bSuccess &= (longVariable == 275L);
        stream << "Test /= operator on VariableWrappers of types long and float (275L /= 34.4f): "
               << (longVariable /= floatVariable) << std::endl;
        bSuccess &= (longVariable == 7L);
        stream << "Test /= operator on a float and VariableWrapper of type long (34.4f /= 7L): "
               << (floatVal /= longVariable) << std::endl;
        bSuccess &= (int(1e6 * floatVal) / 1.0e6 == 4.914286);
        stream << "Test /= operator on a VariableWrapper of type long and a float (7L /= 4.91429f): "
               << (longVariable /= floatVal) << std::endl;
        bSuccess &= (longVariable == 1L);
        stream << "Test %= operator on VariableWrappers of types long and short (5L %= 1): "
               << (longVariable %= shortVariable) << std::endl;
        bSuccess &= (longVariable == 0);
        stream << "Test %= operator on a short and VariableWrapper of type long (1 %= 2L): "
               << (shortVal %= (longVariable + 2)) << std::endl;
        bSuccess &= (shortVal == 1);
        stream << "Test %= operator on a VariableWrapper of type long and an int (2L %= 9): "
               << ((longVariable = 2L) %= intVal) << std::endl;
        bSuccess &= (longVariable == 2L);

        shortVal = 443;              // 00 0000 0000 0000 0001 1011 1011
        longVal = 48384433L;         // 10 1110 0010 0100 1001 1011 0001
        otherLongVariable = 536543L; // 00 0000 1000 0010 1111 1101 1111

        stream << "Test &= operator on VariableWrappers of types long and short (48384433L &= 443): "
               << (longVariable &= shortVariable) << std::endl;
        bSuccess &= (longVariable == 433L);
        stream << "Test &= operator on a short and VariableWrapper of type long (443 &= 433L): "
               << (shortVal &= longVariable) << std::endl;
        bSuccess &= (shortVal == 433L);
        stream << "Test &= operator on a VariableWrapper of type long and an int (433L &= 9): "
               << (longVariable &= intVal) << std::endl;
        bSuccess &= (longVariable == 1L);
        stream << "Test |= operator on VariableWrappers of types long and short (1L |= 433): "
               << (longVariable |= shortVariable) << std::endl;
        bSuccess &= (longVariable == 433L);
        stream << "Test |= operator on a short and VariableWrapper of type long (433 |= 433L): "
               << (shortVal |= longVariable) << std::endl;
        bSuccess &= (shortVal == 433);
        stream << "Test |= operator on a VariableWrapper of type long and an int (433L |= 9): "
               << (longVariable |= intVal) << std::endl;
        bSuccess &= (longVariable == 441L);
        stream << "Test ^= operator on VariableWrappers of types long and short (441L ^= 433): "
               << (longVariable ^= shortVariable) << std::endl;
        bSuccess &= (longVariable == 8L);
        stream << "Test ^= operator on a short and VariableWrapper of type long (433 ^= 8L): "
               << (shortVal ^= longVariable) << std::endl;
        bSuccess &= (shortVal == 441);
        stream << "Test ^= operator on a VariableWrapper of type long and an int (8L ^= 9): "
               << (longVariable ^= intVal) << std::endl;
        bSuccess &= (longVariable == 1L);

        shortVariable = 2;
        longVariable = 3;
        intVal = 4;

        stream << "Test <<= operator on VariableWrappers of types long and short (3L <<= 2): "
               << (longVariable <<= shortVariable) << std::endl;
        bSuccess &= (longVariable == 12L);
        stream << "Test <<= operator on a short and VariableWrapper of type long (2 <<= 12L): "
               << (shortVal <<= longVariable) << std::endl;
        bSuccess &= (shortVal == 8192);
        stream << "Test <<= operator on a VariableWrapper of type long and an int (12L <<= 4): "
               << (longVariable <<= intVal) << std::endl;
        bSuccess &= (longVariable == 192L);

        shortVariable = 2;
        longVariable = 7;
        intVal = 4;

        stream << "Test >>= operator on VariableWrappers of types long and short (7L >>= 2): "
               << (longVariable >>= shortVariable) << std::endl;
        bSuccess &= (longVariable == 1L);
        stream << "Test >>= operator on a short and VariableWrapper of type long (2 >>= 1L): "
               << (shortVal >>= longVariable) << std::endl;
        bSuccess &= (shortVal == 1);
        stream << "Test >>= operator on a VariableWrapper of type long and an int (1L >>= 4): "
               << (longVariable >>= intVal) << std::endl;
        bSuccess &= (longVariable == 0);

        stream.flush();
        stream.close();
    }

    std::cout << "Test " << (bSuccess ? "PASSED" : "FALIED") << "." << std::endl << std::endl;

    return bSuccess;
}

}
