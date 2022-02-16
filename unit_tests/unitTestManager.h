#ifndef UNIT_TEST_MANAGER_H
#define UNIT_TEST_MANAGER_H

#include "cli_argument_processable.h"
#include "cli_configurable.h"
#include "dependency_injectable.h"
#include "executable.h"
#include "loggable.h"
#include "priority_subscriber.h"
#include "priorityPublisher.h"
#include "reflective.h"
#include "setupable.h"
#include "synchronizable.h"
#include "toggleable_stream.h"
#include "token_map_configurable.h"
#include "variable_map_registrable.h"
#include <mutex>
#include <set>

// forward declarations
namespace utilities { class CLI_ArgumentProcessor; }

namespace unit_tests
{

/**
 * This class manages execution of unit tests
 */
class UnitTestManager
: public attributes::interfaces::CLI_ArgumentProcessable<utilities::CLI_ArgumentProcessor>,
  public attributes::interfaces::CLI_Configurable,
  public attributes::concrete::DependencyInjectable<messaging::Publisher *>,
  public attributes::interfaces::Executable,
  public attributes::concrete::Loggable<std::string, utilities::ToggleableStream>,
  public messaging::PriorityPublisher,
  public messaging::PrioritySubscriber<attributes::interfaces::
                                       CLI_ArgumentProcessable<utilities::CLI_ArgumentProcessor>,
                                       attributes::interfaces::CLI_Configurable,
                                       attributes::interfaces::Executable,
                                       attributes::concrete::Loggable<std::string,
                                                                      utilities::ToggleableStream>,
                                       attributes::interfaces::Setupable,
                                       attributes::concrete::Synchronizable<std::string, std::mutex>>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Setupable,
  public attributes::concrete::Synchronizable<std::string, std::mutex>,
  public attributes::interfaces::TokenMapConfigurable,
  public attributes::concrete::VariableMapRegistrable
{
public:

    /**
     * Using declarations
     */
    using Synchronizable<int, std::recursive_mutex>::getMutex;
    using Synchronizable<std::string, std::mutex>::getMutex;

private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    UnitTestManager(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    UnitTestManager(const UnitTestManager &manager) = delete;

    /**
     * Move constructor
     */
    UnitTestManager(UnitTestManager &&manager) = delete;

public:

    /**
     * Destructor
     */
    ~UnitTestManager(void);

private:

    /**
     * Copy assignment operator
     */
    UnitTestManager &operator = (const UnitTestManager &manager) = delete;

    /**
     * Move assignment operator
     */
    UnitTestManager &operator = (UnitTestManager &&manager) = delete;

public:

    /**
     * Configure function (using command-line arguments)
     * @param argc the argument count
     * @param argv an array of command-line arguments
     */
    virtual bool configure(int argc, char **argv) override;

    /**
     * Configure function (using a variable token map)
     */
    virtual bool configure(tTokenMap &tokenMap) override;

    /**
     * create() function
     * @param dependencies a tuple of this object's required injection dependencies
     */
    static UnitTestManager *create(const tDependencies &dependencies);

    /**
     * Execution function
     */
    virtual bool execute(void) override;

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "UnitTestManager";
    }

    /**
     * Get a pointer to this object's CLI argument processor
     */
    inline virtual utilities::CLI_ArgumentProcessor *getCLI_ArgumentProcessor(void) override final
    {
        return m_pCLI_ArgumentProcessor;
    }

    /**
     * Set a pointer to this object's CLI argument processor
     */
    inline virtual void
    setCLI_ArgumentProcessor(utilities::CLI_ArgumentProcessor *pCLI_ArgumentProcessor) override final
    {
        m_pCLI_ArgumentProcessor = pCLI_ArgumentProcessor;
    }

    /**
     * Setup function
     */
    virtual bool setup(void) override;

private:

    /**
     * pointer to CLI argument processor
     */
    utilities::CLI_ArgumentProcessor *m_pCLI_ArgumentProcessor;

    /**
     * token configuration map
     */
    tTokenMap m_tokenMap;

    /**
     * units queued for testing
     */
    std::set<std::string> m_unitsQueuedForTesting;
};

}

#endif
