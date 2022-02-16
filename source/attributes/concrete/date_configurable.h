#ifndef DATE_CONFIGURABLE_H
#define DATE_CONFIGURABLE_H

#include "date.h"
#include "date_formattable.h"

namespace attributes
{

namespace concrete
{

/**
 * This class provides a concrete interface for types that are configurable by date
 */
class DateConfigurable
: public attributes::interfaces::DateFormattable
{
protected:

    /**
     * Constructor
     * @param dateFormat the date format (YYYYMMDD, MMDDYYYY, etc...)
     */
    DateConfigurable(const std::string &dateFormat = "")
    : m_dateFormat(dateFormat),
      m_pEndDate(nullptr),
      m_pStartDate(nullptr)
    {

    }

    /**
     * Copy constructor
     */
    DateConfigurable(const DateConfigurable &configurable)
    : m_pEndDate(nullptr),
      m_pStartDate(nullptr)
    {
        operator = (configurable);
    }

    /**
     * Move constructor
     */
    DateConfigurable(DateConfigurable &&configurable)
    : m_pEndDate(nullptr),
      m_pStartDate(nullptr)
    {
        operator = (std::move(configurable));
    }

public:

    /**
     * Destructor
     */
    virtual ~DateConfigurable(void) override
    {
        if (m_pEndDate != nullptr)
        {
            delete m_pEndDate;
        }

        if (m_pStartDate != nullptr)
        {
            delete m_pStartDate;
        }
    }

protected:

    /**
     * Copy assignment operator
     */
    DateConfigurable &
    operator = (const DateConfigurable &configurable)
    {
        if (&configurable != this)
        {
            m_dateFormat = configurable.m_dateFormat;

            if (m_pEndDate != nullptr)
            {
                delete m_pEndDate;
                m_pEndDate = nullptr;
            }

            if (configurable.m_pEndDate != nullptr)
            {
                m_pEndDate = configurable.m_pEndDate->clone();
            }

            if (m_pStartDate != nullptr)
            {
                delete m_pStartDate;
                m_pStartDate = nullptr;
            }

            if (configurable.m_pStartDate != nullptr)
            {
                m_pStartDate = configurable.m_pStartDate->clone();
            }
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    DateConfigurable &
    operator = (DateConfigurable &&configurable)
    {
        if (&configurable != this)
        {
            m_dateFormat = std::move(configurable.m_dateFormat);

            if (m_pEndDate != nullptr)
            {
                delete m_pEndDate;
                m_pEndDate = nullptr;
            }

            m_pEndDate = std::move(configurable.m_pEndDate);
            configurable.m_pEndDate = nullptr;

            if (m_pStartDate != nullptr)
            {
                delete m_pStartDate;
                m_pStartDate = nullptr;
            }

            m_pStartDate = std::move(configurable.m_pStartDate);
            configurable.m_pStartDate = nullptr;
        }

        return *this;
    }

public:

    /**
     * Get this object's date format
     */
    inline virtual std::string
    getDateFormat(void) const override final
    {
        return m_dateFormat;
    }

    /**
     * Get ending date
     */
    inline virtual int
    getEndDate(void) const final
    {
        return m_pEndDate != nullptr
               ? m_pEndDate->getDate()
               : 0;
    }

    /**
     * Get ending day
     */
    inline virtual int
    getEndDay(void) const final
    {
        return m_pEndDate != nullptr
               ? m_pEndDate->getDay()
               : 0;
    }

    /**
     * Get ending month
     */
    inline virtual int
    getEndMonth(void) const final
    {
        return m_pEndDate != nullptr
               ? m_pEndDate->getMonth()
               : 0;
    }

    /**
     * Get ending year
     */
    inline virtual int
    getEndYear(void) const final
    {
        return m_pEndDate != nullptr
               ? m_pEndDate->getYear()
               : 0;
    }

    /**
     * Get starting date
     */
    inline virtual int
    getStartDate(void) const final
    {
        return m_pStartDate != nullptr
               ? m_pStartDate->getDate()
               : 0;
    }

    /**
     * Get starting day
     */
    inline virtual int
    getStartDay(void) const final
    {
        return m_pStartDate != nullptr
               ? m_pStartDate->getDay()
               : 0;
    }

    /**
     * Get starting month
     */
    inline virtual int
    getStartMonth(void) const final
    {
        return m_pStartDate != nullptr
               ? m_pStartDate->getMonth()
               : 0;
    }

    /**
     * Get starting year
     */
    inline virtual int
    getStartYear(void) const final
    {
        return m_pStartDate != nullptr
               ? m_pStartDate->getYear()
               : 0;
    }

    /**
     * Initialization function
     */
    virtual bool
    initialize(int startDate = 0,
               int endDate = 0) final
    {
        if (m_pEndDate != nullptr)
        {
            delete m_pEndDate;
            m_pEndDate = nullptr;
        }

        if (m_pStartDate != nullptr)
        {
            delete m_pStartDate;
            m_pStartDate = nullptr;
        }

        m_pEndDate = utilities::calendar::Date::create(m_dateFormat);

        if (m_pEndDate != nullptr &&
            endDate != 0)
        {
            m_pEndDate->setDate(endDate);
        }

        m_pStartDate = utilities::calendar::Date::create(m_dateFormat);

        if (m_pStartDate != nullptr &&
            startDate != 0)
        {
            m_pStartDate->setDate(startDate);
        }

        return m_pEndDate != nullptr &&
               m_pStartDate != nullptr;
    }

    /**
     * Set this object's date format
     */
    virtual bool
    setDateFormat(const std::string &dateFormat) override final
    {
        bool bSuccess = true;
        if (dateFormat != m_dateFormat)
        {
            int endDay = 0,
                endMonth = 0,
                endYear = 0;
            int startDay = 0,
                startMonth = 0,
                startYear = 0;

            m_dateFormat = dateFormat;
            if (m_pEndDate != nullptr)
            {
                m_pEndDate->getDate(endDay,
                                    endMonth,
                                    endYear);
            }

            if (m_pStartDate != nullptr)
            {
                m_pStartDate->getDate(startDay,
                                      startMonth,
                                      startYear);
            }

            bSuccess = initialize();
            if (bSuccess)
            {
                m_pEndDate->setDate(endDay,
                                    endMonth,
                                    endYear);
                m_pStartDate->setDate(startDay,
                                      startMonth,
                                      startYear);
            }
        }

        return bSuccess;
    }

    /**
     * Set ending date
     */
    inline virtual void
    setEndDate(int date) final
    {
        if (m_pEndDate != nullptr)
        {
            m_pEndDate->setDate(date);
        }
    }

    /**
     * Set ending date
     */
    inline virtual void
    setEndDate(int day,
               int month,
               int year) final
    {
        if (m_pEndDate != nullptr)
        {
            m_pEndDate->setDate(day,
                                month,
                                year);
        }
    }

    /**
     * Set ending day
     */
    inline virtual void
    setEndDay(int endDay) final
    {
        if (m_pEndDate != nullptr)
        {
            m_pEndDate->setDay(endDay);
        }
    }

    /**
     * Set ending month
     */
    inline virtual void
    setEndMonth(int endMonth) final
    {
        if (m_pEndDate != nullptr)
        {
            m_pEndDate->setMonth(endMonth);
        }
    }

    /**
     * Set ending year
     */
    inline virtual void
    setEndYear(int endYear) final
    {
        if (m_pEndDate != nullptr)
        {
            m_pEndDate->setYear(endYear);
        }
    }

    /**
     * Set starting date
     */
    inline virtual void
    setStartDate(int date) final
    {
        if (m_pStartDate != nullptr)
        {
            m_pStartDate->setDate(date);
        }
    }

    /**
     * Set starting date
     */
    inline virtual void
    setStartDate(int day,
                 int month,
                 int year) final
    {
        if (m_pStartDate != nullptr)
        {
            m_pStartDate->setDate(day,
                                  month,
                                  year);
        }
    }

    /**
     * Set starting day
     */
    inline virtual void
    setStartDay(int startDay) final
    {
        if (m_pStartDate != nullptr)
        {
            m_pStartDate->setDay(startDay);
        }
    }

    /**
     * Set starting month
     */
    inline virtual void
    setStartMonth(int startMonth) final
    {
        if (m_pStartDate != nullptr)
        {
            m_pStartDate->setMonth(startMonth);
        }
    }

    /**
     * Set starting year
     */
    inline virtual void
    setStartYear(int startYear) final
    {
        if (m_pStartDate != nullptr)
        {
            m_pStartDate->setYear(startYear);
        }
    }

protected:

    /**
     * this object's date format
     */
    std::string m_dateFormat;

    /**
     * ending date
     */
    utilities::calendar::Date *m_pEndDate;

    /**
     * starting date
     */
    utilities::calendar::Date *m_pStartDate;
};

}

}

#endif
