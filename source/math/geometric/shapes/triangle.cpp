#include "matrix3x3.h"
#include "ray.h"
#include "rayIntersection.h"
#include "triangle.h"
#include <cmath>

// file-scoped variables
static constexpr char factoryName[] = "Triangle";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::geometric;
using namespace math::linear_algebra::matrix;
using namespace math::linear_algebra::vector;
using namespace math::trigonometric;

namespace math
{

namespace geometric
{

namespace shapes
{

// register factories...
static FactoryRegistrar<Shape> factory(factoryName, &Triangle::create);

/**
 * Constructor
 */
Triangle::Triangle(void)
{

}

/**
 * Constructor
 */
Triangle::Triangle(Vector2d vertices[3])
{
    setVertices(vertices[0],
                vertices[1],
                vertices[2]);
}

/**
 * Constructor
 */
Triangle::Triangle(const Vector2d &vertex1,
                   const Vector2d &vertex2,
                   const Vector2d &vertex3)
{
    setVertices(vertex1,
                vertex2,
                vertex3);
}

/**
 * Constructor (calculates a surface normal from a set of 3-d vertices that describe the triangle in 3-d space
 */
Triangle::Triangle(const Vector3d &vertex1,
                   const Vector3d &vertex2,
                   const Vector3d &vertex3)
{
    from3dVertices(vertex1,
                   vertex2,
                   vertex3);
}

/**
 * Copy constructor
 */
Triangle::Triangle(const Triangle &triangle)
{
    operator = (triangle);
}

/**
 * Move constructor
 */
Triangle::Triangle(Triangle &&triangle)
{
    operator = (std::move(triangle));
}

/**
 * Destructor
 */
Triangle::~Triangle(void)
{

}

/**
 * Copy assignment operator
 */
Triangle &Triangle::operator = (const Triangle &triangle)
{
    if (&triangle != this)
    {
        Shape2d::operator = (triangle);

        m_vertices[0] = triangle.m_vertices[0];
        m_vertices[1] = triangle.m_vertices[1];
        m_vertices[2] = triangle.m_vertices[2];
    }

    return *this;
}

/**
 * Move assignment operator
 */
Triangle &Triangle::operator = (Triangle &&triangle)
{
    if (&triangle != this)
    {
        Shape2d::operator = (std::move(triangle));

        m_vertices[0] = std::move(triangle.m_vertices[0]);
        m_vertices[1] = std::move(triangle.m_vertices[1]);
        m_vertices[2] = std::move(triangle.m_vertices[2]);
    }

    return *this;
}

/**
 * Calculate the area of this shape
 */
double Triangle::calcArea(void) const
{
    auto x1 = m_vertices[0].getX(), x2 = m_vertices[1].getX(), x3 = m_vertices[2].getX();
    auto y1 = m_vertices[0].getY(), y2 = m_vertices[1].getY(), y3 = m_vertices[2].getY();

    return 0.5 * std::abs(x1 * (y2 - y3) +
                          x2 * (y3 - y1) +
                          x3 * (y1 - y2));
}

/**
 * Calculate the centroid of this shape
 */
Vector2d Triangle::calcCentroid(void) const
{
    auto x = (m_vertices[0].getX() + m_vertices[1].getX() + m_vertices[2].getX()) / 3.0;
    auto y = (m_vertices[0].getY() + m_vertices[1].getY() + m_vertices[2].getY()) / 3.0;

    return Vector2d(x, y);
}

/**
 * Calculate the ith edge vector from this triangle's vertices
 * @param index the desired edge vector index, where:
 *              = 0 returns the vector described by vertex 2 - vertex 1
 *              = 1 returns the vector described by vertex 3 - vertex 2
 *              = 2 returns the vector described by vertex 1 - vertex 3
 */
Vector2d Triangle::calcEdge(std::size_t index) const
{
    switch (index)
    {
        case 0: return m_vertices[1] - m_vertices[0];
        case 1: return m_vertices[2] - m_vertices[1];
        case 2: return m_vertices[0] - m_vertices[2];
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": " +
                             "Invalid index.");
}

/**
 * Calculate the area of this shape
 */
double Triangle::calcPerimeter(void) const
{
    auto &&lengthEdgeOne = calcEdge(0).magnitude();
    auto &&lengthEdgeTwo = calcEdge(1).magnitude();
    auto &&lengthEdgeThree = calcEdge(2).magnitude();

    return lengthEdgeOne + lengthEdgeTwo + lengthEdgeThree;
}

/**
 * Calculate a surface normal vector from this triangle's vertices
 */
Vector3d Triangle::calcSurfaceNormal(const Vector3d &vertex1,
                                     const Vector3d &vertex2,
                                     const Vector3d &vertex3)
{
    Vector3d normal;
    calcSurfaceNormal(vertex1,
                      vertex2,
                      vertex3,
                      normal);

    return normal;
}

/**
 * Calculate a surface normal vector from this triangle's vertices
 */
void Triangle::calcSurfaceNormal(const Vector3d &vertex1,
                                 const Vector3d &vertex2,
                                 const Vector3d &vertex3,
                                 Vector3d &normal)
{
    normal = vertex2 - vertex1;
    normal.cross(vertex3 - vertex1).unitize();
}

/**
 * clone() function
 */
Triangle *Triangle::clone(void) const
{
    return new Triangle(*this);
}

/**
 * Determine if the specified point is contained within the triangle
 */
bool Triangle::containsPoint(const Vector2d &point) const
{
    auto ax = m_vertices[0].getX();
    auto ay = m_vertices[0].getY();
    auto bx = m_vertices[1].getX();
    auto by = m_vertices[1].getY();
    auto cx = m_vertices[2].getX();
    auto cy = m_vertices[2].getY();
    auto x = point.getX();
    auto y = point.getY();

    return containsPoint(ax, ay, bx, by, cx, cy, x, y);
}

/**
 * Function to test a point to see if it lies within the triangle formed by the specified vertices
 * @param ax the x coordinate of the first vertex
 * @param ay the y coordinate of the first vertex
 * @param bx the x coordinate of the second vertex
 * @param by the y coordinate of the second vertex
 * @param cx the x coordinate of the third vertex
 * @param cy the y coordinate of the third vertex
 * @param x  the x coordinate of the point under test
 * @param y  the y coordinate of the point under test
 */
bool Triangle::containsPoint(double ax,
                             double ay,
                             double bx,
                             double by,
                             double cx,
                             double cy,
                             double x,
                             double y)
{
    return (cx - bx) * (y - by) - (cy - by) * (x - bx) >= 0.0 &&
           (bx - ax) * (y - ay) - (by - ay) * (x - ax) >= 0.0 &&
           (ax - cx) * (y - cy) - (ay - cy) * (x - cx) >= 0.0;
}

/**
 * create() function
 */
Triangle *Triangle::create(void)
{
    return new Triangle();
}

/**
 * Assign this triangle's vertices from a set of vertices specified in 3-d space
 */
void Triangle::from3dVertices(const Vector3d &vertex1,
                              const Vector3d &vertex2,
                              const Vector3d &vertex3)
{
    // calculate the normal vector of the plane that contains the triangle
    m_plane.setNormal(calcSurfaceNormal(vertex1,
                                        vertex2,
                                        vertex3));

    // calculate the 3d-centroid of the triangle specified by the three vertices and set that as the origin
    // of the plane containing the triangle
    auto x = (vertex1.getX() + vertex2.getX() + vertex3.getX()) / 3.0;
    auto y = (vertex1.getY() + vertex2.getY() + vertex3.getY()) / 3.0;
    auto z = (vertex1.getZ() + vertex2.getZ() + vertex3.getZ()) / 3.0;
    m_plane.setOrigin(x, y, z);

    // determine the angle between the normal vector and the z-axis
    Vector3d zAxis(0.0, 0.0, 1.0);
    auto angle = m_plane.getNormal().calcAngle(zAxis);

    // determine the axis of rotation
    Vector3d axis(m_plane.getNormal());
    axis.cross(zAxis);
    axis.unitize();

    m_plane.project(vertex1, m_vertices[0], angle, axis, AngleUnitType::Radians);
    m_plane.project(vertex2, m_vertices[1], angle, axis, AngleUnitType::Radians);
    m_plane.project(vertex3, m_vertices[2], angle, axis, AngleUnitType::Radians);
}

/**
 * Assign this triangle's vertices from a set of vertices specified in 3-d space
 */
void Triangle::from3dVertices(Vector3d vertices[3])
{
    calcSurfaceNormal(vertices[0],
                      vertices[1],
                      vertices[2]);
}

/**
 * Get the name of this class
 */
std::string Triangle::getClassName(void) const
{
    return "Triangle";
}

/**
 * Get the factory name of this constructible
 */
std::string Triangle::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Get this triangle's ith vertex
 */
Vector2d &Triangle::getVertex(std::size_t index)
{
    if (index < 3)
        return m_vertices[index];

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": " +
                             "Invalid index.");
}

/**
 * Get this triangle's first vertex
 */
Vector2d &Triangle::getVertexOne(void)
{
    return m_vertices[0];
}

/**
 * Get this triangle's first vertex
 */
Vector2d Triangle::getVertexOne(void) const
{
    return m_vertices[0];
}

/**
 * Get this triangle's second vertex
 */
Vector2d &Triangle::getVertexTwo(void)
{
    return m_vertices[1];
}

/**
 * Get this triangle's second vertex
 */
Vector2d Triangle::getVertexTwo(void) const
{
    return m_vertices[1];
}

/**
 * Get this triangle's third vertex
 */
Vector2d &Triangle::getVertexThree(void)
{
    return m_vertices[2];
}

/**
 * Get this triangle's third vertex
 */
Vector2d Triangle::getVertexThree(void) const
{
    return m_vertices[2];
}

/**
 * Get this triangle's vertices
 */
void Triangle::getVertices(Vector2d &vertex1,
                           Vector2d &vertex2,
                           Vector2d &vertex3) const
{
    vertex1 = m_vertices[0];
    vertex2 = m_vertices[1];
    vertex3 = m_vertices[2];
}

/**
 * Get this triangle's vertices
 */
void Triangle::getVertices(Vector2d vertices[3]) const
{
    vertices[0] = m_vertices[0];
    vertices[1] = m_vertices[1];
    vertices[2] = m_vertices[2];
}

/**
 * Calculate the location at which a ray intersects this shape; returns true if at least one intersection
 * occurs
 */
bool Triangle::intersect(const Ray &ray,
                         RayIntersection &intersection)
{
    // first determine if the ray intersects the plane containing the triangle
    auto &&normal = m_plane.getNormal();
    auto &&origin = m_plane.getOrigin();
    auto &&rayDirection = ray.getDirection();
    auto &&rayOrigin = ray.getOrigin();

    // assuming vectors are all normalized
    auto denom = normal.dot(rayDirection);
    if (denom > 0.0)
    {
        auto t = normal.dot(origin - rayOrigin) / denom;
        if (t >= 0.0)
        {
            auto &&point3d = ray.getPosition(t);
            auto &&point2d = m_plane.project(point3d);

            if (containsPoint(point2d))
            {
                intersection.clear();
                intersection.add(point3d);

                return true;
            }
        }
    }

    return false;
}

/**
 * Determine if triangle is coplanar with the plane defined by the input normal vector
 */
bool Triangle::isCoplanar(const Vector3d &normal) const
{
    return m_plane.getNormal().dot(normal) == 1.0;
}

/**
 * Set this triangle's ith vertex
 */
bool Triangle::setVertex(std::size_t index,
                         const Vector2d &vertex)
{
    if (index < 3)
        m_vertices[index] = vertex;

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": " +
                             "Invalid index.");
}

/**
 * Set this triangle's first vertex
 */
void Triangle::setVertexOne(const Vector2d &vertex)
{
    m_vertices[0] = vertex;
}

/**
 * Set this triangle's second vertex
 */
void Triangle::setVertexTwo(const Vector2d &vertex)
{
    m_vertices[1] = vertex;
}

/**
 * Set this triangle's third vertex
 */
void Triangle::setVertexThree(const Vector2d &vertex)
{
    m_vertices[2] = vertex;
}

/**
 * Set this triangle's vertices
 */
void Triangle::setVertices(const Vector2d &vertex1,
                           const Vector2d &vertex2,
                           const Vector2d &vertex3)
{
    m_vertices[0] = vertex1;
    m_vertices[1] = vertex2;
    m_vertices[2] = vertex3;
}

/**
 * Set this triangle's vertices
 */
void Triangle::setVertices(Vector2d vertices[3])
{
    setVertices(vertices[0],
                vertices[1],
                vertices[2]);
}

/**
 * Convert this triangle's vertices to a set of 3-d points
 * @param z the z-coordinate of the centroid of the triangle
 */
void Triangle::to3dVertices(Vector3d &vertex1,
                            Vector3d &vertex2,
                            Vector3d &vertex3)
{
    // determine the angle between the normal vector and the z-axis
    Vector3d zAxis(0.0, 0.0, 1.0);
    auto angle = zAxis.calcAngle(m_plane.getNormal());

    // determine the axis of rotation
    Vector3d axis(zAxis);
    axis.cross(m_plane.getNormal());
    axis.unitize();

    m_plane.unproject(m_vertices[0], vertex1, angle, axis, AngleUnitType::Radians);
    m_plane.unproject(m_vertices[1], vertex2, angle, axis, AngleUnitType::Radians);
    m_plane.unproject(m_vertices[2], vertex3, angle, axis, AngleUnitType::Radians);
}

/**
 * Convert this triangle's vertices to a set of 3-d points
 * @param z the z-coordinate of the centroid of the triangle
 */
void Triangle::to3dVertices(Vector3d vertices[3])
{
    to3dVertices(vertices[0],
                 vertices[1],
                 vertices[2]);
}

}

}

}
