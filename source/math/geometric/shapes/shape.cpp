#include "rayIntersection.h"
#include "shape.h"

namespace math
{

namespace geometric
{

namespace shapes
{

/**
 * Constructor
 */
Shape::Shape(void)
{

}

/**
 * Copy constructor
 */
Shape::Shape(const Shape &shape)
{
    operator = (shape);
}

/**
 * Move constructor
 */
Shape::Shape(Shape &&shape)
{
    operator = (std::move(shape));
}

/**
 * Destructor
 */
Shape::~Shape(void)
{

}

/**
 * Copy assignment operator
 */
Shape &Shape::operator = (const Shape &shape)
{
    if (&shape != this)
    {
        // TODO: add members here
    }

    return *this;
}

/**
 * Move assignment operator
 */
Shape &Shape::operator = (Shape &&shape)
{
    if (&shape != this)
    {
        // TODO: add members here
    }

    return *this;
}

/**
 * Get the name of this class
 */
std::string Shape::getClassName(void) const
{
    return "Shape";
}

/**
 * Calculate the location at which a ray intersects this shape; returns a non-empty ray intersection object if
 * at least one intersection occurs
 */
RayIntersection Shape::intersect(const Ray &ray)
{
    RayIntersection intersection;

    intersect(ray, intersection);

    return intersection;
}

}

}

}
