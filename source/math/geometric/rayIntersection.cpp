#include "rayIntersection.h"

// using namespace declarations
using namespace math::linear_algebra::vector;

namespace math
{

namespace geometric
{

/**
 * Constructor
 */
RayIntersection::RayIntersection(void)
{

}

/**
 * Copy constructor
 */
RayIntersection::RayIntersection(const RayIntersection &intersection)
{
    operator = (intersection);
}

/**
 * Move constructor
 */
RayIntersection::RayIntersection(RayIntersection &&intersection)
{
    operator = (std::move(intersection));
}

/**
 * Destructor
 */
RayIntersection::~RayIntersection(void)
{

}

/**
 * Copy assignment operator
 */
RayIntersection &RayIntersection::operator = (const RayIntersection &intersection)
{
    if (&intersection != this)
    {
        m_points = intersection.m_points;
    }

    return *this;
}

/**
 * Move assignment operator
 */
RayIntersection &RayIntersection::operator = (RayIntersection &&intersection)
{
    if (&intersection != this)
    {
        m_points = std::move(intersection.m_points);
    }

    return *this;
}

/**
 * Add an intersection point
 */
void RayIntersection::add(const Vector3d &point)
{
    m_points.push_back(point);
}

/**
 * Add an intersection point
 */
void RayIntersection::add(double x,
                          double y,
                          double z)
{
    m_points.emplace_back(linear_algebra::vector::Vector3d(x, y, z));
}

/**
 * Clear this object of all intersection points
 */
void RayIntersection::clear(void)
{
    m_points.clear();
}

/**
 * Test for empty object
 */
bool RayIntersection::empty(void) const
{
    return m_points.empty();
}

/**
 * Get the name of this class
 */
std::string RayIntersection::getClassName(void) const
{
    return "RayIntersection";
}

/**
 * Get the intersection points
 */
std::vector<Vector3d> &RayIntersection::getPoints(void)
{
    return m_points;
}

/**
 * Get the number of intersection points
 */
std::size_t RayIntersection::size(void) const
{
    return m_points.size();
}

}

}
