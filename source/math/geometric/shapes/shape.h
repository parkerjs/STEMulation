#ifndef SHAPE_H
#define SHAPE_H

#include "cloneable.h"
#include "export_library.h"
#include "factory_constructible.h"
#include "reflective.h"

namespace math
{

namespace geometric
{

// forward declarations
class Ray;
class RayIntersection;

namespace shapes
{

/**
 * This abstract class represents a geometric shape
 */
class Shape
: public attributes::interfaces::Cloneable<Shape>,
  public attributes::abstract::FactoryConstructible<Shape>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Constructor
     */
    EXPORT_STEM Shape(void);

    /**
     * Copy constructor
     */
    EXPORT_STEM Shape(const Shape &shape);

    /**
     * Move constructor
     */
    EXPORT_STEM Shape(Shape &&shape);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Shape(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Shape &operator = (const Shape &shape);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Shape &operator = (Shape &&shape);

    /**
     * Calculate the area of this shape
     */
    EXPORT_STEM virtual double calcArea(void) const = 0;

    /**
     * clone() function
     */
    EXPORT_STEM virtual Shape *clone(void) const override = 0;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Calculate the location at which a ray intersects this shape; returns a non-empty ray intersection object
     * if at least one intersection occurs
     */
    EXPORT_STEM virtual RayIntersection intersect(const Ray &ray);

    /**
     * Calculate the location at which a ray intersects this shape; returns true if at least one intersection
     * occurs
     */
    EXPORT_STEM virtual bool intersect(const Ray &ray,
                                       RayIntersection &intersection) = 0;
};

}

}

}

#endif
