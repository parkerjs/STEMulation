#ifndef FALLBACK_H
#define FALLBACK_H

#include <utility>

// forward declarations
namespace functional { class Any; }

namespace traits
{

namespace tests
{

namespace operators
{

namespace fallback_operators
{

/**
 * Helper objects
 */
struct tag { };

/**
 * Helper functions
 */
template<typename T> static std::true_type check(const T &) { return std::true_type(); }
inline static std::false_type check(tag) { return std::false_type(); }

/**
 * Arithmetic operators
 */
tag operator + (const functional::Any &, const functional::Any &);
tag operator -- (const functional::Any &);
tag operator -- (const functional::Any &, int);
tag operator / (const functional::Any &, const functional::Any &);
tag operator % (const functional::Any &, const functional::Any &);
tag operator * (const functional::Any &, const functional::Any &);
tag operator ++ (const functional::Any &);
tag operator ++ (const functional::Any &, int);
tag operator - (const functional::Any &, const functional::Any &);

/**
 * Bitwise operators
 */
tag operator &  (const functional::Any &, const functional::Any &);
tag operator << (const functional::Any &, const functional::Any &);
tag operator |  (const functional::Any &, const functional::Any &);
tag operator >> (const functional::Any &, const functional::Any &);
tag operator ^  (const functional::Any &, const functional::Any &);

/**
 * Comparison operators
 */
tag operator == (const functional::Any &, const functional::Any &);
tag operator >= (const functional::Any &, const functional::Any &);
tag operator >  (const functional::Any &, const functional::Any &);
tag operator <= (const functional::Any &, const functional::Any &);
tag operator <  (const functional::Any &, const functional::Any &);
tag operator != (const functional::Any &, const functional::Any &);

/**
 * Compound assignment operators
 */
tag operator +=  (const functional::Any &, const functional::Any &);
tag operator /=  (const functional::Any &, const functional::Any &);
tag operator %=  (const functional::Any &, const functional::Any &);
tag operator *=  (const functional::Any &, const functional::Any &);
tag operator -=  (const functional::Any &, const functional::Any &);
tag operator &=  (const functional::Any &, const functional::Any &);
tag operator <<= (const functional::Any &, const functional::Any &);
tag operator |=  (const functional::Any &, const functional::Any &);
tag operator >>= (const functional::Any &, const functional::Any &);
tag operator ^=  (const functional::Any &, const functional::Any &);

/**
 * Logical operators
 */
tag operator && (const functional::Any &, const functional::Any &);
tag operator || (const functional::Any &, const functional::Any &);

}

}

}

}

#endif
