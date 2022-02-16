#ifndef TOGGLEABLE_STREAM_H
#define TOGGLEABLE_STREAM_H

#include <iostream>
#include <sstream>

namespace utilities
{

/**
 * This class implements a wrapper for stream i/o classes and enables them to be easily toggled on/off
 */
class ToggleableStream final
{
public:

    /**
     * Constructor
     */
    ToggleableStream(std::ostream &stream, bool bEnabled = true)
    : m_bEnabled(bEnabled),
      m_pStream(&stream)
    {

    }

    /**
     * Copy constructor
     */
    ToggleableStream(const ToggleableStream &toggleableStream)
    : m_pStream(nullptr)
    {
        operator = (toggleableStream);
    }

    /**
     * Move constructor
     */
    ToggleableStream(ToggleableStream &&toggleableStream)
    : m_pStream(nullptr)
    {
        operator = (std::move(toggleableStream));
    }

    /**
     * Destructor
     */
    virtual ~ToggleableStream(void)
    {

    }

    /**
     * Copy assignment operator
     */
    ToggleableStream &operator = (const ToggleableStream &toggleableStream)
    {
        if (&toggleableStream != this)
        {
            m_bEnabled = toggleableStream.m_bEnabled;
            m_pStream  = toggleableStream.m_pStream;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    ToggleableStream &operator = (ToggleableStream &&toggleableStream)
    {
        if (&toggleableStream != this)
        {
            m_bEnabled = std::move(toggleableStream.m_bEnabled);
            m_pStream = std::move(toggleableStream.m_pStream);
            toggleableStream.m_pStream = nullptr;
        }

        return *this;
    }

    /**
     * Conversion to std::ostream
     */
    inline virtual operator std::ostream & (void) final
    {
        if (m_bEnabled)
            return *m_pStream;
        else
        {
            // if disabled, return a reference to a statically-defined local stream with the fail-bit set
            static std::ostringstream stream;
            stream.setstate(std::ios::failbit);

            return stream;
        }
    }

    /**
     * std::ostream operator
     */
    template<typename T> inline ToggleableStream &operator << (T &&object)
    {
        if (m_bEnabled && m_pStream != nullptr)
            *m_pStream << std::forward<T>(object);

        return *this;
    }

    /**
     * std::ostream operator for stream manipulators
     */
    inline virtual ToggleableStream &operator << (std::ostream &(*pManipulator)(std::ostream &)) final
    {
        if (m_bEnabled && m_pStream != nullptr)
            *m_pStream << pManipulator;

        return *this;
    }

    /**
     * std::ostream operator for stream manipulators
     */
    inline virtual ToggleableStream &
    operator << (std::basic_ios<char> &(*pManipulator)(std::basic_ios<char> &)) final
    {
        if (m_bEnabled && m_pStream != nullptr)
            *m_pStream << pManipulator;

        return *this;
    }

    /**
     * Get stream enabled flag
     */
    inline virtual bool &enabled(void) final
    {
        return m_bEnabled;
    }

    /**
     * Get stream enabled flag
     */
    inline virtual bool enabled(void) const final
    {
        return m_bEnabled;
    }

    /**
     * Set stream enabled flag
     */
    inline virtual void enabled(bool bEnabled) final
    {
        m_bEnabled = bEnabled;
    }

    /**
     * Set this object's stream
     */
    inline virtual void set(std::ostream &stream) final
    {
        m_pStream = &stream;
    }

private:

    /**
     * flag to indicate that the stream is active/inactive
     */
    bool m_bEnabled;

    /**
     * a pointer to an std::ostream object
     */
    std::ostream *m_pStream;
};

}

#endif
