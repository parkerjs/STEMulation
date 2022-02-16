#ifndef DATE_H
#define DATE_H

#include "cloneable.h"
#include "factory_constructible.h"
#include "output_streamable.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <string>

namespace utilities
{

namespace calendar
{

/**
 * This class serves as an abstract base for derived types that store/manipulate date information
 */
class Date
: public attributes::interfaces::Cloneable<Date>,
  public attributes::abstract::FactoryConstructible<Date>,
  public attributes::concrete::OutputStreamable<Date>,
  public attributes::concrete::StaticMutexMappable<Date, int, std::mutex>,
  public attributes::concrete::StaticSynchronizable<Date>
{
public:

    /**
     * Enumerations
     */
    enum class WeekDay { Sunday,
                         Monday,
                         Tuesday,
                         Wednesday,
                         Thursday,
                         Friday,
                         Saturday,
                         Unknown };

protected:

    /**
     * Constructor
     */
    Date(void)
    {
        m_day = Date::getCurrentDay();
        m_month = Date::getCurrentMonth();
        m_year = Date::getCurrentYear();
    }

    /**
     * Constructor
     */
    Date(int day,
         int month,
         int year)
    : m_day(day),
      m_month(month),
      m_year(year)
    {

    }

    /**
     * Copy constructor
     */
    Date(const Date &date)
    : m_day(date.m_day),
      m_month(date.m_month),
      m_year(date.m_year)
    {

    }

    /**
     * Move constructor
     */
    Date(Date &&date)
    : m_day(std::move(date.m_day)),
      m_month(std::move(date.m_month)),
      m_year(std::move(date.m_year))
    {

    }

public:

    /**
     * Destructor
     */
    virtual ~Date(void) override
    {

    }

    /**
     * Assignment operator
     */
    virtual Date &operator = (int date)
    {
        setDate(date);

        return *this;
    }

    /**
     * Copy assignment operator
     */
    virtual Date &operator = (const Date &date)
    {
        if (&date != this)
        {
            m_day = date.m_day;
            m_month = date.m_month;
            m_year = date.m_year;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    virtual Date &operator = (Date &&date)
    {
        if (&date != this)
        {
            m_day = std::move(date.m_day);
            m_month = std::move(date.m_month);
            m_year = std::move(date.m_year);
        }

        return *this;
    }

    /**
     * Conversion to std::tm
     */
    inline virtual operator std::tm (void) const final
    {
        return { 0,
                 0,
                 0,
                 m_day,
                 m_month,
                 m_year - 1900,
                 0,
                 0,
                 0 };
    }

    /**
     * Conversion to std::string
     */
    virtual operator std::string (void) const = 0;

    /**
     * Conversion to int
     */
    virtual operator int (void) const = 0;

    /**
     * Compare equal operator
     */
    inline virtual bool operator == (const Date &date) const
    {
        return m_day == date.m_day &&
               m_month == date.m_month &&
               m_year == date.m_year;
    }

    /**
     * Compare greater operator
     */
    virtual bool operator > (const Date &date) const final
    {
        std::unique_ptr<Date> pDateLHS(Date::create("yyyymmdd"));
        std::unique_ptr<Date> pDateRHS(Date::create("yyyymmdd"));
        if (pDateLHS != nullptr &&
            pDateRHS != nullptr)
        {
            pDateLHS->setDate(m_day,
                              m_month,
                              m_year);
            pDateRHS->setDate(date.m_day,
                              date.m_month,
                              date.m_year);

            return (int)*pDateLHS > (int)*pDateRHS;
        }

        return 0;
    }

    /**
     * This function computes a future or past date according to the number of specified days from current date
     */
    virtual int calcDateDaysFromCurrent(int numDays) const = 0;

    /**
     * This function computes a future or past date according to the number of specified days from current date
     * @param[in]  day, month, and year contain the date from which the future date will be computed
     * @param[out] day, month, and year contain the computed future date
     */
    static void calcDateDaysFromCurrent(int numDays,
                                        int &day,
                                        int &month,
                                        int &year)
    {
        struct tm time = { 0,
                           0,
                           0,
                           day,
                           month - 1,
                           year - 1900,
                           0,
                           0,
                           0 };

        time.tm_mday += numDays;
        std::time_t futureTime = std::mktime(&time);
        lock(0);
        time = *localtime(&futureTime);
        unlock(0);

        day = time.tm_mday;
        month = time.tm_mon + 1;
        year = time.tm_year + 1900;
    }

    /**
     * Calculate the date from the epoch time
     * @param epochTime the time in seconds since epoch (January 1st, 1970)
     * @param day       computed with respect to the supplied epoch time
     * @param month     computed with respect to the supplied epoch time
     * @param year      computed with respect to the supplied epoch time
     */
    static void calcDateFromEpochTime(long epochTime,
                                      int &day,
                                      int &month,
                                      int &year)
    {
        std::time_t time = epochTime;
        lock(0);
        struct tm *pTimeInfo = localtime(&time);

        day = pTimeInfo->tm_mday;
        month = 1 + pTimeInfo->tm_mon;
        year = 1900 + pTimeInfo->tm_year;
        unlock(0);
    }

    /**
     * This function computes a future or past date according to the number of specified months from current
     * date
     */
    virtual int calcDateMonthsFromCurrent(int numMonths) const = 0;

    /**
     * This function computes a future or past date according to the number of specified months from current
     * date
     * @param[in]  day, month, and year contain the date from which the future date will be computed
     * @param[out] day, month, and year contain the computed future date
     */
    static void calcDateMonthsFromCurrent(int numMonths,
                                          int &day,
                                          int &month,
                                          int &year)
    {
        int numYears = numMonths / 12;
        numMonths -= 12 * numYears;
        month += numMonths;
        year += numYears;

        struct tm time = { 0,
                           0,
                           0,
                           day,
                           month - 1,
                           year - 1900,
                           0,
                           0,
                           0 };
        std::time_t futureTime = std::mktime(&time);
        lock(0);
        time = *localtime(&futureTime);
        unlock(0);

        day = time.tm_mday;
        month = time.tm_mon + 1;
        year = time.tm_year + 1900;
    }

    /**
     * This function computes a future or past date according to the number of specified years from current
     * date
     */
    virtual int calcDateYearsFromCurrent(int numYears) const
    {
        return calcDateMonthsFromCurrent(12 * numYears);
    }

    /**
     * This function computes a future or past date according to the number of specified years from current
     * date
     * @param[in]  day, month, and year contain the date from which the future date will be computed
     * @param[out] day, month, and year contain the computed future date
     */
    inline static void calcDateYearsFromCurrent(int numYears,
                                                int &day,
                                                int &month,
                                                int &year)
    {
        return calcDateMonthsFromCurrent(12 * numYears,
                                         day,
                                         month,
                                         year);
    }

    /**
     * Calculate the number of days in the specified month for the given year
     */
    inline virtual int calcDaysInMonth(void) const final
    {
        return calcDaysInMonth(m_month,
                               m_year);
    }

    /**
     * Calculate the number of days in the specified month for the given year
     */
    static int calcDaysInMonth(int month, int year)
    {
        struct tm timeStartOfMonth = { 0,
                                       0,
                                       0,
                                       1,
                                       month - 1,
                                       year - 1900,
                                       0,
                                       0,
                                       0 };
        struct tm timeEndOfMonth = { 0,
                                     0,
                                     0,
                                     1,
                                     month % 12,
                                     year + (month / 12) - 1900,
                                     0,
                                     0,
                                     0 };

        auto endOfMonth = std::mktime(&timeEndOfMonth);
        auto startOfMonth = std::mktime(&timeStartOfMonth);

        return static_cast<int>(std::difftime(endOfMonth,
                                              startOfMonth) / 86400);
    }

    /**
     * Calculate the number of days that have elapsed between two dates
     */
    virtual int calcDifferenceInDays(const Date &date) const final
    {
        const Date *pEarlier = this, *pLater = &date;
        if (*this > date)
        {
            std::swap(pEarlier,
                      pLater);
        }

        return pLater->calcJulianDay() - pEarlier->calcJulianDay();
    }

    /**
     * Calculate the number of months that have elapsed between two dates
     */
    inline virtual int calcDifferenceInMonths(const Date &date) const final
    {
        int remainingDays = 0;

        return calcDifferenceInMonths(date,
                                      remainingDays);
    }

    /**
     * Calculate the number of months that have elapsed between two dates
     * @param date          a reference to a DATE object
     * @param[out] remainingDays the number of days beyond the last fully elapsed month
     */
    virtual int calcDifferenceInMonths(const Date &date,
                                       int &remainingDays) const final
    {
        const Date *pEarlier = this, *pLater = &date;
        if (*this > date)
        {
            std::swap(pEarlier,
                      pLater);
        }

        int numMonths = (pLater->m_year - pEarlier->m_year) * 12 +
                         pLater->m_month - pEarlier->m_month;
        remainingDays = pLater->m_day;
        if (numMonths > 0 &&
            pLater->m_day < pEarlier->m_day)
        {
            --numMonths;

            int month = pLater->m_month - 1, year = pLater->m_year;
            if (month < 1)
            {
                month = 12;
                year -= 1;
            }

            int daysInPreviousMonth = Date::calcDaysInMonth(month, year);
            int daysRemainingInPreviousMonth = daysInPreviousMonth - pEarlier->m_day;
            if (daysRemainingInPreviousMonth > 0)
            {
                remainingDays += daysRemainingInPreviousMonth;
            }
        }
        else if (pLater->m_day >= pEarlier->m_day)
        {
            remainingDays -= pEarlier->m_day;
        }

        return numMonths;
    }

    /**
     * Calculate the number of years that have elapsed between two dates
     */
    inline virtual int calcDifferenceInYears(const Date &date) const final
    {
        int remainingDays = 0, remainingMonths = 0;

        return calcDifferenceInYears(date,
                                     remainingMonths,
                                     remainingDays);
    }

    /**
     * Calculate the number of years that have elapsed between two dates
     * @param date            a reference to a DATE object
     * @param[out] remainingMonths the number of months beyond the last fully elapsed year
     * @param[out] remainingDays   the number of days beyond the last fully elapsed month
     */
    virtual int calcDifferenceInYears(const Date &date,
                                      int &remainingMonths,
                                      int &remainingDays) const final
    {
        remainingMonths = calcDifferenceInMonths(date,
                                                 remainingDays);
        int years = remainingMonths / 12;
        remainingMonths -= years * 12;

        return years;
    }

    /**
     * Calculate the Julian Day from a date
     */
    inline virtual int calcJulianDay(void) const final
    {
        return calcJulianDay(m_day,
                             m_month,
                             m_year);
    }

    /**
     * Calculate the Julian Day from a date
     */
    static int calcJulianDay(int day, int month, int year)
    {
        int a = (14 - month) / 12;
        int y = year + 4800 - a;
        int m = month + 12 * a - 3;

        int julian = day + (153 * m + 2) / 5 + 365 * y + y / 4;
        if ((year > 1582) ||
            (year == 1582 &&
             month > 10) ||
            (year == 1582 &&
             month == 10 &&
             day >= 15))
        {
            julian += y / 400 - (y / 100 + 32045);
        }
        else
        {
            julian -= 32083;
        }

        return julian;
    }

    /**
     * create() function
     */
    template<typename ... Args>
    inline static Date *create(std::string format, Args && ... args)
    {
        std::transform(format.begin(),
                       format.end(),
                       format.begin(),
                       ::tolower);

        return FactoryConstructible<Date>::create(format,
                                                  std::forward<Args>(args) ...);
    }

    /**
     * Get current date
     */
    inline static void getCurrentDate(int &day,
                                      int &month,
                                      int &year)
    {
        day = Date::getCurrentDay();
        month = Date::getCurrentMonth();
        year = Date::getCurrentYear();
    }

    /**
     * Get current date in the specified format. Returns 0 for invalid date formats.
     */
    static int getCurrentDate(const std::string &format)
    {
        int day = Date::getCurrentDay();
        int month = Date::getCurrentMonth();
        int year = Date::getCurrentYear();

        return getFormattedDate(format,
                                day,
                                month,
                                year);
    }

    /**
     * Get current date
     */
    virtual int getCurrentDate(void) const = 0;

    /**
     * Get current day
     */
    static int getCurrentDay(void)
    {
        std::time_t time = std::time(nullptr);
        lock(0);
        struct tm *pTime = localtime(&time);
        auto day = pTime->tm_mday;
        unlock(0);

        return day;
    }

    /**
     * Get current month
     */
    static int getCurrentMonth(void)
    {
        std::time_t time = std::time(nullptr);
        lock(0);
        struct tm *pTime = localtime(&time);
        auto month = 1 + pTime->tm_mon;
        unlock(0);

        return month;
    }

    /**
     * Get current year
     */
    static int getCurrentYear(void)
    {
        std::time_t time = std::time(nullptr);
        lock(0);
        struct tm *pTime = localtime(&time);
        auto year = 1900 + pTime->tm_year;
        unlock(0);

        return year;
    }

    /**
     * Get 8-digit, base-10 int representation of date
     */
    virtual int getDate(void) const = 0;

    /**
     * Get individually the day, month, and year
     */
    inline virtual void getDate(int &day,
                                int &month,
                                int &year) const final
    {
        day = getDay();
        month = getMonth();
        year = getYear();
    }

    /**
     * Get individually the day, month, and year from the input date having the specified format
     */
    static bool getDate(const std::string &format,
                        int date,
                        int &day,
                        int &month,
                        int &year)
    {
        std::unique_ptr<Date> pDate(Date::create(format));
        bool bSuccess = (pDate != nullptr);
        if (bSuccess)
        {
            pDate->setDate(date);
            day = pDate->getDay();
            month = pDate->getMonth();
            year = pDate->getYear();
        }

        return bSuccess;
    }

    /**
     * Get day
     */
    inline virtual int getDay(void) const final
    {
        return m_day;
    }

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return getFormat();
    }

    /**
     * Get this date object's format
     */
    virtual std::string getFormat(void) const = 0;

    /**
     * Get formatted date according to the specified date. Returns 0 for invalid date formats.
     */
    static int getFormattedDate(const std::string &format,
                                int day,
                                int month,
                                int year)
    {
        std::unique_ptr<Date> pDate(Date::create(format));
        if (pDate != nullptr)
        {
            pDate->setDate(day,
                           month,
                           year);
        }

        return pDate != nullptr ? *pDate : 0;
    }

    /**
     * Get month
     */
    inline virtual int getMonth(void) const final
    {
        return m_month;
    }

    /**
     * Get the next business day
     * @param[in]  day, month, and year contain the date from which the next business day will be computed
     * @param[out] day, month, and year contain the computed future date
     */
    virtual int getNextBusinessDay(void) const final
    {
        int numDaysInFuture = 0;
        int day = m_day, month = m_month, year = m_year;
        while (true)
        {
            calcDateDaysFromCurrent(++numDaysInFuture,
                                    day,
                                    month,
                                    year);

            if (isBusinessDay(day,
                              month,
                              year))
            {
                break;
            }
        }

        return getFormattedDate(getFormat(),
                                day,
                                month,
                                year);
    }

    /**
     * Get the next business day; Upon success, returns a non-zero date
     */
    static int getNextBusinessDay(const std::string &format,
                                  int date)
    {
        int day, month, year;
        if (getDate(format,
                    date,
                    day,
                    month,
                    year))
        {
            getNextBusinessDay(day,
                               month,
                               year);

            return getFormattedDate(format,
                                    day,
                                    month,
                                    year);
        }

        return 0;
    }

    /**
     * Get the next business day
     * @param[in]  day, month, and year contain the date from which the next business day will be computed
     * @param[out] day, month, and year contain the computed future date
     */
    static void getNextBusinessDay(int &day,
                                   int &month,
                                   int &year)
    {
        int numDaysInFuture = 0;
        while (true)
        {
            calcDateDaysFromCurrent(++numDaysInFuture,
                                    day,
                                    month,
                                    year);
            if (isBusinessDay(day,
                              month,
                              year))
            {
                break;
            }
        }
    }

    /**
     * Get the previous business day
     * @param[in]  day, month, and year contain the date from which the previous business day will be computed
     * @param[out] day, month, and year contain the computed previous date
     */
    virtual int getPreviousBusinessDay(void) const final
    {
        int numDaysInFuture = 0;
        int day = m_day, month = m_month, year = m_year;
        while (true)
        {
            calcDateDaysFromCurrent(--numDaysInFuture,
                                    day,
                                    month,
                                    year);
            if (isBusinessDay(day,
                              month,
                              year))
            {
                break;
            }
        }

        return getFormattedDate(getFormat(),
                                day,
                                month,
                                year);
    }

    /**
     * Get the previous business day; Upon success, returns a non-zero date
     */
    static int getPreviousBusinessDay(const std::string &format,
                                      int date)
    {
        int day, month, year;
        if (getDate(format,
                    date,
                    day,
                    month,
                    year))
        {
            getPreviousBusinessDay(day,
                                   month,
                                   year);

            return getFormattedDate(format,
                                    day,
                                    month,
                                    year);
        }

        return 0;
    }

    /**
     * Get the previous business day
     * @param[in]  day, month, and year contain the date from which the previous business day will be computed
     * @param[out] day, month, and year contain the computed previous date
     */
    static void getPreviousBusinessDay(int &day,
                                       int &month,
                                       int &year)
    {
        int numDaysInFuture = 0;
        while (true)
        {
            calcDateDaysFromCurrent(--numDaysInFuture,
                                    day,
                                    month,
                                    year);
            if (isBusinessDay(day,
                              month,
                              year))
            {
                break;
            }
        }
    }

    /**
     * Get day of the week
     */
    inline virtual WeekDay getWeekDay(void) const final
    {
        return getWeekDay(m_day,
                          m_month,
                          m_year);
    }

    /**
     * Get day of the week
     */
    static WeekDay getWeekDay(int day,
                              int month,
                              int year)
    {
        struct tm time = { 0,
                           0,
                           12,
                           day,
                           month - 1,
                           year - 1900,
                           0,
                           0,
                           0 };
        std::mktime(&time);

        switch (time.tm_wday)
        {
            case 0: return WeekDay::Sunday;
            case 1: return WeekDay::Monday;
            case 2: return WeekDay::Tuesday;
            case 3: return WeekDay::Wednesday;
            case 4: return WeekDay::Thursday;
            case 5: return WeekDay::Friday;
            case 6: return WeekDay::Saturday;
        }

        return WeekDay::Unknown;
    }

    /**
     * Get year
     */
    inline virtual int getYear(void) const final
    {
        return m_year;
    }

    /**
     * Test for business day
     */
    inline virtual bool isBusinessDay(void) const final
    {
        return isBusinessDay(m_day,
                             m_month,
                             m_year);
    }

    /**
     * Test for business day
     */
    static bool isBusinessDay(int day,
                              int month,
                              int year)
    {
        WeekDay weekDay = getWeekDay(day,
                                     month,
                                     year);
        switch (weekDay)
        {
            case WeekDay::Monday:
            case WeekDay::Tuesday:
            case WeekDay::Wednesday:
            case WeekDay::Thursday:
            case WeekDay::Friday:
            return true;

            default:
            return false;
        }
    }

    /**
     * Test for whether or not this object contains today's date
     */
    inline virtual bool isTodaysDate(void) const final
    {
        int day, month, year;
        getCurrentDate(day,
                       month,
                       year);

        return day == m_day &&
               month == m_month &&
               year == m_year;
    }

    /**
     * Test for weekend
     */
    inline virtual bool isWeekend(void) const final
    {
        return isWeekend(m_day,
                         m_month,
                         m_year);
    }

    /**
     * Test for weekend
     */
    static bool isWeekend(int day,
                          int month,
                          int year)
    {
        WeekDay weekDay = getWeekDay(day,
                                     month,
                                     year);
        switch (weekDay)
        {
            case WeekDay::Saturday:
            case WeekDay::Sunday:
            return true;

            default:
            return false;
        }
    }

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    inline virtual std::ostream &print(std::ostream &stream) const override
    {
        stream << operator std::string ();

        return stream;
    }

    /**
     * Set 8-digit, base-10 int representation of date
     */
    virtual void setDate(int date) = 0;

    /**
     * Set individually the day, month, and year
     */
    void virtual setDate(int day,
                         int month,
                         int year) final
    {
        setDay(day);
        setMonth(month);
        setYear(year);
    }

    /**
     * Set day
     */
    inline virtual void setDay(int day) final
    {
        m_day = day;
    }

    /**
     * Set month
     */
    inline virtual void setMonth(int month) final
    {
        m_month = month;
    }

    /**
     * Set year
     */
    inline virtual void setYear(int year) final
    {
        m_year = year;
    }

    /**
     * Convert date to seconds since epoch (January 1st, 1970)
     */
    virtual long timeSinceEpoch(int hours = 0,
                                int minutes = 0,
                                int seconds = 0) final
    {
        struct tm startOfYear = { 0,
                                  0,
                                  0,
                                  1,
                                  0,
                                  m_year - 1900,
                                  0,
                                  0,
                                  0 };
        struct tm time = { seconds,
                           minutes,
                           hours,
                           m_day,
                           m_month - 1,
                           m_year - 1900,
                           0,
                           0,
                           0 };

        time.tm_yday = static_cast<int>(std::difftime(std::mktime(&time),
                                                      std::mktime(&startOfYear)) / 86400);

        return time.tm_sec + time.tm_min * 60 + time.tm_hour * 3600 + time.tm_yday * 86400 +
              (time.tm_year - 70) * 31536000 + ((time.tm_year - 69) / 4) * 86400 -
             ((time.tm_year - 1) / 100) * 86400 + ((time.tm_year + 299) / 400) * 86400;
    }

    /**
     * Update this object's date to the next business day
     */
    inline virtual void updateToNextBusinessDay(void) final
    {
        setDate(getNextBusinessDay());
    }

    /**
     * Update this object's date to the previous business day
     */
    inline virtual void updateToPreviousBusinessDay(void) final
    {
        setDate(getPreviousBusinessDay());
    }

protected:

    /**
     * the day
     */
    int m_day;

    /**
     * the month
     */
    int m_month;

    /**
     * the year
     */
    int m_year;
};

}

}

#endif
