#include "drawable_trapezoidalmap_dataset.h"
#include "data_structures/dag_dataset.h"
#include <cg3/viewer/opengl_objects/opengl_objects2.h>

DrawableTrapezoidalMapDataset::DrawableTrapezoidalMapDataset() :
    pointColor(80, 180, 80),
    segmentColor(80, 80, 180),
    pointSize(5),
    segmentSize(3)
{

}

void DrawableTrapezoidalMapDataset::draw() const
{
    for (const cg3::Point2d& p : getPoints()) {
        cg3::opengl::drawPoint2(p, pointColor, static_cast<int>(pointSize));
    }
    for (const cg3::Segment2d& seg : getSegments()) {
        cg3::opengl::drawLine2(seg.p1(), seg.p2(), segmentColor, static_cast<int>(segmentSize));
    }
//    std::unordered_map<cg3::Point2d, Trapezoid>::iterator itr;
//    for (auto itr : TrapezoidalMapDataset::getTrapezoids()){
//        cg3::opengl::drawLine2(itr.second.getTop().p1(),itr.second.getBottom().p1(), QColor(0,0,0), static_cast<int>(segmentSize)-2);
//        cg3::opengl::drawLine2(itr.second.getTop().p2(),itr.second.getBottom().p2(), QColor(0,0,0), static_cast<int>(segmentSize)-2);
//        //++itr;
//    }
    for (auto itr : TrapezoidalMap::getTrapezoids()){
        if(itr.getTop().p1()==itr.getBottom().p1()){
            cg3::opengl::drawTriangle2(itr.getTop().p1(),itr.getTop().p2(),itr.getBottom().p2(), QColor(0,0,0), 1);
        }
        else if(itr.getTop().p2()==itr.getBottom().p2()){
            cg3::opengl::drawTriangle2(itr.getBottom().p2(), itr.getBottom().p1(), itr.getTop().p1(),QColor(0,0,0),1);
        }
        else
            cg3::opengl::drawQuad2(itr.getTop().p1(),itr.getTop().p2(),itr.getBottom().p2(),itr.getBottom().p1(), QColor(0,0,0),1);
        //++itr;
    }
}

cg3::Point3d DrawableTrapezoidalMapDataset::sceneCenter() const
{
    const cg3::BoundingBox2& boundingBox = this->getBoundingBox();
    return cg3::Point3d(boundingBox.center().x(), boundingBox.center().y(), 0);
}

double DrawableTrapezoidalMapDataset::sceneRadius() const
{
    const cg3::BoundingBox2& boundingBox = this->getBoundingBox();
    return boundingBox.diag();
}

unsigned int DrawableTrapezoidalMapDataset::getSegmentSize() const
{
    return segmentSize;
}

void DrawableTrapezoidalMapDataset::setSegmentSize(unsigned int value)
{
    segmentSize = value;
}

unsigned int DrawableTrapezoidalMapDataset::getPointSize() const
{
    return pointSize;
}

void DrawableTrapezoidalMapDataset::setPointSize(unsigned int value)
{
    pointSize = value;
}

const cg3::Color& DrawableTrapezoidalMapDataset::getSegmentColor() const
{
    return segmentColor;
}

void DrawableTrapezoidalMapDataset::setSegmentColor(const cg3::Color &value)
{
    segmentColor = value;
}

const cg3::Color& DrawableTrapezoidalMapDataset::getPointColor() const
{
    return pointColor;
}

void DrawableTrapezoidalMapDataset::setPointColor(const cg3::Color &value)
{
    pointColor = value;
}
