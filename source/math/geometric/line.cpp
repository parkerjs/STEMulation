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
Line::Line(void)
{

}

/**
 * Constructor
 * @param points the pair of points that define this line
 */
Line::Line(linear_algebra::vector::Vector3d points[2])
: Line(points[0],
       points[1])
{

}

/**
 * Constructor
 * @param pointOne the coordinates of the first point which lies on the line
 * @param pointTwo the coordinates of the second point which lies on the line
 */
Line::Line(const Vector3d &pointOne,
           const Vector3d &pointTwo)
{
    m_points[0] = pointOne;
    m_points[1] = pointTwo;
}

/**
 * Constructor
 * @param ray used to construct a line from an origin and direction vector
 */
Line::Line(const Ray &ray)
{
    setFirstPoint(ray.getOrigin());
    setSecondPoint(ray.getOrigin() + ray.getDirection());
}

/**
 * Copy constructor
 */
Line::Line(const Line &line)
{
    operator = (line);
}

/**
 * Move constructor
 */
Line::Line(Line &&line)
{
    operator = (std::move(line));
}

/**
 * Destructor
 */
Line::~Line(void)
{

}

/**
 * Copy assignment operator
 */
Line &Line::operator = (const Line &line)
{
    if (&line != this)
    {
        m_points[0] = line.m_points[0];
        m_points[1] = line.m_points[1];
    }

    return *this;
}

/**
 * Move assignment operator
 */
Line &Line::operator = (Line &&line)
{
    if (&line != this)
    {
        m_points[0] = std::move(line.m_points[0]);
        m_points[1] = std::move(line.m_points[1]);
    }

    return *this;
}

/**
 * Get the name of this class
 */
std::string Line::getClassName(void) const
{
    return "Line";
}

/**
 * Get the direction vector of this line
 */
Vector3d Line::getDirection(void) const
{
    return (m_points[1] - m_points[0]).unitize();
}

/**
 * Get the first point that lies on this line
 */
Vector3d &Line::getFirstPoint(void)
{
    return m_points[0];
}

/**
 * Get the first point that lies on this line
 */
Vector3d Line::getFirstPoint(void) const
{
    return m_points[0];
}

/**
 * Get the pair of points that define this line
 */
void Line::getPoints(Vector3d &firstPoint,
                     Vector3d &secondPoint) const
{
    firstPoint = m_points[0];
    secondPoint = m_points[1];
}

/**
 * Return a 3d-vector representing a position along the line as a function of the parametric value t
 */
void Line::getPosition(double t,
                       Vector3d &position) const
{
    auto x0 = m_points[0].getX();
    auto y0 = m_points[0].getY();
    auto z0 = m_points[0].getZ();

    auto &&direction = getDirection();
    auto dx = direction.getX();
    auto dy = direction.getY();
    auto dz = direction.getZ();

    position.set(x0 + dx * t,
                 y0 + dy * t,
                 z0 + dz * t);
}

/**
 * Return a 3d-vector representing a position along the line as a function of the parametric value t
 */
Vector3d Line::getPosition(double t) const
{
    Vector3d position;

    getPosition(t, position);

    return position;
}

/**
 * Get the second point that lies on this line
 */
Vector3d &Line::getSecondPoint(void)
{
    return m_points[1];
}

/**
 * Get the second point that lies on this line
 */
Vector3d Line::getSecondPoint(void) const
{
    return m_points[1];
}

/**
 * Set the first point that lies on this line
 */
void Line::setFirstPoint(const Vector3d &point)
{
    m_points[0] = point;
}

/**
 * Set the first point that lies on this line
 */
void Line::setFirstPoint(double x,
                         double y,
                         double z)
{
    m_points[0].set(x, y, z);
}

/**
 * Set the pair of points that define this line
 */
void Line::setPoints(const Vector3d &firstPoint,
                     const Vector3d &secondPoint)
{
    setFirstPoint(firstPoint);
    setSecondPoint(secondPoint);
}

/**
 * Set the first point that lies on this line
 */
void Line::setSecondPoint(const Vector3d &point)
{
    m_points[1] = point;
}

/**
 * Set the first point that lies on this line
 */
void Line::setSecondPoint(double x,
                          double y,
                          double z)
{
    m_points[1].set(x, y, z);
}

}

}
