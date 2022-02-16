#include "line.h"
#include "plane.h"
#include "ray.h"
#include "rayIntersection.h"
#include "vector2d.h"
#include <cmath>

// using namespace declarations
using namespace math::linear_algebra::vector;

namespace math
{

namespace geometric
{

/**
 * Constructor
 */
Plane::Plane(void)
{
    setNormal(0, 0, 1);
}

/**
 * Constructor
 * @param normal the vector normal to the plane
 * @param origin the coordinates of the plane's origin
 */
Plane::Plane(const Vector3d &normal,
             const Vector3d &origin)
: m_origin(origin)
{
    setNormal(normal);
}

/**
 * Copy constructor
 */
Plane::Plane(const Plane &plane)
{
    operator = (plane);
}

/**
 * Move constructor
 */
Plane::Plane(Plane &&plane)
{
    operator = (std::move(plane));
}

/**
 * Destructor
 */
Plane::~Plane(void)
{

}

/**
 * Copy assignment operator
 */
Plane &Plane::operator = (const Plane &plane)
{
    if (&plane != this)
    {
        m_normal = plane.m_normal;
        m_origin = plane.m_origin;
    }

    return *this;
}

/**
 * Move assignment operator
 */
Plane &Plane::operator = (Plane &&plane)
{
    if (&plane != this)
    {
        m_normal = std::move(plane.m_normal);
        m_origin = std::move(plane.m_origin);
    }

    return *this;
}

/**
 * Calculate the distance from the specified point to this plane
 */
double Plane::calcDistance(const Vector3d &point) const
{
    return m_normal.dot(point - m_origin);
}

/**
 * Get the name of this class
 */
std::string Plane::getClassName(void) const
{
    return "Plane";
}

/**
 * Get this plane's normal vector
 */
Vector3d &Plane::getNormal(void)
{
    return m_normal;
}

/**
 * Get this plane's normal vector
 */
Vector3d Plane::getNormal(void) const
{
    return m_normal;
}

/**
 * Return this plane's origin
 */
Vector3d &Plane::getOrigin(void)
{
    return m_origin;
}

/**
 * Return this plane's origin
 */
Vector3d Plane::getOrigin(void) const
{
    return m_origin;
}

/**
 * Function to determine if a ray intersects this shape; returns true if at least one intersection occurs
 */
bool Plane::intersect(const Ray &ray)
{
    RayIntersection intersection;

    return intersect(ray, intersection);
}

/**
 * Calculate the location at which a ray intersects this shape; returns true if at least one intersection
 * occurs
 */
bool Plane::intersect(const Ray &ray,
                      RayIntersection &intersection)
{
    auto &&rayDirection = ray.getDirection();
    auto denom = m_normal.dot(rayDirection);
    if (std::fabs(denom) > 1.0e-10)
    {
        auto &&rayOrigin = ray.getOrigin();
        auto t = (m_origin - rayOrigin).dot(m_normal) / denom;
        if (t >= 0)
        {
            intersection.clear();
            intersection.add(ray.getPosition(t));

            return true;
        }
    }

    return false;
}

/**
 * Calculate the intersection of the plane described by the current object with another plane; algorithm
 * adapted from the following source: http://geomalgorithms.com/a05-_intersect-1.html
 * @return an integer that indicates the following:
 *         = 0, the planes do not intersect
 *         = 1, the planes are coincident
 *         = 2, the planes intersect
 */
int Plane::intersect(const Plane &plane,
                     Line &intersection)
{
    // find the direction vector parallel to the line of intersection
    auto &&direction = m_normal.calcCross(plane.getNormal());
    direction.unitize();

    // the components of the intersection line direction
    auto ux = direction.getX();
    auto uy = direction.getY();
    auto uz = direction.getZ();

    // direction vector absolute values
    auto ax = (ux >= 0.0 ? ux : -ux);
    auto ay = (uy >= 0.0 ? uy : -uy);
    auto az = (uz >= 0.0 ? uz : -uz);

    // test if the two planes are parallel
    if ((ax + ay + az) < 1.0e-10)
    {
        // planes are nearly parallel; test if disjoint or coincide
        auto &&v = plane.m_origin -  m_origin;
        if (m_normal.dot(v) == 0.0)
            return 1; // planes are coincident
        else
            return 0; // planes are parallel
    }

    // planes intersect; first determine max abs coordinate of cross product
    int maxc = (ax > ay) ? ((ax > az) ? 1 : 3) : ((ay > az) ? 2 : 3);

    // next, set the max coordinate to zero to get a point on the intersect line,
    // and solve for the other two
    auto d1 = -m_normal.dot(m_origin);
    auto d2 = -plane.m_normal.dot(plane.m_origin);

    Vector3d point;
    switch (maxc)
    {
        case 1: // intersect with x = 0.0
        point.set(0.0,
                  (d2 * m_normal.getZ() - d1 * plane.m_normal.getZ()) / ux,
                  (d1 * plane.m_normal.getY() - d2 * m_normal.getY()) / ux);
        break;

        case 2: // intersect with y = 0.0
        point.set((d1 * plane.m_normal.getZ() - d2 * m_normal.getZ()) / uy,
                  0.0,
                  (d2 * m_normal.getX() - d1 * plane.m_normal.getX()) / uy);
        break;

        case 3: // intersect with z = 0.0
        point.set((d2 * m_normal.getY() - d1 * plane.m_normal.getY()) / uz,
                  (d1 * plane.m_normal.getX() - d2 * m_normal.getX()) / uz,
                  0.0);
    }

    intersection.setFirstPoint(point);
    intersection.setSecondPoint(point + direction);

    return 2;
}

/**
 * Determine if the plane described by the current object is orthogonal to another plane
 */
bool Plane::orthogonal(const Plane &plane) const
{
    return m_normal.dot(plane.m_normal) == 0.0;
}

/**
 * Determine if the plane described by the current object is parallel to another plane
 */
bool Plane::parallel(const Plane &plane) const
{
    // find the direction vector parallel to the line of intersection
    auto &&direction = m_normal.calcCross(plane.getNormal());

    // the components of the intersection line direction
    auto ux = direction.getX();
    auto uy = direction.getY();
    auto uz = direction.getZ();

    return ((std::fabs(ux) + std::fabs(uy) + std::fabs(uz)) < 1.0e-10);
}

/**
 * Function to project a 3-d point onto this plane; returns a 2-d location on the plane
 */
Vector2d Plane::project(const Vector3d &point3d)
{
    // determine the angle between the normal vector and the z-axis
    Vector3d zAxis(0.0, 0.0, 1.0);
    auto angle = m_normal.calcAngle(zAxis);

    // determine the axis of rotation
    Vector3d axis(m_normal);
    axis.cross(zAxis);
    axis.unitize();

    Vector2d point2d;
    project(point3d, point2d, angle, axis, AngleUnitType::Radians);

    return point2d;
}

/**
 * Function to project a 3-d point onto this plane
 * @param point3d    the input point in 3d space
 * @param point2d    upon success, represents the output 2-d point projected onto this plane
 * @param angle      the angle between this plane's normal axis and the parent-frame z-axis
 * @param axis       the cross-product of this plane's normal axis and the parent-frame z-axis
 * @param angleUnits the angle units of the input angle argument, Degrees or Radians
 */
void Plane::project(Vector3d point3d,
                    Vector2d &point2d,
                    double angle,
                    const Vector3d &axis,
                    const AngleUnitType &angleUnits)
{
    double x, y, z; // z should be zero after the rotation
    point3d -= m_origin;
    point3d.rotate(angle, axis, angleUnits).get(x, y, z);

    point2d.set(x, y);
}

/**
 * Set this plane's normal vector
 */
void Plane::setNormal(const Vector3d &normal)
{
    m_normal = normal;
    m_normal.unitize();
}

/**
 * Set this plane's normal vector
 */
void Plane::setNormal(double x,
                      double y,
                      double z)
{
    m_normal.set(x, y, z);
    m_normal.unitize();
}

/**
 * Set this plane's origin
 */
void Plane::setOrigin(const Vector3d &point)
{
    m_origin = point;
}

/**
 * Set this plane's origin
 */
void Plane::setOrigin(double x,
                      double y,
                      double z)
{
    m_origin.set(x, y, z);
}

/**
 * Function to un-project a 2-d point from this plane back into 3-d coordinates
 */
Vector3d Plane::unproject(const Vector2d &point2d)
{
    // determine the angle between the normal vector and the z-axis
    Vector3d zAxis(0.0, 0.0, 1.0);
    auto angle = m_normal.calcAngle(zAxis);

    // determine the axis of rotation
    Vector3d axis(m_normal);
    axis.cross(zAxis);
    axis.unitize();

    Vector3d point3d;
    unproject(point2d, point3d, angle, axis, AngleUnitType::Radians);

    return point3d;
}

/**
 * Function to un-project a 2-d point from this plane back into 3-d coordinates
 * @param point2d    the input 2-d point located on this plane
 * @param point3d    upon success, represents the output point in 3d space
 * @param angle      the angle between this plane's normal axis and the parent-frame z-axis
 * @param axis       the cross-product of this plane's normal axis and the parent-frame z-axis
 * @param angleUnits the angle units of the input angle argument, Degrees or Radians
 */
void Plane::unproject(Vector2d point2d,
                      Vector3d &point3d,
                      double angle,
                      const Vector3d &axis,
                      const AngleUnitType &angleUnits)
{
    double x, y, z; // z should be zero after the rotation
    point3d.rotate(-angle, axis, angleUnits).get(x, y, z);
    point3d += m_origin;

    point3d.set(x, y, z);
}

}

}
