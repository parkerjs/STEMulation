#ifndef ALIGNED_H
#define ALIGNED_H

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#define POSIX
#endif

#ifdef _WIN32
#include <malloc.h>
#elif defined POSIX
#include <cstdlib>
#endif

namespace memory
{

/**
 * Function to allocate memory of the indicated size and alignment
 */
inline void *aligned_alloc(size_t alignment, size_t size)
{
    void *ptr = nullptr;
#ifdef _WIN32
    ptr = _aligned_malloc(size, alignment);
#elif defined POSIX
    posix_memalign(&ptr, alignment, size);
#else
    if (alignment < alignof(void *))
        alignment = alignof(void *);

    auto space = size + alignment - 1;
    void *pAllocatedMemory = ::operator new(space + sizeof(void *));
    ptr = static_cast<void *>(static_cast<char *>(pAllocatedMemory) + sizeof(void *));

    std::align(alignment, size, ptr, space);

    *(static_cast<void **>(ptr) - 1) = pAllocatedMemory;

#endif
    return ptr;
}

/**
 * Function to deallocate memory that was allocated using the aligned_alloc() function
 */
inline void aligned_free(void *ptr)
{
#ifdef _WIN32
    _aligned_free(ptr);
#elif defined POSIX
    free(ptr);
#else
    ::operator delete(*(static_cast<void **>(ptr) - 1));
#endif
}

}

#endif
