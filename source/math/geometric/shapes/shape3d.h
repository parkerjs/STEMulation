#ifndef SHAPE3D_H
#define SHAPE3D_H

#include "shape.h"

namespace math
{

namespace geometric
{

namespace shapes
{

/**
 * This abstract class represents a 3-d geometric shape
 */
class Shape3d
: public Shape,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Destructor
     */
    virtual ~Shape3d(void) override
    {

    }

    /**
     * Calculate the volume of this shape
     */
    EXPORT_STEM virtual double calcVolume(void) const = 0;

    /**
     * clone() function
     */
    EXPORT_STEM virtual Shape3d *clone(void) const override = 0;

    /**
     * Get the name of this class
     */
    virtual std::string getClassName(void) const override
    {
        return "Shape3d";
    }
};

}

}

}

#endif
