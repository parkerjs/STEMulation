#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "shape2d.h"
#include "vector2d.h"

namespace math
{

namespace geometric
{

namespace shapes
{

/**
 * This class represents a 2d-triangle
 */
class Triangle
: virtual private attributes::abstract::Reflective,
  public Shape2d
{
public:

    /**
     * Type alias declarations
     */
    using Vector2d = linear_algebra::vector::Vector2d;
    using Vector3d = linear_algebra::vector::Vector3d;

    /**
     * Using declarations
     */
    using Shape2d::intersect;

    /**
     * Constructor
     */
    EXPORT_STEM Triangle(void);

    /**
     * Constructor
     */
    EXPORT_STEM Triangle(Vector2d vertices[3]);

    /**
     * Constructor
     */
    EXPORT_STEM Triangle(const Vector2d &vertex1,
                         const Vector2d &vertex2,
                         const Vector2d &vertex3);

    /**
     * Constructor (calculates a surface normal from a set of 3-d vertices that describe the triangle in 3-d
     * space
     */
    EXPORT_STEM Triangle(const Vector3d &vertex1,
                         const Vector3d &vertex2,
                         const Vector3d &vertex3);

    /**
     * Copy constructor
     */
    EXPORT_STEM Triangle(const Triangle &triangle);

    /**
     * Move constructor
     */
    EXPORT_STEM Triangle(Triangle &&triangle);

    /**
     * Destructor
     */
    EXPORT_STEM ~Triangle(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Triangle &operator = (const Triangle &triangle);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Triangle &operator = (Triangle &&triangle);

    /**
     * Calculate the area of this shape
     */
    EXPORT_STEM virtual double calcArea(void) const final;

    /**
     * Calculate the centroid of this shape
     */
    EXPORT_STEM virtual Vector2d calcCentroid(void) const final;

    /**
     * Calculate the ith edge vector from this triangle's vertices
     * @param index the desired edge vector index, where:
     *              = 0 returns the vector described by vertex 2 - vertex 1
     *              = 1 returns the vector described by vertex 3 - vertex 2
     *              = 2 returns the vector described by vertex 1 - vertex 3
     */
    EXPORT_STEM virtual Vector2d calcEdge(std::size_t index) const final;

    /**
     * Calculate the area of this shape
     */
    EXPORT_STEM virtual double calcPerimeter(void) const final;

    /**
     * Calculate a surface normal vector from this triangle's vertices
     */
    static Vector3d calcSurfaceNormal(const Vector3d &vertex1,
                                      const Vector3d &vertex2,
                                      const Vector3d &vertex3);

    /**
     * Calculate a surface normal vector from this triangle's vertices
     */
    static void calcSurfaceNormal(const Vector3d &vertex1,
                                  const Vector3d &vertex2,
                                  const Vector3d &vertex3,
                                  Vector3d &normal);

    /**
     * clone() function
     */
    EXPORT_STEM virtual Triangle *clone(void) const override;

    /**
     * Determine if the specified point is contained within the triangle
     */
    EXPORT_STEM virtual bool containsPoint(const Vector2d &point) const final;

    /**
     * Determine if the specified point is contained within the triangle
     * @param ax the x coordinate of the first vertex
     * @param ay the y coordinate of the first vertex
     * @param bx the x coordinate of the second vertex
     * @param by the y coordinate of the second vertex
     * @param cx the x coordinate of the third vertex
     * @param cy the y coordinate of the third vertex
     * @param px the x coordinate of the point under test
     * @param py the y coordinate of the point under test
     */
    static EXPORT_STEM bool containsPoint(double ax,
                                          double ay,
                                          double bx,
                                          double by,
                                          double cx,
                                          double cy,
                                          double px,
                                          double py);

    /**
     * create() function
     */
    static EXPORT_STEM Triangle *create(void);

    /**
     * Assign this triangle's vertices from a set of vertices specified in 3-d space
     */
    EXPORT_STEM virtual void from3dVertices(const Vector3d &vertex1,
                                            const Vector3d &vertex2,
                                            const Vector3d &vertex3) final;

    /**
     * Assign this triangle's vertices from a set of vertices specified in 3-d space
     */
    EXPORT_STEM virtual void from3dVertices(Vector3d vertices[3]) final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override final;

    /**
     * Get this triangle's ith vertex
     */
    EXPORT_STEM virtual Vector2d &getVertex(std::size_t index) final;

    /**
     * Get this triangle's first vertex
     */
    EXPORT_STEM virtual Vector2d &getVertexOne(void) final;

    /**
     * Get this triangle's first vertex
     */
    EXPORT_STEM virtual Vector2d getVertexOne(void) const final;

    /**
     * Get this triangle's second vertex
     */
    EXPORT_STEM virtual Vector2d &getVertexTwo(void) final;

    /**
     * Get this triangle's second vertex
     */
    EXPORT_STEM virtual Vector2d getVertexTwo(void) const final;

    /**
     * Get this triangle's third vertex
     */
    EXPORT_STEM virtual Vector2d &getVertexThree(void) final;

    /**
     * Get this triangle's third vertex
     */
    EXPORT_STEM virtual Vector2d getVertexThree(void) const final;

    /**
     * Get this triangle's vertices
     */
    EXPORT_STEM virtual void getVertices(Vector2d &vertex1,
                                         Vector2d &vertex2,
                                         Vector2d &vertex3) const final;

    /**
     * Get this triangle's vertices
     */
    EXPORT_STEM virtual void getVertices(Vector2d vertices[3]) const final;

    /**
     * Calculate the location at which a ray intersects this shape; returns true if at least one intersection
     * occurs
     */
    EXPORT_STEM virtual bool intersect(const Ray &ray,
                                       RayIntersection &intersection) override;

    /**
     * Determine if triangle is coplanar with the plane defined by the input normal vector
     */
    EXPORT_STEM virtual bool isCoplanar(const Vector3d &normal) const final;

    /**
     * Set this triangle's ith vertex
     */
    EXPORT_STEM virtual bool setVertex(std::size_t index,
                                       const Vector2d &vertex) final;

    /**
     * Set this triangle's first vertex
     */
    EXPORT_STEM virtual void setVertexOne(const Vector2d &vertex) final;

    /**
     * Set this triangle's second vertex
     */
    EXPORT_STEM virtual void setVertexTwo(const Vector2d &vertex) final;

    /**
     * Set this triangle's third vertex
     */
    EXPORT_STEM virtual void setVertexThree(const Vector2d &vertex) final;

    /**
     * Set this triangle's vertices
     */
    EXPORT_STEM virtual void setVertices(const Vector2d &vertex1,
                                         const Vector2d &vertex2,
                                         const Vector2d &vertex3) final;

    /**
     * Set this triangle's vertices
     */
    EXPORT_STEM virtual void setVertices(Vector2d vertices[3]) final;

    /**
     * Convert this triangle's vertices to a set of 3-d points
     * @param z the z-coordinate of the centroid of the triangle
     */
    EXPORT_STEM virtual void to3dVertices(Vector3d &vertex1,
                                          Vector3d &vertex2,
                                          Vector3d &vertex3) final;

    /**
     * Convert this triangle's vertices to a set of 3-d points
     * @param z the z-coordinate of the centroid of the triangle
     */
    EXPORT_STEM virtual void to3dVertices(Vector3d vertices[3]) final;

private:

    /**
     * the vertices that describe this triangle in 3d space
     */
    Vector2d m_vertices[3];
};

}

}

}

#endif
