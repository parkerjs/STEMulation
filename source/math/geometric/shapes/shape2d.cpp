#include "shape2d.h"

namespace math
{

namespace geometric
{

namespace shapes
{

/**
 * Constructor
 */
Shape2d::Shape2d(void)
{

}

/**
 * Constructor
 * @param plane the plane in which this shape lies
 */
Shape2d::Shape2d(const Plane &plane)
: m_plane(plane)
{

}

/**
 * Copy constructor
 */
Shape2d::Shape2d(const Shape2d &shape)
{
    operator = (shape);
}

/**
 * Move constructor
 */
Shape2d::Shape2d(Shape2d &&shape)
{
    operator = (std::move(shape));
}

/**
 * Destructor
 */
Shape2d::~Shape2d(void)
{

}

/**
 * Copy assignment operator
 */
Shape2d &Shape2d::operator = (const Shape2d &shape)
{
    if (&shape != this)
    {
        Shape::operator = (shape);

        m_plane = shape.m_plane;
    }

    return *this;
}

/**
 * Move assignment operator
 */
Shape2d &Shape2d::operator = (Shape2d &&shape)
{
    if (&shape != this)
    {
        Shape::operator = (std::move(shape));

        m_plane = std::move(shape.m_plane);
    }

    return *this;
}

/**
 * Get the name of this class
 */
std::string Shape2d::getClassName(void) const
{
    return "Shape2d";
}

/**
 * Get the plane in which this shape lies
 */
Plane &Shape2d::getPlane(void)
{
    return m_plane;
}

/**
 * Get the plane in which this shape lies
 */
Plane Shape2d::getPlane(void) const
{
    return m_plane;
}

/**
 * Set the plane in which this shape lies
 */
void Shape2d::setPlane(const Plane &plane)
{
    m_plane = plane;
}

}

}

}
