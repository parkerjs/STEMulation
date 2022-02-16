#ifndef CUSTOM_LOCALE_H
#define CUSTOM_LOCALE_H

#include <cstring>
#include <locale>
#include <map>
#include <vector>

namespace utilities
{

/**
 * This class creates a custom locale for use with std::imbue
 */
class CustomLocale final
: public std::ctype<char>
{
public:

    /**
     * Constructor
     */
    CustomLocale(void)
    : std::ctype<char>(getTable())
    {
        memset((void *)getTable(), std::ctype_base::mask(), table_size * sizeof(std::ctype_base::mask));
    }

    /**
     * Constructor
     * @param characterMap a map of characters and corresponding character type classification masks
     */
    CustomLocale(const std::map<char, std::ctype_base::mask> &characterMap)
    : CustomLocale()
    {
        for (auto &&itCharacterMap : characterMap)
            m_table[int(itCharacterMap.first)] = itCharacterMap.second;
    }

    /**
     * Constructor
     * @param characters a vector of characters
     * @param mask       a single classification mask to which all elements in the character vector
     *                   will be mapped
     */
    CustomLocale(const std::vector<char> &characters, std::ctype_base::mask mask)
    : CustomLocale()
    {
        for (auto &&character : characters)
            m_table[int(character)] = mask;
    }

    /**
     * Destructor
     */
    virtual ~CustomLocale(void) override
    {

    }

    /**
     * Add a character mask
     */
    virtual void addMask(char character, std::ctype_base::mask mask) final
    {
        m_table[int(character)] = mask;
    }

    /**
     * create() function
     */
    inline static CustomLocale *create(void)
    {
        return new CustomLocale();
    }

    /**
     * create() function
     * @param characterMap a map of characters and corresponding character type classification masks
     */
    inline static CustomLocale *create(const std::map<char, std::ctype_base::mask> &characterMap)
    {
        return new CustomLocale(characterMap);
    }

    /**
     * create() function
     * @param characters a vector of characters
     * @param mask       a single classification mask to which all elements in the character vector
     *                   will be mapped
     */
    inline static CustomLocale *create(const std::vector<char> &characters, std::ctype_base::mask mask)
    {
        return new CustomLocale(characters, mask);
    }

    /**
     * Get the character table
     */
    inline virtual std::ctype_base::mask const *getTable(void) final
    {
        return &m_table[0];
    }

protected:

    /**
     * a map of bit masks
     */
    std::ctype_base::mask m_table[table_size];
};

}

#endif
