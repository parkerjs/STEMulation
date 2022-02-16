#ifndef ANY_H
#define ANY_H

#include "holder.h"
#include "is_output_streamable.h"
#include "reference_unwrapper.h"
#include "swappable.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <tuple>

namespace functional
{

// forward declarations
class Any;
template<typename T> T any_cast(Any &operand);

/**
 * This class can hold instances of any copy-constructible type. Code adapted and modified from Boost C++
 * libraries.
 */
class Any final
: public attributes::interfaces::Cloneable<Any>,
  public attributes::interfaces::Swappable<Any>,
  public std::tuple<functional::Holder *>
{
public:

    /**
     * Friend declarations
     */
    template<typename T> friend T *any_cast(Any *) noexcept;
    template<typename T> friend T *unsafe_any_cast(Any *) noexcept;

    /**
     * Constructor
     */
    Any(void) noexcept
    : std::tuple<functional::Holder *>(nullptr)
    {

    }

    /**
     * Constructor
     */
    template<typename T, typename U = typename std::decay<T>::type,
             typename std::enable_if<!std::is_same<std::tuple<functional::Holder *>, U>::value &&
                                     !std::is_base_of<std::tuple<functional::Holder *>, U>::value, int>::type = 0>
    Any(const T &value) noexcept
    : std::tuple<functional::Holder *>(new Held<T>(value))
    {

    }

    /**
     * Copy constructor
     */
    Any(const Any &any) noexcept
    : std::tuple<functional::Holder *>(any.holder() != nullptr ? any.holder()->clone() : nullptr)

    {

    }

    /**
     * Move constructor
     */
    Any(Any &&any) noexcept
    : std::tuple<functional::Holder *>(std::get<functional::Holder *>(any))
    {
        std::get<functional::Holder *>(any) = nullptr;
    }

    /**
     * Destructor
     */
    virtual ~Any(void) noexcept
    {
        auto *pHolder = holder();
        if (pHolder != nullptr)
            delete pHolder;
    }

    /**
     * Copy assignment operator
     */
    Any &operator = (const Any &any)
    {
        if (&any != this)
        {
            Any(any).swap(*this);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Any &operator = (Any &&any)
    {
        any.swap(*this);

        return *this;
    }

    /**
     * Assignment operator
     */
    template<typename T, typename U = typename std::decay<T>::type>
    inline typename std::enable_if<!std::is_same<std::tuple<functional::Holder *>, U>::value &&
                                   !std::is_base_of<std::tuple<functional::Holder *>, U>::value, Any &>::type
    operator = (const T &value)
    {
        Any(value).swap(*this);

        return *this;
    }

    /**
     * Implicit conversion operator
     */
    template<typename T>
    operator T (void) const
    {
        return any_cast<T>(*this);
    }

    /**
     * Implicit conversion operator
     */
    template<typename T>
    operator T & (void)
    {
        return any_cast<T &>(*this);
    }

    /**
     * Test for whether or not this entry contains valid content
     */
    inline explicit operator bool (void) const
    {
        return !empty();
    }

    /**
     * clone() function
     */
    inline virtual Any *clone(void) const override
    {
        return new Any(*this);
    }

    /**
     * Test for empty object
     */
    inline bool empty(void) const noexcept
    {
        return holder() == nullptr;
    }

    /**
     * Return a pointer to this object's holder
     */
    inline virtual functional::Holder *holder(void) final
    {
        return std::get<functional::Holder *>(*this);
    }

    /**
     * Return a pointer to this object's holder
     */
    inline virtual functional::Holder *holder(void) const final
    {
        return std::get<functional::Holder *>(*this);
    }

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    inline std::ostream &print(std::ostream &stream) const
    {
        auto *pHolder = static_cast<Holder *>(holder());
        if (pHolder != nullptr)
            pHolder->print(stream);
        else
            stream.setstate(std::istream::failbit);

        return stream;
    }

    /**
     * Swap contents of this object with another
     */
    inline virtual void swap(Any &any) noexcept override final
    {
        std::tuple<functional::Holder *>::swap(any);
    }

    /**
     * Swap contents of two Any objects
     */
    inline friend void swap(Any &lhs,
                            Any &rhs) noexcept
    {
        lhs.swap(rhs);
    }

    /**
     * Function to convert this object to a string
     */
    inline std::string toString(void) const
    {
        std::ostringstream stream;
        print(stream);

        return stream.str();
    }

    /**
     * Return this object's type_info
     */
    inline virtual const std::type_info &type(void) const final
    {
        return holder() ? holder()->type() : typeid(nullptr);
    }

private:

    /**
     * This class serves as an abstract base container class that facilitates the type erasure idiom in classes
     * from which it is derived
     */
    class Holder
    : public attributes::interfaces::Cloneable<Holder>,
      public functional::Holder
    {
    public:

        /**
         * Destructor
         */
        virtual ~Holder(void) override
        {

        }

        /**
         * Output stream print function
         * @param stream a reference to an std::ostream object
         */
        EXPORT_STEM virtual std::ostream &print(std::ostream &stream) const = 0;

        /**
         * print() overload for stream-able types
         */
        template<typename Content>
        inline static typename
        std::enable_if<traits::tests::is_output_streamable<std::ostream, Content>::value, std::ostream &>::type
        print(std::ostream &stream,
              Content &&content)
        {
            return stream << std::boolalpha << content;
        }

        /**
         * print() overload for non-stream-able types
         */
        template<typename Content>
        inline static typename
        std::enable_if<!traits::tests::is_output_streamable<std::ostream, Content>::value, std::ostream &>::type
        print(std::ostream &stream,
              Content &&)
        {
            return stream;
        }
    };

    /**
     * This class implements the abstract base Holder class
     */
    template<typename T>
    class Held
    : public functional::Held<T>,
      public Holder
    {
    public:

        /**
         * Constructor
         */
        Held(const T &value)
        : functional::Held<T>(value)
        {

        }

        /**
         * Destructor
         */
        virtual ~Held(void) override
        {

        }

        /**
         * clone() function
         */
        inline virtual Holder *clone(void) const override
        {
            return new Held<T>(this->m_held);
        }

        /**
         * Output stream print function
         * @param stream a reference to an std::ostream object
         */
        inline virtual std::ostream &print(std::ostream &stream) const override
        {
            return Holder::print(stream, this->m_held);
        }

        /**
         * Return this object's type_info
         */
        inline virtual const std::type_info &type(void) const override
        {
            return typeid(T);
        }
    };

    /**
     * This class implements the abstract base Holder class
     */
    template<typename T>
    class Held<std::reference_wrapper<T>> final
    : public functional::Held<std::reference_wrapper<typename std::remove_cv<T>::type>>,
      public Holder
    {
    public:

        /**
         * Typedef declarations
         */
        typedef typename std::remove_cv<T>::type noconst;

        /**
         * Constructor
         */
        Held(const std::reference_wrapper<T> &value)
        : functional::Held<std::reference_wrapper<noconst>>(const_cast<noconst &>(value.get()))
        {

        }

        /**
         * Destructor
         */
        virtual ~Held(void) override
        {

        }

        /**
         * clone() function
         */
        inline virtual Holder *clone(void) const override
        {
            return new Held<std::reference_wrapper<noconst>>(this->m_held);
        }

        /**
         * Output stream print function
         * @param stream a reference to an std::ostream object
         */
        inline virtual std::ostream &print(std::ostream &stream) const override
        {
            return Holder::print(stream, const_cast<std::reference_wrapper<noconst> &>(this->m_held).get());
        }

        /**
         * Return this object's type_info
         */
        inline virtual const std::type_info &type(void) const override
        {
            return typeid(std::reference_wrapper<noconst>);
        }
    };
};

/**
 * Exception structure thrown as a result of bad any casts
 */
struct bad_any_cast
{
public:

    inline const char *what(void) const throw()
    {
        return "Failed conversion using any_cast";
    }
};

/**
 * Function to perform safe cast from Any to type T
 */
template<typename T>
inline T *any_cast(Any *pAny) noexcept
{
    if (pAny != nullptr)
    {
        typedef typename std::remove_cv<typename traits::helpers::reference_unwrapper<T>::type>::type noconst;
        typedef typename std::conditional<std::is_copy_constructible<typename std::decay<T>::type>::value,
                                          T, typename std::add_lvalue_reference<T>::type>::type type;

        auto &&name = pAny->type().name();
        if (std::strcmp(name, typeid(type).name()) == 0)
            return &static_cast<Any::Held<type> *>(pAny->holder())->m_held;
        else if (std::strcmp(name, typeid(std::reference_wrapper<noconst>).name()) == 0)
            return &(static_cast<Any::Held<std::reference_wrapper<noconst>> *>(pAny->holder())->m_held).get();
    }

    return nullptr;
}

/**
 * Function to perform safe cast from Any to type T
 */
template<typename T>
inline const T *any_cast(const Any *pAny) noexcept
{
    return any_cast<T>(const_cast<Any *>(pAny));
}

/**
 * Function to perform safe cast from Any to type T
 */
template<typename T>
T any_cast(Any &any)
{
    typedef typename std::remove_reference<T>::type nonref;
    nonref *pResult = any_cast<nonref>(&any);
    if (pResult == nullptr)
        throw bad_any_cast();

    return *pResult;
}

/**
 * Function to perform safe cast from Any to type T
 */
template<typename T>
inline T any_cast(const Any &any)
{
    typedef typename std::remove_reference<T>::type nonref;

    return any_cast<const nonref &>(const_cast<Any &>(any));
}

/**
 * Function to perform "unsafe" cast from Any to type T
 */
template<typename T>
inline T *unsafe_any_cast(Any *pAny) noexcept
{
    return &static_cast<Any::Held<T> *>(pAny->holder())->m_held;
}

/**
 * Function to perform "unsafe" cast from Any to type T
 */
template<typename T>
inline const T *unsafe_any_cast(const Any *pAny) noexcept
{
    return unsafe_any_cast<T>(const_cast<Any *>(pAny));
}

/**
 * Output stream operator
 */
template<typename T,
         typename std::enable_if<std::is_same<Any, typename std::decay<T>::type>::value, int>::type = 0>
inline auto operator << (std::ostream &stream, T &&value) -> decltype(auto)
{
    return static_cast<const Any &>(value).print(stream);
}

}

#endif
