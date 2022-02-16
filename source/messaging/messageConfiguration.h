#ifndef MESSAGE_CONFIGURATION_H
#define MESSAGE_CONFIGURATION_H

#include "cloneable.h"
#include "export_library.h"
#include "file_configurable.h"
#include "initializable.h"
#include "loggable.h"
#include "reflective.h"
#include "setupable.h"
#include "swappable.h"
#include "token_map_configurable.h"
#include "variable_map_registrable.h"
#include "xml_readable.h"

// forward declarations
namespace rapidxml { template<class> class xml_node; }

namespace messaging
{

/**
 * This class provides a container for message configuration
 */
class MessageConfiguration
: public attributes::interfaces::Cloneable<MessageConfiguration>,
  public attributes::interfaces::FileConfigurable,
  public attributes::interfaces::Initializable,
  public attributes::concrete::Loggable<std::string, std::ostream>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Setupable,
  public attributes::interfaces::Swappable<MessageConfiguration>,
  public attributes::interfaces::TokenMapConfigurable,
#ifdef RAPID_XML
  virtual public attributes::concrete::VariableMapRegistrable,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>
#else
  virtual public attributes::concrete::VariableMapRegistrable
#endif
{
public:

    /**
     * Using declarations
     */
    using Loggable<std::string, std::ostream>::swap;

    /**
     * Constructor
     */
    EXPORT_STEM MessageConfiguration(void);

    /**
     * Copy constructor
     */
    EXPORT_STEM MessageConfiguration(const MessageConfiguration &configuration);

    /**
     * Move constructor
     */
    EXPORT_STEM MessageConfiguration(MessageConfiguration &&configuration);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~MessageConfiguration(void);

    /**
     * Copy assignment operator
     */
    EXPORT_STEM MessageConfiguration &operator = (const MessageConfiguration &configuration);

    /**
     * Move assignment operator
     */
    EXPORT_STEM MessageConfiguration &operator = (MessageConfiguration &&configuration);

    /**
     * clone() function
     */
    EXPORT_STEM virtual MessageConfiguration *clone(void) const override;

    /**
     * Configure function (using input file or input string)
     */
    EXPORT_STEM virtual bool configure(const std::string &filename) override;

    /**
     * Configure function (using a variable token map)
     */
    EXPORT_STEM virtual bool configure(tTokenMap &tokenMap) override;

    /**
     * Function to construct an instance from an XML node
     * @param pNode a pointer to an XML node
     */
    static EXPORT_STEM MessageConfiguration *createFromXML(rapidxml::xml_node<char> *pNode);

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Retrieve the message priority
     */
    EXPORT_STEM virtual int getPriority(void) const final;

    /**
     * Retrieve the message receive processing delay
     */
    EXPORT_STEM virtual double getReceiveProcessingDelay(void) const final;

    /**
     * Retrieve the message send processing delay
     */
    EXPORT_STEM virtual double getSendProcessingDelay(void) const final;

    /**
     * Retrieve the message send update rate
     */
    EXPORT_STEM virtual double getSendUpdateRate(void) const final;

    /**
     * Retrieve the message type
     */
    EXPORT_STEM virtual std::string getType(void) const final;

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Set the message priority
     */
    EXPORT_STEM virtual void setPriority(std::size_t priority) final;

    /**
     * Set the message receive processing delay
     */
    EXPORT_STEM virtual void setReceiveProcessingDelay(double receiveProcessingDelay) final;

    /**
     * Set the message send processing delay
     */
    EXPORT_STEM virtual void setSendProcessingDelay(double sendProcessingDelay) final;

    /**
     * Set the message send update rate
     */
    EXPORT_STEM virtual void setSendUpdateRate(double sendUpdateRate) final;

    /**
     * Set the message type
     */
    EXPORT_STEM virtual void setType(const std::string &type) final;

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(MessageConfiguration &configuration) override final;

protected:

    /**
     * the message priority
     */
    int m_priority;

    /**
     * the message receive processing delay
     */
    double m_receiveProcessingDelay;

    /**
     * the message send processing delay
     */
    double m_sendProcessingDelay;

    /**
     * the message send update rate
     */
    double m_sendUpdateRate;

    /**
     * the message type
     */
    std::string m_type;
};

}

#endif
