#ifndef VIRTUAL_ENABLE_SHARED_FROM_THIS_H
#define VIRTUAL_ENABLE_SHARED_FROM_THIS_H

#include <memory>

namespace memory
{

/**
 * A derived version of std::enable_shared_from_this that works with multiple inheritance
 */
struct virtual_enable_shared_from_this_base
: private std::enable_shared_from_this<virtual_enable_shared_from_this_base>
{
    /**
     * Destructor
     */
    virtual ~virtual_enable_shared_from_this_base(void) override
    {

    }
};

/**
 * A derived version of std::enable_shared_from_this that works with multiple inheritance
 */
template<typename T>
struct virtual_enable_shared_from_this
: virtual private virtual_enable_shared_from_this_base
{
    /**
     * shared_from_this() override
     */
    inline std::shared_ptr<T> shared_from_this(void)
    {
        return std::dynamic_pointer_cast<T>(virtual_enable_shared_from_this_base::shared_from_this());
    }
};

}

#endif
