#ifndef SECOND_ORDER_SYSTEM_H
#define SECOND_ORDER_SYSTEM_H

#include "stateSpaceModel.h"

namespace math
{

namespace control_systems
{

namespace applied
{

/**
 * This class models a second order control system
 */
class SecondOrderSystem
: virtual private attributes::abstract::Reflective,
  public StateSpaceModel,
  public attributes::interfaces::Swappable<SecondOrderSystem>
{
public:

    /**
     * Using declarations
     */
    using StateSpaceModel::swap;

    /**
     * Constructor
     * @param omega   the natural frequency in radians
     * @param zeta    the damping ratio
     * @param command the command input to this second order system
     */
    EXPORT_STEM SecondOrderSystem(double omega = 1.0,
                                  double zeta = 1.0,
                                  double command = 1.0);

    /**
     * Copy constructor
     */
    EXPORT_STEM SecondOrderSystem(const SecondOrderSystem &system);

    /**
     * Move constructor
     */
    EXPORT_STEM SecondOrderSystem(SecondOrderSystem &&system);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~SecondOrderSystem(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM SecondOrderSystem &operator = (const SecondOrderSystem &system);

    /**
     * Move assignment operator
     */
    EXPORT_STEM SecondOrderSystem &operator = (SecondOrderSystem &&system);

    /**
     * Function to evaluate the dynamics model which represents this control system
     * @param t  time at which the dynamics model is evaluated
     * @param x  the state vector
     * @param xd the state derivative vector
     */
    EXPORT_STEM virtual void dynamicsModel(double t,
                                           const StateVector &x,
                                           StateVector &xd) override;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the command input to this second order system
     */
    EXPORT_STEM virtual double getCommand(void) const final;

    /**
     * Get the damping ratio
     */
    EXPORT_STEM virtual double getDampingRatio(void) final;

    /**
     * Get the natural frequency (radians)
     */
    EXPORT_STEM virtual double getNaturalFrequency(void) final;

    /**
     * Get the time-domain response of this system; upon success, returns true populates output vector
     */
    EXPORT_STEM virtual bool getResponse(StateVector &output) override;

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;

    /**
     * Return whether or not this system is critically-damped
     */
    EXPORT_STEM virtual bool isCriticallyDamped(void) const final;

    /**
     * Return whether or not this system is over-damped
     */
    EXPORT_STEM virtual bool isOverDamped(void) const final;

    /**
     * Return whether or not this system is under-damped
     */
    EXPORT_STEM virtual bool isUnderDamped(void) const final;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Set the command input to this second order system
     */
    EXPORT_STEM virtual void setCommand(double command) final;

    /**
     * Set the damping ratio
     */
    EXPORT_STEM virtual void setDampingRatio(double zeta) final;

    /**
     * Set the natural frequency (radians)
     */
    EXPORT_STEM virtual void setNaturalFrequency(double omega) final;

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(SecondOrderSystem &system) override final;

protected:

    /**
     * the command input to this second order system
     */
    double m_command;

    /**
     * the natural frequency (radians)
     */
    double m_omega;

    /**
     * the damping ratio
     */
    double m_zeta;
};

}

}

}

#endif
