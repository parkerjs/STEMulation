#ifndef DATE_YYYYMMDD_H
#define DATE_YYYYMMDD_H

#include "date.h"
#include <iomanip>

namespace utilities
{

namespace calendar
{

/**
 * This class stores date information in the YYYYMMDD format
 */
class DateYYYYMMDD
: public Date
{
public:

    /**
     * Forwarding constructor
     */
    template<typename ... Args> DateYYYYMMDD(Args && ... args)
    : Date(std::forward<Args>(args) ...)
    {

    }

    /**
     * Constructor a date object
     */
    template<typename T,
             typename std::enable_if<std::is_integral<typename std::decay<T>::type>::value, int>::type = 0>
    DateYYYYMMDD(T &&date)
    {
        DateYYYYMMDD::setDate(date);
    }

    /**
     * Destructor
     */
    virtual ~DateYYYYMMDD(void) override
    {

    }

    /**
     * Forwarding assignment operator
     */
    template<typename T> Date &operator = (T &&rhs)
    {
        Date::operator = (std::forward<T>(rhs));

        return *this;
    }

    /**
     * Conversion to std::string
     */
    inline virtual operator std::string(void) const override
    {
        return std::to_string(m_year) + std::to_string(m_month) + std::to_string(m_day);
    }

    /**
     * Conversion to int
     */
    inline virtual operator int (void) const override
    {
        return m_year * 10000 + m_month * 100 + m_day;
    }

    /**
     * This function computes a future or past date according to the number of specified days from current date
     */
    inline virtual int calcDateDaysFromCurrent(int numDays) const override
    {
        int day = m_day, month = m_month, year = m_year;
        Date::calcDateDaysFromCurrent(numDays, day, month, year);

        return DateYYYYMMDD(day, month, year);
    }

    /**
     * Calculate the date from the epoch time
     * @param epochTime the time in seconds since epoch (January 1st, 1970)
     */
    inline static int calcDateFromEpochTime(long epochTime)
    {
        int day, month, year;
        Date::calcDateFromEpochTime(epochTime, day, month, year);

        return DateYYYYMMDD(day, month, year);
    }

    /**
     * This function computes a future or past date according to the number of specified days from current date
     */
    inline virtual int calcDateMonthsFromCurrent(int numMonths) const override
    {
        int day = m_day, month = m_month, year = m_year;
        Date::calcDateMonthsFromCurrent(numMonths, day, month, year);

        return DateYYYYMMDD(day, month, year);
    }

    /**
     * clone() function
     */
    inline virtual DateYYYYMMDD *clone(void) const override
    {
        return new DateYYYYMMDD(*this);
    }

    /**
     * create() function
     */
    inline static Date *create(void)
    {
        return new DateYYYYMMDD();
    }

    /**
     * Get current date in YYYYMMDD format
     */
    inline virtual int getCurrentDate(void) const override
    {
        return Date::getCurrentDate(getFormat());
    }

    /**
     * Get date in YYYYMMDD format
     */
    inline virtual int getDate(void) const override
    {
        return *this;
    }

    /**
     * Get individually the day, month, and year
     */
    inline static void getDate(int date, int &day, int &month, int &year)
    {
        day = date % 100;
        month = (date / 100) % 100;
        year = date / 10000;
    }

    /**
     * Get this date object's format
     */
    inline virtual std::string getFormat(void) const override
    {
        return "yyyymmdd";
    }

    /**
     * Get formatted date according to the specified date
     */
    inline static int getFormattedDate(int day, int month, int year)
    {
        return Date::getFormattedDate("yyyymmdd", day, month, year);
    }

    /**
     * Get the next business day; Upon success, returns a non-zero date
     */
    inline static int getNextBusinessDay(int date)
    {
        return Date::getNextBusinessDay("yyyymmdd", date);
    }

    /**
     * Get the previous business day; Upon success, returns a non-zero date
     */
    inline static int getPreviousBusinessDay(int date)
    {
        return Date::getPreviousBusinessDay("yyyymmdd", date);
    }

    /**
     * Set date in YYYYMMDD format
     */
    inline virtual void setDate(int date) override
    {
        getDate(date, m_day, m_month, m_year);
    }
};

/**
 * Factory registrations
 */
static attributes::abstract::FactoryRegistrar<Date> yyyymmdd_factory("yyyymmdd", &DateYYYYMMDD::create);

}

}

#endif
