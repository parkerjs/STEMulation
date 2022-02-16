#ifndef SHAPE2D_H
#define SHAPE2D_H

#include "plane.h"
#include "shape.h"

namespace math
{

namespace geometric
{

namespace shapes
{

/**
 * This abstract class represents a 2-d geometric shape
 */
class Shape2d
: public Shape,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Constructor
     */
    EXPORT_STEM Shape2d(void);

    /**
     * Constructor
     * @param plane the plane in which this shape lies
     */
    EXPORT_STEM Shape2d(const Plane &plane);

    /**
     * Copy constructor
     */
    EXPORT_STEM Shape2d(const Shape2d &shape);

    /**
     * Move constructor
     */
    EXPORT_STEM Shape2d(Shape2d &&shape);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Shape2d(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Shape2d &operator = (const Shape2d &shape);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Shape2d &operator = (Shape2d &&shape);

    /**
     * Calculate the perimeter of this shape
     */
    EXPORT_STEM virtual double calcPerimeter(void) const = 0;

    /**
     * clone() function
     */
    EXPORT_STEM virtual Shape2d *clone(void) const override = 0;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the plane in which this shape lies
     */
    EXPORT_STEM virtual Plane &getPlane(void) final;

    /**
     * Get the plane in which this shape lies
     */
    EXPORT_STEM virtual Plane getPlane(void) const final;

    /**
     * Set the plane in which this shape lies
     */
    EXPORT_STEM virtual void setPlane(const Plane &plane) final;

protected:

    /**
     * the plane in which this shape lies
     */
    Plane m_plane;
};

}

}

}

#endif
