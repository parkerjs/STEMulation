#ifndef RAY_INTERSECTION_H
#define RAY_INTERSECTION_H

#include "reflective.h"
#include "vector3d.h"

namespace math
{

namespace geometric
{

/**
 * This class provides a container to store the point(s) at which a ray intersects a geometric object
 */
class RayIntersection
: virtual private attributes::abstract::Reflective
{
public:

    /**
     * Constructor
     */
    EXPORT_STEM RayIntersection(void);

    /**
     * Copy constructor
     */
    EXPORT_STEM RayIntersection(const RayIntersection &intersection);

    /**
     * Move constructor
     */
    EXPORT_STEM RayIntersection(RayIntersection &&intersection);

    /**
     * Destructor
     */
    EXPORT_STEM ~RayIntersection(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM RayIntersection &operator = (const RayIntersection &intersection);

    /**
     * Move assignment operator
     */
    EXPORT_STEM RayIntersection &operator = (RayIntersection &&intersection);

    /**
     * Add an intersection point
     */
    EXPORT_STEM virtual void add(const linear_algebra::vector::Vector3d &point) final;

    /**
     * Add an intersection point
     */
    EXPORT_STEM virtual void add(double x,
                                 double y,
                                 double z) final;

    /**
     * Clear this object of all intersection points
     */
    EXPORT_STEM virtual void clear(void) final;

    /**
     * Test for empty object
     */
    EXPORT_STEM virtual bool empty(void) const final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the intersection points
     */
    EXPORT_STEM virtual std::vector<linear_algebra::vector::Vector3d> &getPoints(void) final;

    /**
     * Get the number of intersection points
     */
    EXPORT_STEM virtual std::size_t size(void) const final;

private:

    /**
     * a vector of intersection points
     */
    std::vector<linear_algebra::vector::Vector3d> m_points;
};

}

}

#endif
