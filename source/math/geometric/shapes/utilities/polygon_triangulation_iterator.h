#ifndef POLYGON_TRIANGULATION_ITERATOR_H
#define POLYGON_TRIANGULATION_ITERATOR_H

#include "input_iterator.h"
#include "vector2d.h"
#include <cstring>
#include <functional>

namespace iterators
{

// forward declarations
struct polygon_triangulation_iterator_tag;

/**
 * This specialization implements a custom polygon triangulation iterator for the PolygonTriangulator class
 */
template<typename T, typename ... categories>
class Iterator<T, false, polygon_triangulation_iterator_tag, categories ...>
: public Iterator<T, false, std::input_iterator_tag, polygon_triangulation_iterator_tag, categories ...>
{
public:

    /**
     * Typedef declarations
     */
    typedef Iterator<T, false, std::input_iterator_tag, polygon_triangulation_iterator_tag,
                     categories ...> base_iterator;
    typedef typename std::tuple_element<sizeof ... (categories),
                                        std::tuple<polygon_triangulation_iterator_tag,
                                                   categories ...>>::type category;
    typedef Iterator<const T, false, polygon_triangulation_iterator_tag, categories ...> const_iterator;
    typedef typename std::decay<T>::type decay_type;
    typedef Iterator<T, false, category> derived_iterator;
    typedef typename base_iterator::difference_type difference_type;
    typedef Iterator<T, false, polygon_triangulation_iterator_tag, categories ...> iterator;
    typedef typename base_iterator::iterator_category iterator_category;
    typedef typename base_iterator::pointer pointer;
    typedef typename base_iterator::reference reference;
    typedef typename base_iterator::value_type value_type;

    /**
     * Friend declarations
     */
    friend const_iterator;

    /**
     * Type alias declarations
     */
    using Polygon = math::geometric::shapes::Polygon;
    using Triangle = math::geometric::shapes::Triangle;
    using Vector2d = math::linear_algebra::vector::Vector2d;

    /**
     * Using declarations
     */
    using base_iterator::operator ++;

    /**
     * Constructor
     * @param vertices a vector of vertices defining the polygon
     */
    Iterator(const std::vector<Vector2d> &vertices)
    : base_iterator(nullptr),
      m_numVerticesRemaining(vertices.size()),
      m_triangulationFunctor([] (auto &&, auto &&, auto &&, auto &&) { return false; }),
      m_vertices(vertices)
    {

    }

    /**
     * Constructor
     * @param vertices             a vector of vertices defining the polygon
     * @param triangulationFunctor a function object that is responsible for finding the next triangle
     */
    template<typename Functor>
    Iterator(const std::vector<Vector2d> &vertices,
             Functor &&triangulationFunctor)
    : base_iterator(nullptr),
      m_numVerticesRemaining(vertices.size()),
      m_triangulationFunctor(triangulationFunctor),
      m_vertices(vertices)
    {

    }

    /**
     * Copy constructor
     */
    Iterator(const iterator &it)
    : base_iterator(it)
    {
        operator = (it);
    }

    /**
     * Conversion to constant iterator
     */
    template<typename U = typename std::remove_const<T>::type,
             typename std::enable_if<std::is_same<T, const U>::value, int>::type = 0>
    Iterator(const Iterator<U, false, polygon_triangulation_iterator_tag, categories ...> &it)
    : base_iterator(it)
    {

    }

    /**
     * Destructor
     */
    virtual ~Iterator(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    iterator &operator = (const iterator &it)
    {
        if (&it != this)
        {
            base_iterator::operator = (it);

            m_indices = it.m_indices;
            m_numVerticesRemaining = it.m_numVerticesRemaining;
            m_triangle = it.m_triangle;
            m_triangulationFunctor = it.m_triangulationFunctor;
            m_vertices = it.m_vertices;
        }

        return *this;
    }

    /**
     * Indirection operator
     */
    inline virtual reference operator * (void) final
    {
        return m_triangle;
    }

    /**
     * Indirection operator
     */
    inline virtual value_type operator * (void) const override
    {
        return m_triangle;
    }

    /**
     * Structure dereference operator
     */
    inline virtual pointer operator -> (void) const override
    {
        return const_cast<pointer>(&m_triangle);
    }

    /**
     * Prefix increment operator
     */
    inline virtual iterator &operator ++ (void) override
    {
        m_triangulationFunctor(m_vertices, m_indices, m_triangle, m_numVerticesRemaining);

        return *this;
    }

    /**
     * Compare equal operator
     */
    inline virtual bool operator == (const iterator &it) const override
    {
        return m_numVerticesRemaining == it.m_numVerticesRemaining;
    }

    /**
     * swap function
     */
    inline virtual void swap(iterator &it) override final
    {
        base_iterator::swap(it);

        m_indices.swap(it.m_indices);
        std::swap(m_numVerticesRemaining, it.m_numVerticesRemaining);
        std::swap(m_triangle, it.m_triangle);
        std::swap(m_triangulationFunctor, it.m_triangulationFunctor);
        m_vertices.swap(it.m_vertices);
    }

private:

    /**
     * an index vector which ensures that the polygon is traversed in a counter-clockwise order
     */
    std::vector<std::size_t> m_indices;

    /**
     * the number of vertices remaining to be processed
     */
    std::size_t m_numVerticesRemaining;

    /**
     * the current triangle extracted from the polygon
     */
    decay_type m_triangle;

    /**
     * function object that is responsible for finding the next triangle
     */
    std::function<bool (const std::vector<Vector2d> &,
                        std::vector<std::size_t> &,
                        decay_type &,
                        std::size_t &)> m_triangulationFunctor;

    /**
     * a vector of vertices defining the polygon
     */
    std::vector<Vector2d> m_vertices;
};

}

#endif
