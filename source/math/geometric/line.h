#ifndef LINE_H
#define LINE_H

#include "reflective.h"
#include "vector3d.h"

namespace math
{

namespace geometric
{

// forward declarations
class Ray;

/**
 * This class represents a geometric line in 3-d space
 */
class Line
: virtual private attributes::abstract::Reflective
{
public:

    /**
     * Constructor
     */
    EXPORT_STEM Line(void);

    /**
     * Constructor
     * @param points the pair of points that define this line
     */
    EXPORT_STEM Line(linear_algebra::vector::Vector3d points[2]);

    /**
     * Constructor
     * @param pointOne the coordinates of the first point which lies on the line
     * @param pointTwo the coordinates of the second point which lies on the line
     */
    EXPORT_STEM Line(const linear_algebra::vector::Vector3d &pointOne,
                     const linear_algebra::vector::Vector3d &pointTwo);

    /**
     * Constructor
     * @param ray used to construct a line from an origin and direction vector
     */
    EXPORT_STEM Line(const Ray &ray);

    /**
     * Copy constructor
     */
    EXPORT_STEM Line(const Line &line);

    /**
     * Move constructor
     */
    EXPORT_STEM Line(Line &&line);

    /**
     * Destructor
     */
    EXPORT_STEM ~Line(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Line &operator = (const Line &line);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Line &operator = (Line &&line);

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the direction vector of this line
     */
    EXPORT_STEM virtual linear_algebra::vector::Vector3d getDirection(void) const final;

    /**
     * Get the first point that lies on this line
     */
    EXPORT_STEM virtual linear_algebra::vector::Vector3d &getFirstPoint(void) final;

    /**
     * Get the first point that lies on this line
     */
    EXPORT_STEM virtual linear_algebra::vector::Vector3d getFirstPoint(void) const final;

    /**
     * Get the pair of points that define this line
     */
    EXPORT_STEM virtual void getPoints(linear_algebra::vector::Vector3d &firstPoint,
                                       linear_algebra::vector::Vector3d &secondPoint) const final;

    /**
     * Return a 3d-vector representing a position along the line as a function of the parametric value t
     */
    EXPORT_STEM virtual void getPosition(double t,
                                         linear_algebra::vector::Vector3d &position) const final;

    /**
     * Return a 3d-vector representing a position along the line as a function of the parametric value t
     */
    EXPORT_STEM virtual linear_algebra::vector::Vector3d getPosition(double t) const final;

    /**
     * Get the second point that lies on this line
     */
    EXPORT_STEM virtual linear_algebra::vector::Vector3d &getSecondPoint(void) final;

    /**
     * Get the second point that lies on this line
     */
    EXPORT_STEM virtual linear_algebra::vector::Vector3d getSecondPoint(void) const final;

    /**
     * Set the first point that lies on this line
     */
    EXPORT_STEM virtual void setFirstPoint(const linear_algebra::vector::Vector3d &point) final;

    /**
     * Set the first point that lies on this line
     */
    EXPORT_STEM virtual void setFirstPoint(double x,
                                           double y,
                                           double z) final;

    /**
     * Set the pair of points that define this line
     */
    EXPORT_STEM virtual void setPoints(const linear_algebra::vector::Vector3d &firstPoint,
                                       const linear_algebra::vector::Vector3d &secondPoint) final;

    /**
     * Set the first point that lies on this line
     */
    EXPORT_STEM virtual void setSecondPoint(const linear_algebra::vector::Vector3d &point) final;

    /**
     * Set the first point that lies on this line
     */
    EXPORT_STEM virtual void setSecondPoint(double x,
                                            double y,
                                            double z) final;

private:

    /**
     * the pair of points that define this line
     */
    linear_algebra::vector::Vector3d m_points[2];
};

}

}

#endif
