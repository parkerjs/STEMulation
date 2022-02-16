#ifndef SIGNAL_SLOT_H
#define SIGNAL_SLOT_H

#include <functional>
#include <map>
#include <utility>

namespace messaging
{

/**
 * This class implements the signal/slot messaging pattern where objects can send signals containing event
 * information which can then be received by other controls using special functions known as slots. A signal
 * object may call multiple slots with the same signature. Functions can be connected to the signal, which will
 * be called when the emit() method on the signal object is invoked. Arguments passed to emit() will be passed
 * to the given functions.
 */
template<typename Result = void, typename ... Args>
class Signal final
{
public:

    /**
     * Constructor
     */
    Signal(void)
    : m_currentSlotId(0)
    {

    }

private:

    /**
     * Copy constructor
     */
    Signal(const Signal<Result, Args ...> &signal) = delete;

    /**
     * Move constructor
     */
    Signal(Signal<Result, Args ...> &&signal) = delete;

public:

    /**
     * Destructor
     */
    virtual ~Signal(void)
    {

    }

private:

    /**
     * Copy assignment operator
     */
    Signal<Result, Args ...> &operator = (const Signal<Result, Args ...> &signal) = delete;

    /**
     * Move assignment operator
     */
    Signal<Result, Args ...> &operator = (Signal<Result, Args ...> &&signal) = delete;

public:

    /**
     * Connect a member function of a given object to this signal
     */
    template<typename Function> inline int connect(Function &&function, Args && ... args) const
    {
        m_slots.insert({ ++m_currentSlotId, std::bind(function, std::forward<Args>(args) ...) });

        return m_currentSlotId;
    }

    /**
     * Connect an std::function to the signal. The returned value can be used to disconnect the function again
     */
    inline virtual int connect(const std::function<Result (Args && ...)> &slot) const final
    {
        m_slots.insert(std::make_pair(++m_currentSlotId, slot));

        return m_currentSlotId;
    }

    /**
     * Disconnect a previously connected function
     */
    inline virtual void disconnect(int slotId) const final
    {
        m_slots.erase(m_currentSlotId);
    }

    /**
     * Disconnect all previously connected functions
     */
    inline virtual void disconnect(void) const final
    {
        m_slots.clear();
    }

    /**
     * Invoke all connected functions
     */
    template<typename T = Result> typename std::enable_if<std::is_same<T, void>::value, void>::type
    emit(Args && ... args)
    {
        for (auto &&itSlot : m_slots)
            itSlot.second(std::forward<Args>(args) ...);
    }

    /**
     * Invoke all connected functions (non-void returns types)
     */
    template<typename T = Result>
    typename std::enable_if<!std::is_same<T, void>::value, std::map<int, Result>>::type
    emit(Args && ... args)
    {
        std::map<int, Result> results;
        for (auto &&itSlot : m_slots)
            results[itSlot.first] = itSlot.second(std::forward<Args>(args) ...);
    }

private:

    /**
     * current slot id
     */
    mutable int m_currentSlotId;

    /**
     * map of id-slot pairs
     */
    mutable std::map<int, std::function<Result (Args && ...)>> m_slots;
};

}

#endif
