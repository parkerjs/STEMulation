#ifndef IS_INPUT_STREAMABLE_H
#define IS_INPUT_STREAMABLE_H

#include <type_traits>

namespace traits
{

namespace tests
{

/**
 * Type traits struct to determine whether or not T is an input-streamable type
 */
template<typename Stream, typename T>
class is_input_streamable final
{
    /**
     * Helper functions
     */
    template<typename Left, typename Right>
    static auto test(int) -> decltype(std::declval<Left &>() >> std::declval<Right>(), std::true_type());
    template<typename, typename> static auto test(...) -> std::false_type;

public:

    /**
     * Static constants
     */
    static const bool value = decltype(test<Stream, T>(0))::value;
};

}

}

#endif
