#include "drawable_trapezoidalmap.h"
#include <cg3/viewer/opengl_objects/opengl_objects2.h>

Drawable_trapezoidalmap::Drawable_trapezoidalmap(): TrapezoidalMap(){

}

void Drawable_trapezoidalmap::draw() const
{
    for (auto itr : getTrapezoids()){
        if(itr.getTop().p1()==itr.getBottom().p1()){
            cg3::opengl::drawTriangle2(itr.getTop().p1(),itr.getTop().p2(),itr.getBottom().p2(), itr.getColor(), 1, true);
        }
        else if(itr.getTop().p2()==itr.getBottom().p2()){
            cg3::opengl::drawTriangle2(itr.getBottom().p2(), itr.getBottom().p1(), itr.getTop().p1(),itr.getColor(),1, true);
        }
        else
            cg3::opengl::drawQuad2(itr.getTop().p1(),itr.getTop().p2(),itr.getBottom().p2(),itr.getBottom().p1(), itr.getColor(),1, true);
    }
}
cg3::Point3d Drawable_trapezoidalmap::sceneCenter() const{}

double Drawable_trapezoidalmap::sceneRadius() const{}

