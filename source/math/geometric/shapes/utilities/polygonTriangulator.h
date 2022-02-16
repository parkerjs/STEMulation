#ifndef POLYGON_TRIANGULATOR_H
#define POLYGON_TRIANGULATOR_H

#include "iterable.h"
#include "polygon.h"
#include "polygon_triangulation_iterator.h"
#include "triangle.h"

namespace math
{

namespace geometric
{

namespace shapes
{

namespace utilities
{

/**
 * This class implements algorithms to perform polygon triangulation
 */
class PolygonTriangulator
: public attributes::abstract::Iterable<iterators::Iterator, Triangle,
                                        iterators::polygon_triangulation_iterator_tag>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Type alias declarations
     */
    using Vector2d = linear_algebra::vector::Vector2d;

    /**
     * Typedef declarations
     */
    typedef attributes::abstract::Iterable<iterators::Iterator, Triangle,
                                           iterators::polygon_triangulation_iterator_tag>
                                           ::const_iterator const_iterator;
    typedef attributes::abstract::Iterable<iterators::Iterator, Triangle,
                                           iterators::polygon_triangulation_iterator_tag>
                                           ::iterator iterator;

    /**
     * Constructor
     */
    EXPORT_STEM PolygonTriangulator(void);

    /**
     * Copy constructor
     */
    EXPORT_STEM PolygonTriangulator(const PolygonTriangulator &triangulator);

    /**
     * Move constructor
     */
    EXPORT_STEM PolygonTriangulator(PolygonTriangulator &&triangulator);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~PolygonTriangulator(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM PolygonTriangulator &operator = (const PolygonTriangulator &triangulator);

    /**
     * Move assignment operator
     */
    EXPORT_STEM PolygonTriangulator &operator = (PolygonTriangulator &&triangulator);

    /**
     * begin() override
     */
    EXPORT_STEM virtual iterator begin(void) override;

    /**
     * cbegin() override
     */
    EXPORT_STEM virtual const_iterator cbegin(void) const override;

    /**
     * cend() override
     */
    EXPORT_STEM virtual const_iterator cend(void) const override;

    /**
     * end() override
     */
    EXPORT_STEM virtual iterator end(void) override;

private:

    /**
     * Find the next triangle
     * @param vertices             the list of vertices which define the polygon
     * @param indices              an index vector which ensures that the polygon is traversed in a
     *                             counter-clockwise order
     * @param triangle             upon success, contains the output triangle
     * @param numVerticesRemaining the number of vertices remaining to be processed
     */
    EXPORT_STEM virtual bool findNextTriangle(const std::vector<Vector2d> &vertices,
                                              std::vector<std::size_t> &indices,
                                              Triangle &triangle,
                                              std::size_t &numVerticesRemaining) final;

public:

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

private:

    /**
     * Function to test a set of vertices to see if they form a valid triangle
     * @param vertices the list of vertices which define the polygon
     * @param indices  an index vector which ensures that the polygon is traversed in a counter-clockwise order
     * @param u, v, w  define the indices associated with the vertices of the triangle under test
     */
    EXPORT_STEM virtual bool testVertices(const std::vector<Vector2d> &vertices,
                                          const std::vector<std::size_t> &indices,
                                          std::size_t u,
                                          std::size_t v,
                                          std::size_t w) final;

public:

    /**
     * Function to triangulate a polygon; returns true upon success and input vector of triangles is populated
     * @param polygon   that which is to be triangulated
     * @param triangles upon success, contains a vector triangles that result from the triangulation
     */
    EXPORT_STEM virtual void triangulate(const Polygon &polygon,
                                         std::vector<Triangle> &triangles) final;

private:

    /**
     * a pointer to the polygon to be triangulated
     */
    const Polygon *m_pPolygon;
};

}

}

}

}

#endif
