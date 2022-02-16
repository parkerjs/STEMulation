#ifndef SIZE_H
#define SIZE_H

#include <cstring>
#include <cwchar>

namespace traits
{

namespace helpers
{

/**
 * Returns the size of the specified container
 */
template<typename Container>
inline constexpr auto size(const Container &container) -> decltype(container.size())
{
    return container.size();
}

/**
 * Returns the size of the specified array
 */
template<typename T, std::size_t N>
inline constexpr std::size_t size(const T (&array)[N]) noexcept
{
    return N;
}

/**
 * Returns the size of a character string
 */
inline auto size(const char *pString) noexcept
{
    return std::strlen(pString);
}

/**
 * Returns the size of a wide character string
 */
inline auto size(const wchar_t *pString) noexcept
{
    return std::wcslen(pString);
}

}

}

#endif
