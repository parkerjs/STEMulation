#include "polygonTriangulator.h"
#include <numeric>

// using namespace declarations
using namespace math::linear_algebra::vector;

namespace math
{

namespace geometric
{

namespace shapes
{

namespace utilities
{

/**
 * Constructor
 */
PolygonTriangulator::PolygonTriangulator(void)
: m_pPolygon(nullptr)
{

}

/**
 * Copy constructor
 */
PolygonTriangulator::PolygonTriangulator(const PolygonTriangulator &triangulator)
: m_pPolygon(nullptr)
{
    operator = (triangulator);
}

/**
 * Move constructor
 */
PolygonTriangulator::PolygonTriangulator(PolygonTriangulator &&triangulator)
: m_pPolygon(nullptr)
{
    operator = (std::move(triangulator));
}

/**
 * Destructor
 */
PolygonTriangulator::~PolygonTriangulator(void)
{

}

/**
 * Copy assignment operator
 */
PolygonTriangulator &PolygonTriangulator::operator = (const PolygonTriangulator &triangulator)
{
    if (&triangulator != this)
    {
        m_pPolygon = triangulator.m_pPolygon;
    }

    return *this;
}

/**
 * Move assignment operator
 */
PolygonTriangulator &PolygonTriangulator::operator = (PolygonTriangulator &&triangulator)
{
    if (&triangulator != this)
    {
        m_pPolygon = std::move(triangulator.m_pPolygon);
    }

    return *this;
}

/**
 * begin() override
 */
PolygonTriangulator::iterator PolygonTriangulator::begin(void)
{
    if (m_pPolygon != nullptr)
    {
        auto &&vertices = m_pPolygon->getVertices();

        iterator it(vertices, std::bind(&PolygonTriangulator::findNextTriangle, this, std::placeholders::_1,
                                        std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

        return ++it;
    }

    return end();
}

/**
 * cbegin() override
 */
PolygonTriangulator::const_iterator PolygonTriangulator::cbegin(void) const
{
    return const_cast<PolygonTriangulator *>(this)->begin();
}

/**
 * cend() override
 */
PolygonTriangulator::const_iterator PolygonTriangulator::cend(void) const
{
    return const_cast<PolygonTriangulator *>(this)->end();
}

/**
 * end() override
 */
PolygonTriangulator::iterator PolygonTriangulator::end(void)
{
    return iterator(std::vector<Vector2d>{ { }, { } });
}

/**
 * Find the next triangle
 * @param vertices             the list of vertices which define the polygon
 * @param indices              an index vector which ensures that the polygon is traversed in a
 *                             counter-clockwise order
 * @param triangle             upon success, contains the output triangle
 * @param numVerticesRemaining the number of vertices remaining to be processed
 */
bool PolygonTriangulator::findNextTriangle(const std::vector<Vector2d> &vertices,
                                           std::vector<std::size_t> &indices,
                                           Triangle &triangle,
                                           std::size_t &numVerticesRemaining)
{
    if (numVerticesRemaining < 3)
        return false;

    //  remove size - 2 vertices, creating 1 triangle every time
    auto count = numVerticesRemaining << 1; // error detection
    auto v = numVerticesRemaining - 1;
    while (numVerticesRemaining > 2)
    {
        // if we loop, the polygon probably contains intersecting line segments
        if (--count < 1)
        {
            // probably a bad polygon
            return false;
        }

        // three consecutive vertices in current polygon, <u, v, w>
        auto u = v;
        if (numVerticesRemaining <= u)
            u = 0; // previous

        v = u + 1;
        if (numVerticesRemaining <= v)
            v = 0; // new v

        auto w = v + 1;
        if (numVerticesRemaining <= w)
            w = 0; // next

        bool bValidTriangle = testVertices(vertices, indices, u, v, w);
        if (bValidTriangle)
        {
            // true names of the vertices
            auto a = indices[u];
            auto b = indices[v];
            auto c = indices[w];

            // assign vertices of the triangle
            triangle.setVertices(vertices[a],
                                 vertices[b],
                                 vertices[c]);

            // remove v from remaining polygon
            for (std::size_t s = v, t = v + 1; t < numVerticesRemaining; ++s, ++t)
                indices[s] = indices[t];

            --numVerticesRemaining;

            return true;
        }
    }

    return false;
}

/**
 * Get the name of this class
 */
std::string PolygonTriangulator::getClassName(void) const
{
    return "PolygonTriangulator";
}

/**
 * Function to test a set of vertices to see if they form a valid triangle
 * @param vertices the list of vertices which define the polygon
 * @param indices  an index vector which ensures that the polygon is traversed in a counter-clockwise order
 * @param u, v, w  define the indices associated with the vertices of the triangle under test
 */
bool PolygonTriangulator::testVertices(const std::vector<Vector2d> &vertices,
                                       const std::vector<std::size_t> &indices,
                                       std::size_t u,
                                       std::size_t v,
                                       std::size_t w)
{
    auto ax = vertices[indices[u]].getX();
    auto ay = vertices[indices[u]].getY();
    auto bx = vertices[indices[v]].getX();
    auto by = vertices[indices[v]].getY();
    auto cx = vertices[indices[w]].getX();
    auto cy = vertices[indices[w]].getY();

    if ((bx - ax) * (cy - ay) - (by - ay) * (cx - ax) < 0.0)
        return false;

    for (std::size_t i = 0; i < vertices.size(); ++i)
    {
        if (i == u || i == v || i == w)
            continue;

        auto x = vertices[indices[i]].getX();
        auto y = vertices[indices[i]].getY();

        bool bPointWithinTriangle = Triangle::containsPoint(ax, ay, bx, by, cx, cy, x, y);
        if (bPointWithinTriangle)
            return false;
    }

    return true;
}

/**
 * Function to triangulate a polygon; returns true upon success and input vector of triangles is populated
 * @param polygon   that which is to be triangulated
 * @param triangles upon success, contains a vector triangles that result from the triangulation
 */
void PolygonTriangulator::triangulate(const Polygon &polygon,
                                      std::vector<Triangle> &triangles)
{
    m_pPolygon = &polygon;
    auto &&itTriangle = cbegin();
    while (itTriangle != cend())
    {
        triangles.push_back(*itTriangle);

        ++itTriangle;
    }
}

}

}

}

}
