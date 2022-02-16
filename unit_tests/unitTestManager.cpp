#include "cli_argument_processor.h"
#include "factory_constructible.h"
#include "file_system.h"
#include "unitTest.h"
#include "unitTestManager.h"

// using namespace declarations
using namespace attributes::abstract;
using namespace messaging;
using namespace utilities;
using namespace utilities::file_system;

namespace unit_tests
{

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
UnitTestManager::UnitTestManager(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  m_pCLI_ArgumentProcessor(nullptr)
{
    auto *pPublisher = getDependency<Publisher *>();
    if (pPublisher != nullptr)
        pPublisher->addSubscriber(this);

    getMessageDispatcher()->addRecipient("CLI_ArgumentProcessable::setCLI_ArgumentProcessor",
                                         &CLI_ArgumentProcessable::setCLI_ArgumentProcessor, this);
    getMessageDispatcher()->addRecipient("CLI_Configurable::configure",
                                         &CLI_Configurable::configure, this);
    getMessageDispatcher()->addRecipient("Executable::execute",
                                         &Executable::execute, this);
    getMessageDispatcher()->addRecipient("Loggable<std::string, ToggleableStream>::setLoggingStreamMap",
                                         &Loggable<std::string, ToggleableStream>::setLoggingStreamMap, this);
    getMessageDispatcher()->addRecipient("Setupable::setup",
                                         &Setupable::setup, this);
    getMessageDispatcher()->addRecipient("Synchronizable<std::string, std::mutex>::setMutexMap",
                                         &Synchronizable<std::string, std::mutex>::setMutexMap, this);
}

/**
 * Destructor
 */
UnitTestManager::~UnitTestManager(void)
{

}

/**
 * Configure function (using command-line arguments)
 * @param argc the argument count
 * @param argv an array of command-line arguments
 */
bool UnitTestManager::configure(int argc, char **argv)
{
    bool bSuccess = (m_pCLI_ArgumentProcessor != nullptr);
    if (bSuccess)
    {
        CLI_ArgumentProcessor::tTokenVectorPair optionArgumentsMap;
        bSuccess = m_pCLI_ArgumentProcessor->parse(argc, argv, optionArgumentsMap);
        if (bSuccess)
        {
            // process this object's variable registry
            Dictionary dictionary(&m_registry);
            bSuccess = m_pCLI_ArgumentProcessor->process(optionArgumentsMap, dictionary);
        }
        else
        {
            logMsg("error", LoggingLevel::Enum::Error,
                   "Failed to parse command-line argument list!\n\n",
                   getQualifiedMethodName(__func__));
        }
    }
    else
    {
        logMsg("error", LoggingLevel::Enum::Error,
               "Pointer to command-line argument processor is null!\n\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Configure function (using a variable token map)
 */
bool UnitTestManager::configure(tTokenMap &tokenMap)
{
    m_tokenMap = tokenMap;

    // process this object's variable registry
    Dictionary dictionary(&m_registry);

    return dictionary.populate(tokenMap);
}

/**
 * create() function
 * @param dependencies a tuple of this object's required injection dependencies
 */
UnitTestManager *UnitTestManager::create(const tDependencies &dependencies)
{
    UnitTestManager *pUnitTestManager = nullptr;
    if (UnitTestManager::dependenciesInitialized(dependencies))
        pUnitTestManager = new UnitTestManager(dependencies);

    return pUnitTestManager;
}

/**
 * Execution function
 */
bool UnitTestManager::execute(void)
{
    bool bSuccess = true; // assume success
    if (!m_unitsQueuedForTesting.empty())
    {
        std::cout << "Running unit test(s)..." << std::endl << std::endl;

        if (m_unitsQueuedForTesting.find("ALL") != m_unitsQueuedForTesting.cend())
        {
            m_unitsQueuedForTesting.erase("ALL");
            auto &&factoryMap = FactoryConstructible<UnitTest>::getFactoryMap();
            for (auto &&itNameFactoryPair : factoryMap)
                m_unitsQueuedForTesting.insert(itNameFactoryPair.first);
        }

        // instantiate all unit tests queued for testing
        std::vector<std::unique_ptr<UnitTest>> unitTests;
        for (auto &&unitToTest : m_unitsQueuedForTesting)
        {
            auto &&pUnitToTest = std::unique_ptr<UnitTest>(
                FactoryConstructible<UnitTest>::create(unitToTest, this));
            bSuccess &= (pUnitToTest != nullptr);
            if (bSuccess)
                unitTests.emplace_back(std::move(pUnitToTest));
            else
            {
                std::string error_string("No unit test named \"" + unitToTest + "\" exists!\n\n"
                                         "The following unit tests are available:\n");

                auto &&factoryMap = FactoryConstructible<UnitTest>::getFactoryMap();
                for (auto &&itNameFactoryPair : factoryMap)
                    error_string += "- " + itNameFactoryPair.first + "\n";

                error_string += "\n";

                logMsg("error", LoggingLevel::Enum::Error,
                       error_string,
                       getQualifiedMethodName(__func__));
            }

            if (!bSuccess)
                break;
        }

        if (bSuccess)
        {
            auto *pMutexMap = Synchronizable<std::string, std::mutex>::getMutexMap();
            if (pMutexMap != nullptr)
            {
                // notify all subscribers to refer to a single output mutex
                bSuccess = notify("Synchronizable<std::string, std::mutex>::setMutexMap", *pMutexMap);
            }

            // notify all subscribers to set the following ToggleableStream object references
            if (bSuccess)
            {
                auto &&loggingMap = getLoggingStreamMap();
                bSuccess = notify("Loggable<std::string, ToggleableStream>::setLoggingMap", loggingMap);
            }

            // perform setup
            if (bSuccess)
                bSuccess = notify("Setupable::setup");

            // perform token map configuration
            if (bSuccess)
                bSuccess = notify("TokenMapConfigurable::configure", m_tokenMap);

            for (auto &&pUnitTest : unitTests)
            {
                bSuccess = (pUnitTest != nullptr);
                if (bSuccess)
                    bSuccess = pUnitTest->execute();

                if (!bSuccess)
                    break;
            }
        }

        if (!bSuccess)
            logMsg("error", LoggingLevel::Enum::Error,
                   "One or more unit tests FAILED.\n\n",
                   getQualifiedMethodName(__func__));
        else
            logMsg("debug", LoggingLevel::Enum::Debug,
                   "All unit tests PASSED.\n\n",
                   getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Setup function
 */
bool UnitTestManager::setup(void)
{
    bool bSuccess = (m_pCLI_ArgumentProcessor != nullptr);
    if (bSuccess)
    {
        // add options to the CLI processor
        m_pCLI_ArgumentProcessor->addOption("-test", "test_1 [test_2] ... [test_n]",
                                            "Run unit tests specified by test_1 [test_2] ... [test_n].");

        // setup cli-configurable variables...
        m_registry["-test"] = m_unitsQueuedForTesting;
    }
    else
    {
        logMsg("error", LoggingLevel::Enum::Error,
               "Pointer to command-line argument processor is null!\n\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

}
