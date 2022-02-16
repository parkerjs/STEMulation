#ifndef PLANE_H
#define PLANE_H

#include "angle_unit_type.h"
#include "reflective.h"
#include "vector3d.h"

namespace math
{

namespace geometric
{

// forward declarations
class Line;
class Ray;
class RayIntersection;

/**
 * This class represents a geometric plane in 3-d space
 */
class Plane
: virtual private attributes::abstract::Reflective
{
public:

    /**
     * Type alias declarations
     */
    using AngleUnitType = trigonometric::AngleUnitType;
    using Vector2d = linear_algebra::vector::Vector2d;
    using Vector3d = linear_algebra::vector::Vector3d;

    /**
     * Constructor
     */
    EXPORT_STEM Plane(void);

    /**
     * Constructor
     * @param normal the vector normal to the plane
     * @param origin the coordinates of the plane's origin
     */
    EXPORT_STEM Plane(const Vector3d &normal,
                      const Vector3d &origin);

    /**
     * Copy constructor
     */
    EXPORT_STEM Plane(const Plane &plane);

    /**
     * Move constructor
     */
    EXPORT_STEM Plane(Plane &&plane);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Plane(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Plane &operator = (const Plane &plane);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Plane &operator = (Plane &&plane);

    /**
     * Calculate the distance from the specified point to this plane
     */
    EXPORT_STEM virtual double calcDistance(const Vector3d &point) const final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get this plane's normal vector
     */
    EXPORT_STEM virtual Vector3d &getNormal(void) final;

    /**
     * Get this plane's normal vector
     */
    EXPORT_STEM virtual Vector3d getNormal(void) const final;

    /**
     * Return this plane's origin
     */
    EXPORT_STEM virtual Vector3d &getOrigin(void) final;

    /**
     * Return this plane's origin
     */
    EXPORT_STEM virtual Vector3d getOrigin(void) const final;

    /**
     * Function to determine if a ray intersects this shape; returns true if at least one intersection occurs
     */
    EXPORT_STEM virtual bool intersect(const Ray &ray);

    /**
     * Calculate the location at which a ray intersects this shape; returns true if at least one intersection
     * occurs
     */
    EXPORT_STEM virtual bool intersect(const Ray &ray,
                                       RayIntersection &intersection);

    /**
     * Calculate the intersection of the plane described by the current object with another plane; algorithm
     * adapted from the following source: http://geomalgorithms.com/a05-_intersect-1.html
     * @return an integer that indicates the following:
     *         = 0, the planes do not intersect
     *         = 1, the planes are coincident
     *         = 2, the planes intersect
     */
    EXPORT_STEM virtual int intersect(const Plane &plane,
                                      Line &intersection);

    /**
     * Determine if the plane described by the current object is orthogonal to another plane
     */
    EXPORT_STEM virtual bool orthogonal(const Plane &plane) const final;

    /**
     * Determine if the plane described by the current object is parallel to another plane
     */
    EXPORT_STEM virtual bool parallel(const Plane &plane) const final;

    /**
     * Function to project a 3-d point onto this plane; returns a 2-d location on the plane
     */
    EXPORT_STEM virtual Vector2d project(const Vector3d &point3d) final;

    /**
     * Function to project a 3-d point onto this plane
     * @param point3d    the input point in 3d space
     * @param point2d    upon success, represents the output 2-d point projected onto this plane
     * @param angle      the angle between this plane's normal axis and the parent-frame z-axis
     * @param axis       the cross-product of this plane's normal axis and the parent-frame z-axis
     * @param angleUnits the angle units of the input angle argument, Degrees or Radians
     */
    EXPORT_STEM virtual void project(Vector3d point3d,
                                     Vector2d &point2d,
                                     double angle,
                                     const Vector3d &axis,
                                     const AngleUnitType &angleUnits = AngleUnitType::Degrees) final;

    /**
     * Set this plane's normal vector
     */
    EXPORT_STEM virtual void setNormal(const Vector3d &normal) final;

    /**
     * Set this plane's normal vector
     */
    EXPORT_STEM virtual void setNormal(double x,
                                       double y,
                                       double z) final;

    /**
     * Set this plane's origin
     */
    EXPORT_STEM virtual void setOrigin(const Vector3d &origin) final;

    /**
     * Set this plane's origin
     */
    EXPORT_STEM virtual void setOrigin(double x,
                                       double y,
                                       double z) final;

    /**
     * Function to un-project a 2-d point from this plane back into 3-d coordinates
     */
    EXPORT_STEM virtual Vector3d unproject(const Vector2d &point2d) final;

    /**
     * Function to un-project a 2-d point from this plane back into 3-d coordinates
     * @param point2d    the input 2-d point located on this plane
     * @param point3d    upon success, represents the output point in 3d space
     * @param angle      the angle between this plane's normal axis and the parent-frame z-axis
     * @param axis       the cross-product of this plane's normal axis and the parent-frame z-axis
     * @param angleUnits the angle units of the input angle argument, Degrees or Radians
     */
    EXPORT_STEM virtual void unproject(Vector2d point2d,
                                       Vector3d &point3d,
                                       double angle,
                                       const Vector3d &axis,
                                       const AngleUnitType &angleUnits = AngleUnitType::Degrees) final;

private:

    /**
     * the vector normal to the plane
     */
    Vector3d m_normal;

    /**
     * this plane's origin
     */
    Vector3d m_origin;
};

}

}

#endif
