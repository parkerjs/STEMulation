#include "polygon.h"
#include "polygonTriangulator.h"
#include "rayIntersection.h"
#include "vector2d.h"
#include <cmath>

// file-scoped variables
static constexpr char factoryName[] = "Polygon";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::geometric;
using namespace math::geometric::shapes::utilities;
using namespace math::linear_algebra::vector;

namespace math
{

namespace geometric
{

namespace shapes
{

// register factories...
static FactoryRegistrar<Shape> factory(factoryName, &Polygon::create);

/**
 * Constructor - constructs a triangle with vertices (-1.0, -1.0), (1.0, -1.0), (0.0, 1.0)
 */
Polygon::Polygon(void)
: m_vertices(3)
{
    m_vertices[0].set(-1.0, -1.0);
    m_vertices[1].set( 1.0, -1.0);
    m_vertices[2].set( 0.0,  1.0);
}

/**
 * Copy constructor
 */
Polygon::Polygon(const Polygon &polygon)
{
    operator = (polygon);
}

/**
 * Move constructor
 */
Polygon::Polygon(Polygon &&polygon)
{
    operator = (std::move(polygon));
}

/**
 * Destructor
 */
Polygon::~Polygon(void)
{

}

/**
 * Copy assignment operator
 */
Polygon &Polygon::operator = (const Polygon &polygon)
{
    if (&polygon != this)
    {
        Shape2d::operator = (polygon);

        m_vertices = polygon.m_vertices;
    }

    return *this;
}

/**
 * Move assignment operator
 */
Polygon &Polygon::operator = (Polygon &&polygon)
{
    if (&polygon != this)
    {
        Shape2d::operator = (std::move(polygon));

        m_vertices = std::move(polygon.m_vertices);
    }

    return *this;
}

/**
 * Add a vertex
 */
void Polygon::add(const Vector2d &vertex)
{
    m_vertices.push_back(vertex);
}

/**
 * Add vertices to this polygon
 */
void Polygon::add(const std::vector<linear_algebra::vector::Vector2d> &vertices)
{
    m_vertices.insert(m_vertices.end(), vertices.cbegin(), vertices.cend());
}

/**
 * Add a vertex
 */
void Polygon::add(double x,
                  double y)
{
    m_vertices.emplace_back(Vector2d(x, y));
}

/**
 * Calculate the area of this shape
 */
double Polygon::calcArea(void) const
{
    double area = 0.0;
    auto size = m_vertices.size();
    for (std::size_t p = size - 1, q = 0; q < size; p = q++)
    {
        auto &&px = m_vertices[p].getX();
        auto &&py = m_vertices[p].getY();
        auto &&qx = m_vertices[q].getX();
        auto &&qy = m_vertices[q].getY();

        area += px * qy - qx * py;
    }

    return 0.5 * area;
}

/**
 * Calculate the perimeter of this shape
 */
double Polygon::calcPerimeter(void) const
{
    double perimeter = 0.0;
    auto size = m_vertices.size();
    for (std::size_t i = 0; i < size; ++i)
    {
        auto dx = m_vertices[(i + 1) % size].getX() - m_vertices[i].getX();
        auto dy = m_vertices[(i + 1) % size].getY() - m_vertices[i].getY();
        perimeter += std::sqrt(dx * dx + dy * dy);
    }

    return perimeter;
}

/**
 * clone() function
 */
Polygon *Polygon::clone(void) const
{
    return new Polygon(*this);
}

/**
 * create() function
 */
Polygon *Polygon::create(const Vector2d &vertexOne,
                         const Vector2d &vertexTwo,
                         const Vector2d &vertexThree)
{
    return new Polygon(vertexOne,
                       vertexTwo,
                       vertexThree);
}

/**
 * Get the name of this class
 */
std::string Polygon::getClassName(void) const
{
    return "Polygon";
}

/**
 * Get the factory name of this constructible
 */
std::string Polygon::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Get vertices
 */
std::vector<Vector2d> &Polygon::getVertices(void)
{
    return m_vertices;
}

/**
 * Get this object's vertices
 */
std::vector<Vector2d> Polygon::getVertices(void) const
{
    return m_vertices;
}

/**
 * Calculate the location at which a ray intersects this shape; returns true if at least one intersection
 * occurs
 */
bool Polygon::intersect(const Ray &ray,
                        RayIntersection &intersection)
{
    bool bIntersects = getPlane().intersect(ray);
    if (bIntersects)
    {
        intersection.clear();

        PolygonTriangulator polygonTriangulator;
        auto &&itTriangle = polygonTriangulator.begin();
        while (itTriangle != polygonTriangulator.end())
        {
            auto &&triangle = *itTriangle;
            if (triangle.intersect(ray, intersection))
            {
                return true;
            }

            ++itTriangle;
        }
    }

    return false;
}

/**
 * Triangulate this polygon; returns true upon success and the input vector of triangles is populated
 */
void Polygon::triangulate(std::vector<Triangle> &triangles) const
{
    PolygonTriangulator polygonTriangulator;
    polygonTriangulator.triangulate(*this, triangles);
}

}

}

}
