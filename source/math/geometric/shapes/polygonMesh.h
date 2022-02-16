#ifndef POLYGON_MESH_H
#define POLYGON_MESH_H

#include "inherited_iterator.h"
#include "iterable.h"
#include "loggable.h"
#include "polygon.h"
#include "reverse_iterable.h"
#include "shape3d.h"

namespace math
{

namespace geometric
{

namespace shapes
{

/**
 * This object represents a polyhedral object as a collection of polygons
 */
class PolygonMesh
: public attributes::abstract::Iterable<iterators::Iterator, std::vector<Polygon>,
                                        iterators::inherited_iterator_tag>,
  public attributes::concrete::Loggable<std::string, std::ostream>,
  virtual private attributes::abstract::Reflective,
  public attributes::abstract::ReverseIterable<iterators::Iterator, std::vector<Polygon>,
                                               iterators::inherited_iterator_tag>,
  public Shape3d
{
public:

    /**
     * Typedef declarations
     */
    typedef attributes::abstract::Iterable<iterators::Iterator, PolygonMesh, iterators
                      ::inherited_iterator_tag>::const_iterator const_iterator;
    typedef attributes::abstract::ReverseIterable<iterators::Iterator, PolygonMesh, iterators
                      ::inherited_iterator_tag>::const_reverse_iterator const_reverse_iterator;
    typedef attributes::abstract::Iterable<iterators::Iterator, PolygonMesh, iterators
                      ::inherited_iterator_tag>::iterator iterator;
    typedef attributes::abstract::ReverseIterable<iterators::Iterator, PolygonMesh, iterators
                      ::inherited_iterator_tag>::reverse_iterator reverse_iterator;

    /**
     * Constructor
     */
    EXPORT_STEM PolygonMesh(void);

    /**
     * Copy constructor
     */
    EXPORT_STEM PolygonMesh(const PolygonMesh &polygonMesh);

    /**
     * Move constructor
     */
    EXPORT_STEM PolygonMesh(PolygonMesh &&polygonMesh);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~PolygonMesh(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM PolygonMesh &operator = (const PolygonMesh &polygonMesh);

    /**
     * Move assignment operator
     */
    EXPORT_STEM PolygonMesh &operator = (PolygonMesh &&polygonMesh);

    /**
     * Subscript operator
     */
    EXPORT_STEM virtual Polygon &operator [] (int index) final;

    /**
     * Subscript operator
     */
    EXPORT_STEM virtual Polygon operator [] (int index) const final;

    /**
     * begin() overload
     */
    EXPORT_STEM virtual iterator begin(void) override;

    /**
     * Calculate the area of this shape
     */
    EXPORT_STEM virtual double calcArea(void) const override;

    /**
     * Calculate the volume of this shape
     */
    EXPORT_STEM virtual double calcVolume(void) const override;

    /**
     * cbegin() overload
     */
    EXPORT_STEM virtual const_iterator cbegin(void) const override;

    /**
     * cend() overload
     */
    EXPORT_STEM virtual const_iterator cend(void) const override;

    /**
     * Clear this polygon mesh of its entries
     */
    EXPORT_STEM virtual void clear(void) final;

    /**
     * clone() function
     */
    EXPORT_STEM virtual PolygonMesh *clone(void) const override;

    /**
     * crbegin() overload
     */
    EXPORT_STEM virtual const_reverse_iterator crbegin(void) const override;

    /**
     * create() function
     */
    static EXPORT_STEM PolygonMesh *create(void);

    /**
     * crend() overload
     */
    EXPORT_STEM virtual const_reverse_iterator crend(void) const override;

    /**
     * Test for empty variable registry
     */
    EXPORT_STEM virtual bool empty(void) const final;

    /**
     * end() overload
     */
    EXPORT_STEM virtual iterator end(void) override;

    /**
     * Erase the entry in this polygon mesh associated with the input iterator
     */
    EXPORT_STEM virtual iterator erase(iterator it) final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override final;

    /**
     * Get this object's polygons
     */
    EXPORT_STEM virtual std::vector<Polygon> &getPolygons(void) final;

    /**
     * Calculate the location(s) at which a ray intersects this shape; returns true if at least one
     * intersection occurs
     */
    EXPORT_STEM virtual bool intersect(const Ray &ray,
                                       RayIntersection &intersection) override;

    /**
     * rbegin() overload
     */
    EXPORT_STEM virtual reverse_iterator rbegin(void) override;

    /**
     * Function to read a standard obj file and populate this object's polygons
     */
    EXPORT_STEM virtual bool readObjFile(const std::string &filename);

    /**
     * rend() overload
     */
    EXPORT_STEM virtual reverse_iterator rend(void) override;

    /**
     * Re-size this state vector
     */
    EXPORT_STEM virtual void resize(std::size_t size);

    /**
     * Set this object's polygons
     */
    EXPORT_STEM virtual void setPolygons(const std::vector<Polygon> &polygons) final;

    /**
     * Return the size of this state vector
     */
    EXPORT_STEM virtual std::size_t size(void) const final;

private:

    /**
     * this object's polygons
     */
    std::vector<Polygon> m_polygons;
};

}

}

}

#endif
