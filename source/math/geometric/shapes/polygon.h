#ifndef POLYGON_H
#define POLYGON_H

#include "shape2d.h"
#include "vector3d.h"

namespace math
{

namespace geometric
{

namespace shapes
{

// forward declarations
class Triangle;

/**
 * This class represents a polygon
 */
class Polygon
: virtual private attributes::abstract::Reflective,
  public Shape2d
{
public:

    /**
     * Using declarations
     */
    using Shape2d::intersect;
    using Vector2d = linear_algebra::vector::Vector2d;

    /**
     * Constructor - constructs a triangle with vertices (-1.0, -1.0), (1.0, -1.0), (0.0, 1.0)
     */
    EXPORT_STEM Polygon(void);

    /**
     * Constructor
     */
    template<typename ... Vertex>
    Polygon(const Vector2d &vertexOne,
            const Vector2d &vertexTwo,
            const Vector2d &vertexThree,
            Vertex && ... vertices)
    {
        setVertices(vertexOne, vertexTwo, vertexThree, std::forward<Vertex>(vertices) ...);
    }

    /**
     * Constructor
     */
    template<std::size_t N, typename std::enable_if<(N >= 3), int>::type = 0>
    Polygon(Vector2d vertices[N])
    : m_vertices(vertices, vertices + N)
    {

    }

    /**
     * Copy constructor
     */
    EXPORT_STEM Polygon(const Polygon &polygon);

    /**
     * Move constructor
     */
    EXPORT_STEM Polygon(Polygon &&polygon);

    /**
     * Destructor
     */
    EXPORT_STEM ~Polygon(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Polygon &operator = (const Polygon &polygon);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Polygon &operator = (Polygon &&polygon);

    /**
     * Add a vertex
     */
    EXPORT_STEM virtual void add(const Vector2d &vertex) final;

    /**
     * Add vertices to this polygon
     */
    EXPORT_STEM virtual void add(const std::vector<Vector2d> &vertices) final;

    /**
     * Add a vertex
     */
    EXPORT_STEM virtual void add(double x,
                                 double y) final;

    /**
     * Calculate the area of this shape
     */
    EXPORT_STEM virtual double calcArea(void) const override;

    /**
     * Calculate the perimeter of this shape
     */
    EXPORT_STEM virtual double calcPerimeter(void) const override;

    /**
     * clone() function
     */
    EXPORT_STEM virtual Polygon *clone(void) const override;

    /**
     * create() function
     */
    static EXPORT_STEM Polygon *create(const Vector2d &vertexOne,
                                       const Vector2d &vertexTwo,
                                       const Vector2d &vertexThree);

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override final;

    /**
     * Get vertices
     */
    EXPORT_STEM std::vector<Vector2d> &getVertices(void);

    /**
     * Get vertices
     */
    EXPORT_STEM std::vector<Vector2d> getVertices(void) const;

    /**
     * Calculate the location at which a ray intersects this shape; returns true if at least one intersection
     * occurs
     */
    EXPORT_STEM virtual bool intersect(const Ray &ray,
                                       RayIntersection &intersection) override;

    /**
     * Set this object's vertices
     */
    template<typename ... Vertex>
    inline void setVertices(const Vector2d &vertexOne,
                            const Vector2d &vertexTwo,
                            const Vector2d &vertexThree,
                            Vertex && ... vertices)
    {
        m_vertices = { vertexOne, vertexTwo, vertexThree, Vector2d(vertices) ... };
    }

    /**
     * Triangulate this polygon; returns true upon success and the input vector of triangles is populated
     */
    EXPORT_STEM virtual void triangulate(std::vector<Triangle> &triangles) const final;

private:

    /**
     * the vertices describing this polygon
     */
    std::vector<Vector2d> m_vertices;
};

}

}

}

#endif
