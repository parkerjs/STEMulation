#ifndef DEADBAND_H
#define DEADBAND_H

#include "loggable.h"
#include "nonlinearity.h"

namespace math
{

namespace control_systems
{

namespace blocks
{

/**
 * This class implements a dead-band control block
 */
template<typename T>
class Deadband
: public attributes::concrete::Loggable<std::string, std::ostream>,
  public Nonlinearity<T>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Constructor
     */
    Deadband(void)
    : Deadband(0.0, 0.0)
    {

    }

    /**
     * Constructor
     * @param bandStart the band start value of the dead-band region
     * @param bandStop  the band stop value of the dead-band region
     */
    Deadband(T bandStart,
             T bandStop)
    : m_bandStart(bandStart),
      m_bandStop(bandStop)
    {

    }

    /**
     * Copy constructor
     */
    Deadband(const Deadband<T> &deadband)
    {
        operator = (deadband);
    }

    /**
     * Move constructor
     */
    Deadband(Deadband<T> &&deadband)
    {
        operator = (std::move(deadband));
    }

    /**
     * Destructor
     */
    virtual ~Deadband(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Deadband<T> &operator = (const Deadband<T> &deadband)
    {
        if (&deadband != this)
        {
            Loggable<std::string, std::ostream>::operator = (deadband);

            m_bandStart = deadband.m_bandStart;
            m_bandStop = deadband.m_bandStop;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Deadband<T> &operator = (Deadband<T> &&deadband)
    {
        if (&deadband != this)
        {
            Loggable<std::string, std::ostream>::operator = (std::move(deadband));

            m_bandStart = std::move(deadband.m_bandStart);
            m_bandStop = std::move(deadband.m_bandStop);
        }

        return *this;
    }

    /**
     * Function to apply this object's dead-band region to the input signal
     */
    virtual T apply(T signal)
    {
        // only apply the dead-band if the region is non-zero
        if (withinDeadband(signal))
        {
            signal = 0.0;
        }

        return signal;
    }

    /**
     * Get the band start value of the dead-band region
     */
    inline virtual T getBandStart(void) const final
    {
        return m_bandStart;
    }

    /**
     * Get the band stop value of the dead-band region
     */
    inline virtual T getBandStop(void) const final
    {
        return m_bandStop;
    }

    /**
     * Get the bandwidth of the dead-band region
     */
    inline virtual T getBandwidth(void) const final
    {
        return m_bandStop - bandStart;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "Deadband";
    }
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override
    {
        bool bSuccess = Nonlinearity<T>::readFromXML(pNode);
        if (bSuccess)
        {
            auto *pBandStartNode = pNode->first_node("band start");
            if (pBandStartNode != nullptr)
                m_bandStart = std::stod(pBandStartNode->value());

            auto *pBandStopNode = pNode->first_node("band stop");
            if (pBandStopNode != nullptr)
                m_bandStop = std::stod(pBandStopNode->value());
        }

        return bSuccess;
    }
#endif
    /**
     * Set the band start value of the dead-band region
     */
    inline virtual bool setBandStart(T bandStart) final
    {
        bool bSuccess = (m_bandStop >= bandStart);
        if (bSuccess)
        {
            m_bandStart = bandStart;
        }
        else
        {
            logMsg(std::cout, LoggingLevel::Enum::Warning,
                   "Band start value must be less or equal to band stop value.\n",
                   getQualifiedStaticMethodName(__func__, Limiter));
        }

        return bSuccess;
    }

    /**
     * Set the band stop value of the dead-band region
     */
    virtual bool setBandStop(T bandStop) const final
    {
        bool bSuccess = (bandStop >= m_bandStart);
        if (bSuccess)
        {
            m_bandStop = bandStop;
        }
        else
        {
            logMsg(std::cout, LoggingLevel::Enum::Warning,
                   "Band stop value must be greater or equal to band start value.\n",
                   getQualifiedStaticMethodName(__func__, Limiter));
        }

        return bSuccess;
    }

    /**
     * Function to test if the input signal is within this object's specified dead-band region
     */
    inline virtual bool withinDeadband(T signal) const final
    {
        // only apply the dead-band if the region is non-zero
        return getBandwidth() > 0.0 &&
               signal >= m_startBand &&
               signal <= m_stopBand;
    }

protected:

    /**
     * the band start value of the dead-band region
     */
    T m_bandStart;

    /**
     * the band stop value of the dead-band region
     */
    T m_bandStop;
};

}

}

}

#endif
