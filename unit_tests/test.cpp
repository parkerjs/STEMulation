#include "cli_argument_processor.h"
#include "static_message_dispatcher.h"
#include "toggleable_stream.h"
#include "unitTestManager.h"

// using namespace declarations
using namespace attributes::interfaces;
using namespace functional;
using namespace messaging;
using namespace unit_tests;
using namespace utilities;

/**
 * Main
 */
int main(int argc, char **argv)
{
    std::cout << std::endl;
    std::cout << "*********************************" << std::endl;
    std::cout << "*** STEMulation++ Unit Tester ***" << std::endl;
    std::cout << "*********************************" << std::endl << std::endl;

    // define a mutex to control output to streams across multiple threads
    std::mutex std_out_mutex;
    std::map<std::string, std::mutex *> mutexMap = { { "std_out_mutex", &std_out_mutex } };

    // define toggleable streams
    ToggleableStream debug_stream(std::cout);
    ToggleableStream error_stream(std::cout);
    ToggleableStream usage_stream(std::cout);
    ToggleableStream warning_stream(std::cout);
    std::map<std::string, ToggleableStream *> toggleableStreamMap{ { "debug", &debug_stream },
                                                                   { "error", &error_stream },
                                                                   { "usage", &usage_stream },
                                                                   { "warning", &warning_stream } };

    // set this executable's CLI argument processor
    CLI_ArgumentProcessor cliArgumentProcessor(toggleableStreamMap);
    auto *pCLI_ArgumentProessor = &cliArgumentProcessor;
    pCLI_ArgumentProessor->removeProcessedVariablesFromRegistry(true);

    // configuration token map
    TokenMapConfigurable::tTokenMap tokenMap;

    // define vector of recipient-message vector pairs
    std::vector<std::pair<std::string, std::vector<Any>>> recipientMessageVectorPairs =
    {
        { "StaticSynchronizable::setMutexMap<std::string, std::mutex *>", { mutexMap } },
        { "Synchronizable<std::string, std::mutex>::setMutexMap" , { mutexMap } },
        { "Loggable<std::string, ToggleableStream>::setLoggingStreamMap", { toggleableStreamMap } },
        { "StaticLoggable::setLoggingStreamMap<std::string, ToggleableStream>", { toggleableStreamMap } },
        { "CLI_ArgumentProcessable::setCLI_ArgumentProcessor", { pCLI_ArgumentProessor } },
        { "StaticCLI_ArgumentProcessable::setStaticCLI_ArgumentProcessor", { pCLI_ArgumentProessor } },
        { "Setupable::setup", { } },
        { "StaticSetupable::staticSetup", { } },
        { "CLI_Configurable::configure", { argc, argv } },
        { "StaticCLI_Configurable::staticConfigure", { argc, argv } },
        { "StockAnalyzerExecutiveConfigurationMapper::getConfiguration(tTokenMap &)", { tokenMap } },
        { "TokenMapConfigurable::configure", { tokenMap } },
        { "StaticTokenMapConfigurable::staticConfigure", { tokenMap } },
        { "CLI_UsageMenuPrintable::printUsage", { std::ref(std::cout) } },
        { "Initializable::initialize", { } },
        { "StaticInitializable::staticInitialize", { } },
        { "Executable::execute", { } },
        { "StaticExecutable::staticExecute", { } }
    };

    // define a message publisher
    Publisher publisher;

    // create a subscriber for static message recipients
    Subscriber subscriber(StaticMessageDispatcher::getMessageDispatcher());
    publisher.addSubscriber(&subscriber);

    // create an instance of UnitTestManager
    auto &&pUnitTestManager = std::unique_ptr<UnitTestManager>(UnitTestManager::create(&publisher));

    bool bSuccess = publisher.notify(recipientMessageVectorPairs);
    if (bSuccess)
        std::cout << "Message from main(): Successfully executed, closing..."
                  << std::endl << std::endl;
    else
        std::cout << "Error from main(): An error occurred..." << std::endl << std::endl;

    return bSuccess ? 0 : 1;
}
