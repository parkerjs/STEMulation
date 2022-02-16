#include "math_constants.h"
#include "ray.h"
#include "rayIntersection.h"
#include "sphere.h"
#include <cmath>

// file-scoped variables
static constexpr char factoryName[] = "Sphere";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::geometric;
using namespace math::linear_algebra::vector;

namespace math
{

namespace geometric
{

namespace shapes
{

// register factories...
static FactoryRegistrar<Shape> factory(factoryName, &Sphere::create);

/**
 * Constructor
 */
Sphere::Sphere(void)
: m_origin(0.0, 0.0, 0.0)
{

}

/**
 * Constructor
 */
Sphere::Sphere(double radius)
: m_radius(radius)
{

}

/**
 * Constructor
 */
Sphere::Sphere(const Vector3d &origin,
               double radius)
: m_origin(origin),
  m_radius(radius)
{

}

/**
 * Copy constructor
 */
Sphere::Sphere(const Sphere &sphere)
{
    operator = (sphere);
}

/**
 * Move constructor
 */
Sphere::Sphere(Sphere &&sphere)
{
    operator = (std::move(sphere));
}

/**
 * Destructor
 */
Sphere::~Sphere(void)
{

}

/**
 * Copy assignment operator
 */
Sphere &Sphere::operator = (const Sphere &sphere)
{
    if (&sphere != this)
    {
        Shape3d::operator = (sphere);

        m_origin = sphere.m_origin;
        m_radius = sphere.m_radius;
    }

    return *this;
}

/**
 * Move assignment operator
 */
Sphere &Sphere::operator = (Sphere &&sphere)
{
    if (&sphere != this)
    {
        Shape3d::operator = (std::move(sphere));

        m_origin = sphere.m_origin;
        m_radius = sphere.m_radius;
    }

    return *this;
}

/**
 * Calculate the area of this shape
 */
double Sphere::calcArea(void) const
{
    return 4 * PI * m_radius * m_radius;
}

/**
 * Calculate the volume of this shape
 */
double Sphere::calcVolume(void) const
{
    return 4 * PI * m_radius * m_radius * m_radius / 3.0;
}

/**
 * clone() function
 */
Sphere *Sphere::clone(void) const
{
    return new Sphere(*this);
}

/**
 * create() function
 */
Sphere *Sphere::create(void)
{
    return new Sphere();
}

/**
 * Get the name of this class
 */
std::string Sphere::getClassName(void) const
{
    return "Sphere";
}

/**
 * Get the factory name of this constructible
 */
std::string Sphere::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Get the origin of this shape
 */
Vector3d &Sphere::getOrigin(void)
{
    return m_origin;
}

/**
 * Get the origin of this shape
 */
Vector3d Sphere::getOrigin(void) const
{
    return m_origin;
}

/**
 * Get the sphere's radius
 */
double Sphere::getRadius(void) const
{
    return m_radius;
}

/**
 * Calculate the location at which a ray intersects this shape; returns true if at least one intersection
 * occurs
 */
bool Sphere::intersect(const Ray &ray,
                       RayIntersection &intersection)
{
    // ray direction vector components
    double rdx, rdy, rdz;
    auto &&direction = ray.getDirection();
    direction.get(rdx, rdy, rdz);

    // sphere origin components
    double sox, soy, soz;
    m_origin.get(sox, soy, soz);

    // ray origin components
    double rox, roy, roz;
    ray.getOrigin().get(rox, roy, roz);

    auto a = rdx * rdx + rdy * rdy + rdz * rdz;
    auto b = 2.0 * (rdx * (rox - sox) +  rdy * (roy - soy) +  rdz * (roz - soz));
    auto c = sox * sox + soy * soy + soz * soz + rox * rox + roy * roy + roz * roz -
             2.0 * (sox * rox + soy * roy + soz * roz) - m_radius * m_radius;

    auto discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0.0) // no intersection
        return false;
    else if (discriminant == 0.0) // one intersection
    {
        auto mu = -0.5 * b / a;
        intersection.clear();
        intersection.add(rox + mu * rdx, roy + mu * rdy, roz + mu * rdz);

        return true;
    }
    else if (discriminant > 0.0) // two intersections
    {
        auto sqrtDiscriminant = std::sqrt(discriminant);
        auto mu1 =  0.5 * (-b + sqrtDiscriminant) / a;
        auto mu2 = -0.5 * ( b + sqrtDiscriminant) / a;
        auto nu1 = std::min(mu1, mu2);
        auto nu2 = std::max(mu1, mu2);
        intersection.clear();
        intersection.add(rox + nu1 * rdx, roy + nu1 * rdy, roz + nu1 * rdz);
        intersection.add(rox + nu2 * rdx, roy + nu2 * rdy, roz + nu2 * rdz);

        return true;
    }

    return false;
}

/**
 * Set the origin of this shape
 */
void Sphere::setOrigin(const Vector3d &origin)
{
    m_origin = origin;
}

/**
 * Set the origin of this shape
 */
void Sphere::setOrigin(double x,
                       double y,
                       double z)
{
    m_origin.set(x, y, z);
}

/**
 * Set the sphere's radius
 */
void Sphere::setRadius(double radius)
{
    m_radius = radius;
}

}

}

}
