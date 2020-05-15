#ifndef DRAWABLE_TRAPEZOIDALMAP_H
#define DRAWABLE_TRAPEZOIDALMAP_H

#include "data_structures/trapezoidalmap.h"

#include <cg3/viewer/interfaces/drawable_object.h>

#include <cg3/utilities/color.h>

class Drawable_trapezoidalmap : public cg3::DrawableObject,  public TrapezoidalMap
{
public:
    Drawable_trapezoidalmap();
    cg3::Point3d sceneCenter() const;
    double sceneRadius() const;
    void draw() const;
};

#endif // DRAWABLE_TRAPEZOIDALMAP_H
