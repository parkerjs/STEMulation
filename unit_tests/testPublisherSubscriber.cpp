#include "any_configurable.h"
#include "cli_argument_processor.h"
#include "cli_configurable.h"
#include "file_configurable.h"
#include "priorityPublisher.h"
#include "registry_configurable.h"
#include "testPublisherSubscriber.h"
#include "token_map_configurable.h"
#include "unitTestManager.h"
#include "variable_map_registrable.h"
#include <chrono>
#include <iostream>
#include <thread>

// using namespace declarations
using namespace attributes::abstract;
using namespace attributes::concrete;
using namespace attributes::interfaces;
using namespace functional;
using namespace messaging;
using namespace utilities;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testPublisherSubscriber",
                                                    &PublisherSubscriberUnitTest::create);

/**
 * A priority publisher class
 */
class Notifier
: public PriorityPublisher
{
public:

    /**
     * Constructor
     */
    Notifier(void)
    {

    }

    /**
     * Destructor
     */
    ~Notifier(void)
    {

    }
};

/**
 * A priority subscriber class
 */
class Receiver
: public AnyConfigurable,
  public CLI_Configurable,
  public FileConfigurable,
  public PrioritySubscriber<AnyConfigurable,
                            CLI_Configurable,
                            FileConfigurable,
                            RegistryConfigurable,
                            TokenMapConfigurable>,
  public RegistryConfigurable,
  public TokenMapConfigurable,
  public VariableMapRegistrable
{
public:

    /**
     * Constructor
     */
    Receiver(Publisher *pPublisher)
    : m_a(0),
      m_argc(0),
      m_b(0),
      m_c(0.0),
      m_d(0.0f),
      m_i(0),
      m_j(0),
      m_pPublisher(pPublisher)
    {
        if (m_pPublisher != nullptr)
            m_pPublisher->addSubscriber(this);

        getMessageDispatcher()->addRecipient("AnyConfigurable::configure",
                                             &AnyConfigurable::configure, this);
        getMessageDispatcher()->addRecipient("CLI_Configurable::configure",
                                             &CLI_Configurable::configure, this);
        getMessageDispatcher()->addRecipient("FileConfigurable::configure",
                                             &FileConfigurable::configure, this);
        getMessageDispatcher()->addRecipient("RegistryConfigurable::configure",
                                             &RegistryConfigurable::configure, this);
        getMessageDispatcher()->addRecipient("TokenMapConfigurable::configure",
                                             &TokenMapConfigurable::configure, this);
    }

    /**
     * Move constructor
     */
    Receiver(Receiver &&receiver)
    : Receiver(receiver.m_pPublisher)
    {
        m_a = std::move(receiver.m_a);
        m_argc = std::move(receiver.m_argc);
        m_argv = std::move(receiver.m_argv);
        m_b = std::move(receiver.m_b);
        m_c = std::move(receiver.m_c);
        m_d = std::move(receiver.m_d);
        m_filename = std::move(receiver.m_filename);
        m_i = std::move(receiver.m_i);
        m_j = std::move(receiver.m_j);
        m_time_cli_configure = std::move(receiver.m_time_cli_configure);
        m_time_file_configure = std::move(receiver.m_time_file_configure);
        m_time_registry_configure = std::move(receiver.m_time_registry_configure);
        m_time_token_map_configure = std::move(receiver.m_time_token_map_configure);
    }

    /**
     * Destructor
     */
    virtual ~Receiver(void) override
    {

    }

    /**
     * Configure function (using a vector of Any objects)
     */
    virtual bool configure(const std::vector<Any> &anys) override
    {
        m_i = any_cast<int>(anys[0]);
        m_j = any_cast<int>(anys[1]);

        m_time_any_configure = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        return true;
    }

    /**
     * Configure function (using command-line arguments)
     * @param argc the argument count
     * @param argv an array of command-line arguments
     */
    virtual bool configure(int argc,
                           char **argv) override
    {
        m_argc = argc;
        for (int i = 0; i < argc; ++i)
            m_argv.push_back(argv[i]);

        m_time_cli_configure = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        return true;
    }

    /**
     * Configure function (using input file or input string)
     */
    virtual bool configure(const std::string &filename) override
    {
        m_filename = filename;
        m_time_file_configure = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        return !m_filename.empty();
    }

    /**
     * Configure function (using variable registry)
     */
    virtual bool configure(VariableRegistry &registry) override
    {
        m_registry["a"] = m_a;
        m_registry["b"] = m_b;
        m_registry["c"] = m_c;

        bool bSuccess = m_registry.update(registry);

        m_time_registry_configure = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        return bSuccess;
    }

    /**
     * Configure function (using a variable token map)
     */
    virtual bool configure(tTokenMap &tokenMap) override
    {
        VariableRegistry registry(m_registry);
        registry["d"] = m_d;

        Dictionary dictionary(&registry);
        bool bSuccess = dictionary.populate(tokenMap);

        m_time_token_map_configure = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        return bSuccess;
    }

    int m_a;
    int m_argc;
    std::vector<std::string> m_argv;
    int m_b;
    double m_c;
    float m_d;
    std::string m_filename;
    int m_i;
    int m_j;
    Publisher *m_pPublisher;
    std::chrono::high_resolution_clock::time_point m_time_any_configure;
    std::chrono::high_resolution_clock::time_point m_time_cli_configure;
    std::chrono::high_resolution_clock::time_point m_time_file_configure;
    std::chrono::high_resolution_clock::time_point m_time_registry_configure;
    std::chrono::high_resolution_clock::time_point m_time_token_map_configure;
};

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
PublisherSubscriberUnitTest::PublisherSubscriberUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
PublisherSubscriberUnitTest *PublisherSubscriberUnitTest::create(UnitTestManager *pUnitTestManager)
{
    PublisherSubscriberUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new PublisherSubscriberUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * unit test to verify publisher-subscriber messaging
 */
bool PublisherSubscriberUnitTest::execute(void)
{
    std::cout << "Starting unit test for implementation of the Publisher-Subscriber messaging pattern..."
              << std::endl << std::endl;

    Notifier notifier;
    std::vector<Receiver> receiver;
    receiver.emplace_back(Receiver(&notifier));
    receiver.emplace_back(Receiver(&notifier));

    // set the order of priority in which the subscribers receives messages for each of the following types;
    // instances that assign a higher priority (greater number) to any of the following types will receive
    // messages in advance of subscribers that assign a lower priority
    receiver[1].setPriority<AnyConfigurable>(1);
    receiver[1].setPriority<CLI_Configurable>(0);
    receiver[1].setPriority<FileConfigurable>(0);
    receiver[1].setPriority<RegistryConfigurable>(1);
    receiver[1].setPriority<TokenMapConfigurable>(1);
    receiver[0].setPriority<AnyConfigurable>(0);
    receiver[0].setPriority<CLI_Configurable>(1);
    receiver[0].setPriority<FileConfigurable>(1);
    receiver[0].setPriority<RegistryConfigurable>(0);
    receiver[0].setPriority<TokenMapConfigurable>(0);

    // perform file configuration first
    std::string filename("filename");
    bool bSuccess = notifier.notify("FileConfigurable::configure", filename);

    // test for file-configure success
    if (bSuccess)
        bSuccess = (receiver[0].m_filename == receiver[1].m_filename && receiver[0].m_filename == filename);

    // did subscriber one configure before subscriber two?
    if (bSuccess)
        bSuccess = (receiver[0].m_time_file_configure < receiver[1].m_time_file_configure);

    // now perform command line argument configuration
    int argc = 2;
    char **argv = CLI_ArgumentProcessor::createDynamicArgumentVector(std::vector<std::string>{ "one", "two" });
    if (bSuccess)
        bSuccess = notifier.notify("CLI_Configurable::configure", argc, argv);

    // test for cli-configure success
    if (bSuccess)
        bSuccess = (receiver[0].m_argc == receiver[1].m_argc && receiver[0].m_argc == argc &&
                    receiver[0].m_argv[0] == "one" && receiver[1].m_argv[1] == "two");

    // did subscriber one configure before subscriber two?
    if (bSuccess)
        bSuccess = (receiver[0].m_time_cli_configure < receiver[1].m_time_cli_configure);

    delete argv;

    // perform registry configuration
    int a = 1, b = 2;
    double c = 3.0;
    VariableRegistry registry;
    registry["a"] = a;
    registry["b"] = b;
    registry["c"] = c;
    if (bSuccess)
        bSuccess = notifier.notify("RegistryConfigurable::configure", registry);

    // test for registry-configure success
    if (bSuccess)
        bSuccess = (receiver[0].m_a == receiver[1].m_a && receiver[0].m_b == receiver[1].m_b &&
                    receiver[0].m_c == receiver[1].m_c && receiver[0].m_a == a && receiver[0].m_b == b &&
                    receiver[0].m_c == c);

    // did subscriber two configure before subscriber one?
    if (bSuccess)
        bSuccess = (receiver[0].m_time_registry_configure > receiver[1].m_time_registry_configure);

    // perform token map configuration
    TokenMapConfigurable::tTokenMap tokenMap{ { "d", "2.0" } };
    if (bSuccess)
        bSuccess = notifier.notify("TokenMapConfigurable::configure", tokenMap);

    // test for token map configure success
    if (bSuccess)
        bSuccess = (receiver[0].m_d == receiver[1].m_d && receiver[1].m_d == 2.0);

    // did subscriber two configure before subscriber one?
    if (bSuccess)
        bSuccess = (receiver[0].m_time_token_map_configure > receiver[1].m_time_token_map_configure);

    // perform any configuration
    int i = 42, j = 24;
    if (bSuccess)
        bSuccess = notifier.notify("AnyConfigurable::configure", std::vector<Any>{ std::ref(i), std::ref(j) });

    // test for any-configure success
    if (bSuccess)
        bSuccess = (receiver[0].m_i == receiver[1].m_i && receiver[0].m_j == receiver[1].m_j &&
                    receiver[0].m_i == 42 && receiver[0].m_j == 24);

    // did subscriber two configure before subscriber one?
    if (bSuccess)
        bSuccess = (receiver[0].m_time_any_configure > receiver[1].m_time_any_configure);

    std::cout << "Test " << (bSuccess ? "PASSED" : "FALIED") << "." << std::endl << std::endl;

    return bSuccess;
}

}
