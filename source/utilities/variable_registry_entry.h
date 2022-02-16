#ifndef VARIABLE_REGISTRY_ENTRY_H
#define VARIABLE_REGISTRY_ENTRY_H

#include "any.h"
#include "csv_tokenizer.h"
#include "holder.h"
#include "is_input_streamable.h"
#include "is_output_streamable.h"
#include "is_resizeable_sequence_container.h"
#include "is_std_basic_string.h"
#include "is_std_set.h"
#include "swappable.h"
#include <cctype>
#include <cstring>
#include <functional>
#include <map>
#include <set>
#include <sstream>
#include <tuple>

namespace utilities
{

// forward declarations
class VariableRegistryEntry;
template<typename T> T registry_entry_cast(VariableRegistryEntry &);

/**
 * Exception structure thrown as a result of bad variable registry entry casts
 */
struct bad_registry_entry_cast
{
public:

    inline const char *what(void) const throw()
    {
        return "Failed conversion using bad_registry_entry_cast";
    }
};

/**
 * This class serves as a container to hold variable references and can be used in conjunction with a variable
 * registry class to manipulate variable entries within a dictionary
 */
class VariableRegistryEntry final
: public attributes::interfaces::Cloneable<VariableRegistryEntry>,
  public attributes::interfaces::Swappable<VariableRegistryEntry>,
  public std::tuple<functional::Holder *>
{
public:

    /**
     * Friend declarations
     */
    template<typename T> friend T *registry_entry_cast(VariableRegistryEntry *) noexcept;
    template<typename T> friend T *unsafe_registry_entry_cast(VariableRegistryEntry *) noexcept;

    /**
     * Constructor
     */
    VariableRegistryEntry(void) noexcept
    : std::tuple<functional::Holder *>(nullptr)
    {

    }

    /**
     * Constructor
     */
    template<typename T, typename U = typename std::decay<T>::type,
             typename std::enable_if<!std::is_same<std::tuple<functional::Holder *>, U>::value &&
                                     !std::is_base_of<std::tuple<functional::Holder *>, U>::value, int>::type = 0>
    VariableRegistryEntry(T &&variable) noexcept
    : std::tuple<functional::Holder *>(new Held<T>(std::forward<T>(variable)))
    {

    }

    /**
     * Copy constructor
     */
    VariableRegistryEntry(const VariableRegistryEntry &entry)
    : std::tuple<functional::Holder *>(entry.holder() != nullptr ? entry.holder()->clone() : nullptr)
    {

    }

    /**
     * Move constructor
     */
    VariableRegistryEntry(VariableRegistryEntry &&entry) noexcept
    : std::tuple<functional::Holder *>(std::get<functional::Holder *>(entry))
    {
        std::get<functional::Holder *>(entry) = nullptr;
    }

    /**
     * Destructor
     */
    virtual ~VariableRegistryEntry(void) noexcept
    {
        auto *pHeld = holder();
        if (pHeld != nullptr)
            delete pHeld;
    }

    /**
     * Copy assignment operator
     */
    VariableRegistryEntry &operator = (const VariableRegistryEntry &entry)
    {
        if (&entry != this)
        {
            VariableRegistryEntry(entry).swap(*this);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    VariableRegistryEntry &operator = (VariableRegistryEntry &&entry)
    {
        entry.swap(*this);
        VariableRegistryEntry().swap(entry);

        return *this;
    }

    /**
     * Assignment operator
     */
    template<typename T, typename U = typename std::decay<T>::type>
    inline typename std::enable_if<!std::is_same<std::tuple<functional::Holder *>, U>::value &&
                                   !std::is_base_of<std::tuple<functional::Holder *>, U>::value,
                                   VariableRegistryEntry &>::type
    operator = (T &&variable)
    {
        VariableRegistryEntry(std::forward<T>(variable)).swap(*this);

        return *this;
    }

    /**
     * Implicit conversion operator
     */
    template<typename T>
    inline operator T (void) const
    {
        return registry_entry_cast<T>(*this);
    }

    /**
     * Implicit conversion operator
     */
    template<typename T>
    inline operator T & (void)
    {
        return registry_entry_cast<T &>(*this);
    }

    /**
     * Test for whether or not this entry contains valid content
     */
    inline explicit operator bool (void) const
    {
        return !empty();
    }

    /**
     * Compare equal operator
     */
    bool operator == (const VariableRegistryEntry &entry) const
    {
        auto *pHolder = static_cast<Holder *>(entry.holder());
        auto *pThisHolder = static_cast<Holder *>(holder());
        bool bEqual = (pHolder != nullptr && pThisHolder != nullptr);
        if (bEqual)
            bEqual = (*pHolder == *pThisHolder);

        return bEqual;
    }

    /**
     * Assign a value to this entry's content
     */
    template<typename T, typename U = typename std::decay<T>::type>
    typename std::enable_if<!std::is_base_of<std::istream, U>::value &&
                            !std::is_base_of<std::tuple<functional::Holder *>, U>::value &&
                            !traits::tests::is_std_basic_string<U>::value, bool>::type
    assign(T &value)
    {
        T &variable = registry_entry_cast<T &>(*this);

        variable = std::forward<T>(value);

        return true;
    }

    /**
     * Read token(s) from the input stream and attempt to convert and assign the result as the value of this
     * object's content
     */
    template<typename T>
    typename std::enable_if<std::is_base_of<std::istream, typename std::decay<T>::type>::value, bool>::type
    assign(T &stream)
    {
        auto *pHolder = static_cast<Holder *>(holder());
        bool bSuccess = (pHolder != nullptr);
        if (bSuccess)
            bSuccess = pHolder->assign(stream);

        return bSuccess;
    }

    /**
     * Read token(s) from the input string and attempt to convert and assign the result as the value of this
     * object's content
     */
    template<typename T>
    typename std::enable_if<traits::tests::is_std_basic_string<typename std::decay<T>::type>::value, bool>::type
    assign(T &string)
    {
        auto *pHolder = static_cast<Holder *>(holder());
        bool bSuccess = (pHolder != nullptr);
        if (bSuccess)
            bSuccess = pHolder->assign(string);

        return bSuccess;
    }

    /**
     * Assign the content value of one entry to another
     */
    template<typename T>
    typename std::enable_if<std::is_same<VariableRegistryEntry,
                            typename std::decay<T>::type>::value, bool>::type
    assign(T &entry)
    {
        auto *pHolder = static_cast<Holder *>(entry.holder());
        auto *pThisHolder = static_cast<Holder *>(holder());
        bool bSuccess = (pHolder != nullptr && pThisHolder != nullptr);
        if (bSuccess)
            bSuccess = pThisHolder->assign(*pHolder);

        return bSuccess;
    }

    /**
     * clone() function
     */
    inline virtual VariableRegistryEntry *clone(void) const override
    {
        return new VariableRegistryEntry(*this);
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
    std::ostream &print(std::ostream &stream) const
    {
        auto *pHolder = static_cast<Holder *>(holder());
        if (pHolder != nullptr)
            pHolder->print(stream);
        else
            stream.setstate(std::istream::failbit);

        return stream;
    }

    /**
     * Swap contents of this VariableRegistryEntry with another
     */
    inline void swap(VariableRegistryEntry &entry) noexcept override
    {
        std::tuple<functional::Holder *>::swap(entry);
    }

    /**
     * Swap contents of two VariableRegistryEntry objects
     */
    inline friend void swap(VariableRegistryEntry &lhs,
                            VariableRegistryEntry &rhs) noexcept
    {
        lhs.swap(rhs);
    }

    /**
     * Function to convert VariableRegistryEntry to functional::Any object
     */
    virtual functional::Any toAny(void) const final
    {
        auto *pHolder = static_cast<VariableRegistryEntry::Holder *>(holder());
        if (pHolder == nullptr)
            throw bad_registry_entry_cast();

        functional::Any any;
        if (pHolder != nullptr)
            pHolder->toAny(any);

        return any;
    }

    /**
     * Function to convert this object to a string
     */
    virtual std::string toString(void) const final
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
         * Compare equal operator
         */
        virtual bool operator == (const Holder &holder) const = 0;

        /**
         * Read token(s) from the input stream and attempt to convert and assign the result as the value of
         * this object's content
         */
        virtual bool assign(std::istream &stream) = 0;

        /**
         * Read token(s) from the input string and attempt to convert and assign the result as the value of
         * this object's content
         */
        virtual bool assign(const std::string &string) = 0;

        /**
         * Assign the content value of one entry to another
         */
        virtual bool assign(const Holder &holder) = 0;

        /**
         * Output stream print function
         * @param stream a reference to an std::ostream object
         */
        virtual std::ostream &print(std::ostream &stream) const = 0;

        /**
         * Function to convert VariableRegistryEntry to functional::Any object
         */
        virtual void toAny(functional::Any &any) const = 0;
    };

    /**
     * This class implements the abstract base Holder class
     */
    template<typename T>
    class Held final
    : public Holder,
      public functional::Held<T &>
    {
    public:

        /**
         * Constructor
         */
        template<typename U>
        Held(U &&value)
        : functional::Held<T &>(std::forward<U>(value))
        {

        }

        /**
         * Destructor
         */
        virtual ~Held(void) override
        {

        }

        /**
         * Compare equal operator
         */
        virtual bool operator == (const Holder &holder) const override
        {
            const auto *pHeld = dynamic_cast<const Held<T> *>(&holder);
            if (pHeld != nullptr)
                return &this->m_held == &pHeld->m_held;

            return false;
        }

    private:

        /**
         * assign() overload for re-sizable sequence containers (excluding std::basic_string)
         */
        template<template<typename, typename ...> class Content, typename Type, typename ... Args>
        static auto assign(std::istream &stream,
                           Content<Type, Args ...> &content) ->
        typename std::enable_if<traits::tests::is_resizeable_sequence_container<decltype(content)>::value &&
                                !std::is_const<decltype(content)>::value &&
                                !traits::tests::is_std_basic_string<
                                typename std::decay<decltype(content)>::type>::value, bool>::type
        {
            utilities::CsvTokenizer csvTokenizer(true, true, true);
            bool bSuccess = csvTokenizer.initialize(stream);
            if (bSuccess)
            {
                auto &&itContent = content.begin();
                auto &&itToken = csvTokenizer.cbegin();
                while (bSuccess && itToken != csvTokenizer.cend())
                {
                    Type value;
                    bSuccess = assign(*itToken++, value);
                    if (bSuccess)
                    {
                        if (itContent == content.end())
                            itContent = content.insert(content.end(), value);
                        else
                            *itContent = value;

                        ++itContent;
                    }
                }

                // remove any excess elements
                content.erase(itContent, content.end());
            }

            return bSuccess;
        }

        /**
         * assign() overload for associative set containers
         */
        template<typename Type>
        inline static bool assign(std::istream &stream,
                                  std::set<Type> &content)
        {
            utilities::CsvTokenizer csvTokenizer(true, true, true);
            bool bSuccess = csvTokenizer.initialize(stream);
            if (bSuccess)
            {
                auto &&itToken = csvTokenizer.cbegin();
                while (bSuccess && itToken != csvTokenizer.cend())
                {
                    Type value;
                    bSuccess = assign(*itToken++, value);
                    if (bSuccess)
                        content.insert(value);
                }
            }

            return bSuccess;
        }

        /**
         * assign() overload for non-stream-able or constant types
         */
        template<typename Content>
        inline static auto assign(std::istream &stream,
                                  Content &&) ->
        typename std::enable_if<std::is_const<typename std::remove_reference<Content>::type>::value ||
                               !traits::tests::is_input_streamable<std::istream, Content>::value, bool>::type
        {
            return false;
        }

        /**
         * assign() overload for std::istream
         */
        template<typename Content>
        inline static typename
        std::enable_if<traits::tests::is_input_streamable<std::istream, Content>::value &&
                       !std::is_const<typename std::remove_reference<Content>::type>::value, bool>::type
        assign(std::istream &stream,
               Content &&content)
        {
            return !(stream >> std::boolalpha >> content).fail();
        }

        /**
         * assign() overload for std::basic_string input with constant types
         */
        template<typename Content, typename ... Args>
        inline static typename
        std::enable_if<std::is_const<typename std::remove_reference<Content>::type>::value, bool>::type
        assign(const std::basic_string<Args ...> &,
               Content &&)
        {
            return false;
        }

        /**
         * assign() overload for std::basic_string
         */
        template<typename Content, typename ... Args>
        inline static typename
        std::enable_if<traits::tests::is_std_basic_string<typename std::decay<Content>::type>::value &&
                      !std::is_const<typename std::remove_reference<Content>::type>::value, bool>::type
        assign(const std::basic_string<Args ...> &string,
               Content &&content)
        {
            content = string;

            return true;
        }

        /**
         * assign() overload for std::basic_string input and a container object type other than
         * std::basic_string
         */
        template<typename Content, typename ... Args>
        inline static typename
        std::enable_if<!traits::tests::is_std_basic_string<typename std::decay<Content>::type>::value &&
                       !std::is_const<typename std::remove_reference<Content>::type>::value, bool>::type
        assign(const std::basic_string<Args ...> &string,
               Content &&content)
        {
            std::istringstream stream(string);

            return assign(stream, content);
        }

        /**
         * assign() overload to assign the content value of one entry to another when the target is an
         * assignable type
         */
        template<typename U, typename V>
        inline static auto assign(Held<U> &lhs,
                                  const Held<V> &rhs) ->
        typename std::enable_if<std::is_assignable<decltype(lhs.m_held), decltype(rhs.m_held)>::value &&
                               !std::is_const<typename std::remove_reference<decltype(lhs.m_held)>::type>::value,
                               bool>::type
        {
            lhs.m_held = rhs.m_held;

            return true;
        }

        /**
         * assign() overload to ignore assigning content value of one entry to another when the target is not
         * an assignable type
         */
        template<typename U, typename V>
        inline static auto assign(Held<U> &lhs,
                                  const Held<V> &rhs) ->
        typename std::enable_if<std::is_const<typename
                                std::remove_reference<decltype(lhs.m_held)>::type>::value ||
                               !std::is_assignable<decltype(lhs.m_held), decltype(rhs.m_held)>::value,
                                bool>::type
        {
            // silence compiler warnings
            (void)lhs;
            (void)rhs;

            return false;
        }

    public:

        /**
         * Read token(s) from the input stream and attempt to convert and assign the result as the value of
         * this object's content
         */
        inline virtual bool assign(std::istream &stream) override
        {
            return assign(stream, this->m_held);
        }

        /**
         * Read token(s) from the input string and attempt to convert and assign the result as the value of
         * this object's content
         */
        inline virtual bool assign(const std::string &string) override
        {
            return assign(string, this->m_held);
        }

        /**
         * Assign the content of this object's held content to another holder
         */
        virtual bool assign(const Holder &holder) override
        {
            auto *pHeld = dynamic_cast<const Held<T> *>(&holder);
            bool bSuccess = (pHeld != nullptr);
            if (bSuccess)
                return assign(*this, *pHeld);

            return bSuccess;
        }

        /**
         * clone() function
         */
        inline virtual Held<T> *clone(void) const override
        {
            return new Held<T>(this->m_held);
        }

    private:

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
        std::enable_if<!traits::tests::is_output_streamable<std::ostream, Content>::value,
                       std::ostream &>::type
        print(std::ostream &stream,
              Content &&)
        {
            return stream;
        }

    public:

        /**
         * Output stream print function
         * @param stream a reference to an std::ostream object
         */
        inline virtual std::ostream &print(std::ostream &stream) const override
        {
            return print(stream, this->m_held);
        }

    private:

        /**
         * Function to convert VariableRegistryEntry non-reference member variable to functional::Any object
         */
        template<typename U, typename DecayU = typename std::decay<U>::type,
                 typename std::enable_if<std::is_copy_constructible<DecayU>::value, int>::type = 0>
        inline static void toAny(functional::Any &any,
                                 U &&value)
        {
            any = value;
        }

        /**
         * Function to convert VariableRegistryEntry reference member variable to functional::Any object
         */
        template<typename U, typename DecayU = typename std::decay<U>::type,
                 typename std::enable_if<!std::is_copy_constructible<DecayU>::value, int>::type = 0>
        inline static void toAny(functional::Any &any,
                                 U &&value)
        {
            any = std::ref(value);
        }

    public:

        /**
         * Function to convert VariableRegistryEntry to functional::Any object
         */
        inline virtual void toAny(functional::Any &any) const override
        {
            toAny(any, this->m_held);
        }

        /**
         * Return this object's type_info
         */
        inline virtual const std::type_info &type(void) const override
        {
            return typeid(T);
        }
    };
};

/**
 * Function to perform safe cast from VariableRegistryEntry to type T
 */
template<typename T> inline T *registry_entry_cast(VariableRegistryEntry *pEntry) noexcept
{
    return (pEntry != nullptr && std::strcmp(pEntry->type().name(), typeid(T).name()) == 0) ?
           &static_cast<VariableRegistryEntry::Held<T> *>(pEntry->holder())->m_held : nullptr;
}

/**
 * Function to perform safe cast from VariableRegistryEntry to type T
 */
template<typename T> inline const T *registry_entry_cast(const VariableRegistryEntry *pEntry) noexcept
{
    return registry_entry_cast<T>(const_cast<VariableRegistryEntry *>(pEntry));
}

/**
 * Function to perform safe cast from VariableRegistryEntry to type T
 */
template<typename T> inline T registry_entry_cast(VariableRegistryEntry &entry)
{
    typedef typename std::remove_reference<T>::type nonref;
    nonref *pEntry = registry_entry_cast<nonref>(&entry);
    if (pEntry == nullptr)
        throw bad_registry_entry_cast();

    return *pEntry;
}

/**
 * Function to perform safe cast from VariableRegistryEntry to type T
 */
template<typename T> inline T registry_entry_cast(const VariableRegistryEntry &entry)
{
    typedef typename std::remove_reference<T>::type nonref;

    return registry_entry_cast<const nonref &>(const_cast<VariableRegistryEntry &>(entry));
}

/**
 * Function to perform "unsafe" cast from VariableRegistryEntry to type T
 */
template<typename T> inline T *unsafe_registry_entry_cast(VariableRegistryEntry *pEntry) noexcept
{
    return &static_cast<VariableRegistryEntry::Held<T> *>(pEntry->holder())->m_held;
}

/**
 * Function to perform "unsafe" cast from VariableRegistryEntry to type T
 */
template<typename T> inline const T *unsafe_registry_entry_cast(const VariableRegistryEntry *pEntry) noexcept
{
    return unsafe_registry_entry_cast<T>(const_cast<VariableRegistryEntry *>(pEntry));
}

}

#endif
