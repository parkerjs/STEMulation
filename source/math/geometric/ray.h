#ifndef RAY_H
#define RAY_H

#include "reflective.h"
#include "vector3d.h"

namespace math
{

namespace geometric
{

// forward declarations
class Line;

/**
 * This class represents a geometric ray in 3-d space
 */
class Ray
: virtual private attributes::abstract::Reflective
{
public:

    /**
     * Constructor
     */
    EXPORT_STEM Ray(void);

    /**
     * Constructor
     * @param direction the ray's direction vector
     * @param origin    the coordinates of the ray's origin
     */
    EXPORT_STEM Ray(const linear_algebra::vector::Vector3d &direction,
                    const linear_algebra::vector::Vector3d &origin);

    /**
     * Copy constructor
     */
    EXPORT_STEM Ray(const Ray &ray);

    /**
     * Move constructor
     */
    EXPORT_STEM Ray(Ray &&ray);

    /**
     * Destructor
     */
    EXPORT_STEM ~Ray(void) override;

    /**
     * Implicit conversion to Line
     */
    EXPORT_STEM operator Line (void);

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Ray &operator = (const Ray &ray);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Ray &operator = (Ray &&ray);

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the direction vector of this ray
     */
    EXPORT_STEM virtual linear_algebra::vector::Vector3d &getDirection(void) final;

    /**
     * Get the direction vector of this ray
     */
    EXPORT_STEM virtual linear_algebra::vector::Vector3d getDirection(void) const final;

    /**
     * Get the origin of this ray
     */
    EXPORT_STEM virtual linear_algebra::vector::Vector3d &getOrigin(void) final;

    /**
     * Get the origin of this ray
     */
    EXPORT_STEM virtual linear_algebra::vector::Vector3d getOrigin(void) const final;

    /**
     * Return a 3d-vector representing a position along the ray as a function of the parametric value t
     */
    EXPORT_STEM virtual void getPosition(double t,
                                         linear_algebra::vector::Vector3d &position) const final;

    /**
     * Return a 3d-vector representing a position along the ray as a function of the parametric value t
     */
    EXPORT_STEM virtual linear_algebra::vector::Vector3d getPosition(double t) const final;

    /**
     * Set the direction of this ray
     */
    EXPORT_STEM virtual void setDirection(const linear_algebra::vector::Vector3d &direction) final;

    /**
     * Set the direction of this ray
     */
    EXPORT_STEM virtual void setDirection(double x,
                                          double y,
                                          double z) final;

    /**
     * Set the origin of this ray
     */
    EXPORT_STEM virtual void setOrigin(const linear_algebra::vector::Vector3d &origin) final;

    /**
     * Set the origin of this ray
     */
    EXPORT_STEM virtual void setOrigin(double x,
                                       double y,
                                       double z) final;

private:

    /**
     * the ray's direction vector
     */
    linear_algebra::vector::Vector3d m_direction;

    /**
     * the origin of this ray
     */
    linear_algebra::vector::Vector3d m_origin;
};

}

}

#endif
