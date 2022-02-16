#ifndef SPHERE_H
#define SPHERE_H

#include "shape3d.h"
#include "vector3d.h"

namespace math
{

namespace geometric
{

namespace shapes
{

/**
 * This class represents a 3-d sphere
 */
class Sphere
: virtual private attributes::abstract::Reflective,
  public Shape3d
{
public:

    /**
     * Using declarations
     */
    using Shape3d::intersect;

    /**
     * Constructor
     */
    EXPORT_STEM Sphere(void);

    /**
     * Constructor
     */
    EXPORT_STEM Sphere(double radius);

    /**
     * Constructor
     */
    EXPORT_STEM Sphere(const linear_algebra::vector::Vector3d &origin,
                       double radius = 1.0);

    /**
     * Copy constructor
     */
    EXPORT_STEM Sphere(const Sphere &sphere);

    /**
     * Move constructor
     */
    EXPORT_STEM Sphere(Sphere &&sphere);

    /**
     * Destructor
     */
    EXPORT_STEM ~Sphere(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Sphere &operator = (const Sphere &sphere);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Sphere &operator = (Sphere &&sphere);

    /**
     * Calculate the area of this shape
     */
    EXPORT_STEM virtual double calcArea(void) const override;

    /**
     * Calculate the volume of this shape
     */
    EXPORT_STEM virtual double calcVolume(void) const override;

    /**
     * clone() function
     */
    EXPORT_STEM virtual Sphere *clone(void) const override;

    /**
     * create() function
     */
    static EXPORT_STEM Sphere *create(void);

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override final;

    /**
     * Get the origin of this shape
     */
    EXPORT_STEM virtual linear_algebra::vector::Vector3d &getOrigin(void) final;

    /**
     * Get the origin of this shape
     */
    EXPORT_STEM virtual linear_algebra::vector::Vector3d getOrigin(void) const final;

    /**
     * Get the sphere's radius
     */
    EXPORT_STEM virtual double getRadius(void) const final;

    /**
     * Calculate the location at which a ray intersects this shape; returns true if at least one intersection
     * occurs
     */
    EXPORT_STEM virtual bool intersect(const Ray &ray,
                                       RayIntersection &intersection) override;

    /**
     * Set the origin of this shape
     */
    EXPORT_STEM virtual void setOrigin(const linear_algebra::vector::Vector3d &origin);

    /**
     * Set the origin of this shape
     */
    EXPORT_STEM virtual void setOrigin(double x,
                                       double y,
                                       double z);

    /**
     * Set the sphere's radius
     */
    EXPORT_STEM virtual void setRadius(double radius) final;

private:

    /**
     * the origin of this
     */
    linear_algebra::vector::Vector3d m_origin;

    /**
     * the radius of this sphere
     */
    double m_radius;
};

}

}

}

#endif
