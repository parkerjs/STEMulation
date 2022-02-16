#include "line.h"
#include "ray.h"

// using namespace declarations
using namespace math::linear_algebra::vector;

namespace math
{

namespace geometric
{

/**
 * Constructor
 */
Ray::Ray(void)
: m_direction(1.0, 0.0, 0.0),
  m_origin(0.0, 0.0, 0.0)
{

}

/**
 * Constructor
 * @param direction the ray's direction vector
 * @param origin    the coordinates of the ray's origin
 */
Ray::Ray(const Vector3d &direction,
         const Vector3d &origin)
: m_direction(direction),
  m_origin(origin)
{

}

/**
 * Copy constructor
 */
Ray::Ray(const Ray &ray)
{
    operator = (ray);
}

/**
 * Move constructor
 */
Ray::Ray(Ray &&ray)
{
    operator = (std::move(ray));
}

/**
 * Destructor
 */
Ray::~Ray(void)
{

}

/**
 * Implicit conversion to Line
 */
Ray::operator Line (void)
{
    Line line;
    line.setFirstPoint(m_origin);
    line.setSecondPoint(m_origin + m_direction);

    return line;
}

/**
 * Copy assignment operator
 */
Ray &Ray::operator = (const Ray &ray)
{
    if (&ray != this)
    {
        m_direction = ray.m_direction;
        m_origin = ray.m_origin;
    }

    return *this;
}

/**
 * Move assignment operator
 */
Ray &Ray::operator = (Ray &&ray)
{
    if (&ray != this)
    {
        m_direction = std::move(ray.m_direction);
        m_origin = std::move(ray.m_origin);
    }

    return *this;
}

/**
 * Get the name of this class
 */
std::string Ray::getClassName(void) const
{
    return "Ray";
}

/**
 * Get the direction vector of this ray
 */
Vector3d &Ray::getDirection(void)
{
    return m_direction;
}

/**
 * Get the direction vector of this ray
 */
Vector3d Ray::getDirection(void) const
{
    return m_direction;
}

/**
 * Get the origin of this ray
 */
Vector3d &Ray::getOrigin(void)
{
    return m_origin;
}

/**
 * Get the origin of this ray
 */
Vector3d Ray::getOrigin(void) const
{
    return m_origin;
}

/**
 * Return a 3d-vector representing a position along the ray as a function of the parametric value t
 */
void Ray::getPosition(double t,
                      Vector3d &position) const
{
    position.set(m_origin.getX() + m_direction.getX() * t,
                 m_origin.getY() + m_direction.getY() * t,
                 m_origin.getZ() + m_direction.getZ() * t);
}

/**
 * Return a 3d-vector representing a position along the ray as a function of the parametric value t
 */
Vector3d Ray::getPosition(double t) const
{
    Vector3d position;

    getPosition(t, position);

    return position;
}

/**
 * Set the direction of this ray
 */
void Ray::setDirection(const Vector3d &direction)
{
    m_direction = direction;
}

/**
 * Set the direction of this ray
 */
void Ray::setDirection(double x,
                       double y,
                       double z)
{
    m_direction.set(x, y, z);
}

/**
 * Set the origin of this ray
 */
void Ray::setOrigin(const Vector3d &origin)
{
    m_origin = origin;
}

/**
 * Set the origin of this ray
 */
void Ray::setOrigin(double x,
                    double y,
                    double z)
{
    m_origin.set(x, y, z);
}

}

}
