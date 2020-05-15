#include "drawable_trapezoidalmap.h"
#include <cg3/viewer/opengl_objects/opengl_objects2.h>

Drawable_trapezoidalmap::Drawable_trapezoidalmap(): TrapezoidalMap(){

}

void Drawable_trapezoidalmap::draw() const
{
    for (auto itr : getTrapezoids()){
        if(itr.getTop().p1()==itr.getBottom().p1()){
            cg3::opengl::drawTriangle2(itr.getTop().p1(),itr.getTop().p2(),itr.getBottom().p2(), QColor(0,0,0), 1);
        }
        else if(itr.getTop().p2()==itr.getBottom().p2()){
            cg3::opengl::drawTriangle2(itr.getBottom().p2(), itr.getBottom().p1(), itr.getTop().p1(),QColor(0,0,0),1);
        }
        else
            cg3::opengl::drawQuad2(itr.getTop().p1(),itr.getTop().p2(),itr.getBottom().p2(),itr.getBottom().p1(), QColor(0,0,0),1);
    }
}
cg3::Point3d Drawable_trapezoidalmap::sceneCenter() const{}

double Drawable_trapezoidalmap::sceneRadius() const{}
